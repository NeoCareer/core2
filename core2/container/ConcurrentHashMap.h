#pragma once

#include <functional>
#include <vector>

namespace core2 {

template <typename K, typename V> class Bucket {
private:
  shared_mutex mux;
  std::list<std::pair<K, V>> data;
  using ListIt = typename list<pair<K, V>>::iterator;

public:
  template <typename K_, typename V_, typename H_>
  friend class ConcurrentHashMap;

  bool insert(const K& k, const V& v) {
    unique_lock lock(mux);

    for (ListIt it = data.begin(); it != data.end(); ++it) {
      // Marks an insertion
      if (it->first == k) {
        it->second = v;
        return false;
      }
    }

    // It is an insertion
    data.emplace_front(k, v);
    return true;
  }

  optional<V> get(const K& k) {
    shared_lock lock(mux);

    for (ListIt it = data.begin(); it != data.end(); ++it) {
      // Find it
      if (it->first == k) {
        return it->second;
      }
    }

    return nullopt;
  }

  bool erase(const K& k) {
    unique_lock lock(mux);

    for (ListIt it = data.begin(); it != data.end(); ++it) {
      // Find it
      if (it->first == k) {
        data.erase(it);
        return true;
      }
    }

    return false;
  }
};

template <typename K, typename V, typename Hash = std::hash<K>>
class ConcurrentHashMap {
private:
  Hash hash;
  mutex mux;
  std::vector<Bucket<K, V>> bucketList;

  size_t accessing = 0;
  bool rehashing = false;

  condition_variable readyToRehash;
  condition_variable readyToContinue;

  size_t size_ = 0;

  inline size_t h(const K& k) { return hash(k) % bucketList.size(); }

  void enter() {
    unique_lock lock(mux);

    if (size_ >= bucketList.size() && !rehashing) {
      rehashing = true;

      readyToRehash.wait(lock, [this]() { return accessing == 0; });

      // Do rehashing here
      vector<Bucket<K, V>> newBucketList(2 * bucketList.size());

      for (Bucket<K, V>& bucket : bucketList) {
        for (pair<K, V>& entry : bucket.data) {
          newBucketList[hash(entry.first) % newBucketList.size()]
              .data.push_front(entry);
        }
      }

      bucketList.swap(newBucketList);

      rehashing = false;

      readyToContinue.notify_all();
    }

    // Wait until it is time to continue
    readyToContinue.wait(lock, [this]() { return !rehashing; });

    accessing++;
  }

  void exit(int diff) {
    unique_lock lock(mux);

    if (--accessing == 0) {
      readyToRehash.notify_one();
    }

    if (diff >= 0) {
      size_ += static_cast<size_t>(diff);
    } else {
      size_--;
    }
  }

public:
  ConcurrentHashMap() : bucketList(1) {}

  bool insert(const K& k, const V& v) {
    enter();

    bool success = bucketList[h(k)].insert(k, v);

    exit(success ? 1 : 0);

    return success;
  }

  optional<V> get(const K& k) {
    enter();

    optional<V> v = bucketList[h(k)].get(k);

    exit(0);

    return v;
  }

  bool erase(const K& k) {
    enter();

    bool success = bucketList[h(k)].erase(k);

    exit(success ? -1 : 0);

    return success;
  }
};

} // namespace core2