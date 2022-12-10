#pragma once

#include "core2/STL.h"

namespace core2 {

template <typename Key, typename Value, typename Hash = std::hash<Key>>
class HashMap {
private:
  using ElementType = std::pair<const Key, Value>;

  using ElementListType = std::list<ElementType>;
  ElementListType elements;

  Hash hash;
  std::vector<std::list<typename ElementListType::iterator>> bucketLists;

  static constexpr const size_t MIN_SIZE = 32;

  size_t hashKey(const Key& key) const {
    return hash(key) % bucketLists.size();
  }

  void tryExpand() {
    if (elements.size() >= bucketLists.size() * 2) {
      size_t currentSize = bucketLists.size();

      // Rebuild bucketLists
      bucketLists.clear();
      bucketLists.resize(currentSize * 2);

      for (auto elementIterator = elements.begin();
           elementIterator != elements.end(); elementIterator++) {
        bucketLists[hashKey(elementIterator->first)].emplace_front(
            elementIterator);
      }
    }
  }

public:
  explicit HashMap() : bucketLists(MIN_SIZE) {}

  template <typename Key_, typename Value_>
  void insert(Key_&& key, Value_&& value) {
    tryExpand();

    size_t index = hashKey(key);

    std::list<typename ElementListType::iterator>& bucketList =
        bucketLists[index];

    // If already exist in map, simply update value
    for (const auto& elementIterator : bucketList) {
      if (elementIterator->first == key) {
        elementIterator->second = std::forward<Value_>(value);
        return;
      }
    }

    // Otherwise, add a new element
    elements.emplace_front(std::forward<Key_>(key),
                           std::forward<Value_>(value));
    bucketList.emplace_front(elements.begin());
  };

  Value& get(const Key& key) {
    size_t index = hashKey(key);

    std::list<typename ElementListType::iterator>& bucketList =
        bucketLists[index];

    // If already exist in map, simply update value
    for (const auto& elementIterator : bucketList) {
      if (elementIterator->first == key) {
        return elementIterator->second;
      }
    }

    throw std::logic_error("Could not find element");
  }

  size_t getSize() const { return elements.size(); }

  auto begin() { return elements.begin(); }

  auto end() { return elements.end(); }
};

} // namespace core2
