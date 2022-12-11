#pragma once

#include <pthread.h>
#include <thread>

#include "core2/Log.h"
#include "core2/allocator/bit_allocator/Utility.h"

namespace core2::BitAllocator {

class PageQueue {
private:
  pthread_spinlock_t lock;

  void* buffer[8];

  unsigned begin, end, size;

public:
  PageQueue() : begin(0), end(0), size(0) { pthread_spin_init(&lock, 0); }

  ~PageQueue() { pthread_spin_destroy(&lock); }

  bool push(void* page) noexcept {
    pthread_spin_lock(&lock);

    if (size == 8) {
      pthread_spin_unlock(&lock);
      return false;
    }

    buffer[end++] = page;
    if (end == 8) {
      end = 0;
    }

    size++;

    pthread_spin_unlock(&lock);

    return true;
  }

  void* pop() noexcept {
    pthread_spin_lock(&lock);

    if (size == 0) {
      pthread_spin_unlock(&lock);
      return nullptr;
    }

    void* addr = buffer[begin++];
    if (begin == 8) {
      begin = 0;
    }

    size--;

    pthread_spin_unlock(&lock);

    return addr;
  }
};

class PageServer {
private:
  PageQueue pageQueue;

public:
  PageServer() {
    std::thread daemon([&]() {
      while (true) {
        void* page = allocateAlignedChunk(CHUNK_SIZE);
        while (!pageQueue.push(page))
          ;
      }
    });

    daemon.detach();
  }

  void* getPage() {
    void* page;

    if ((page = pageQueue.pop()) == nullptr) {
      return allocateAlignedChunk(CHUNK_SIZE);
    }

    return page;
  }
};

} // namespace core2::BitAllocator
