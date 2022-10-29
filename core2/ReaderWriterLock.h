#pragma once

#include "core2/STL.h"

namespace core2 {

class ReaderWriterLock {
public:
  enum class ReaderWriterLockMode { READER_FIRST, WRITER_FIRST, BALANCED };

private:
  static constexpr unsigned HEADCOUNT = 1 << 6;

  std::mutex mux;
  unsigned readerCount = 0;
  bool writing = false;
  unsigned readerHeadcount = HEADCOUNT;
  unsigned queueingWriter = 0;

  std::condition_variable readyToRead;
  std::condition_variable readyToWrite;

  const ReaderWriterLockMode mode;

public:
  explicit ReaderWriterLock(
      const ReaderWriterLockMode mode_ = ReaderWriterLockMode::BALANCED)
      : mode(mode_) {}

  void readerLock() {
    std::unique_lock lock(mux);

    // Wait until no one is writing and there is headcount
    readyToRead.wait(lock, [this]() {
      switch (mode) {
      case ReaderWriterLockMode::READER_FIRST:
        return !writing;
      case ReaderWriterLockMode::WRITER_FIRST:
        return !writing && queueingWriter == 0;
      case ReaderWriterLockMode::BALANCED:
        if (queueingWriter == 0) {
          readerHeadcount = HEADCOUNT;
        }

        return !writing && readerHeadcount > 0;
      default:
        throw std::logic_error("Unrecognized mode");
      }
    });

    readerCount++;

    if (mode == ReaderWriterLockMode::BALANCED) {
      readerHeadcount--;
    }
  }

  void readerUnlock() {
    std::unique_lock lock(mux);

    readerCount--;

    if (readerCount == 0) {
      readyToWrite.notify_one();
    }
  }

  void writerLock() {
    std::unique_lock lock(mux);
    queueingWriter++;

    readyToWrite.wait(lock, [this]() { return readerCount == 0 && !writing; });

    queueingWriter--;

    readerHeadcount = HEADCOUNT;
    writing = true;
  }

  void writerUnlock() {
    std::unique_lock lock(mux);

    assert(writing);
    assert(readerCount == 0);

    writing = false;

    const auto notifyReaderFirst = [this]() {
      readyToRead.notify_all();
      readyToWrite.notify_one();
    };

    const auto notifyWriterFirst = [this]() {
      readyToWrite.notify_one();
      readyToRead.notify_all();
    };

    switch (mode) {
    case ReaderWriterLockMode::READER_FIRST:
      notifyReaderFirst();
      break;
    case ReaderWriterLockMode::WRITER_FIRST:
      notifyWriterFirst();
      break;
    case ReaderWriterLockMode::BALANCED:
      if (readerHeadcount < HEADCOUNT / 4) {
        notifyWriterFirst();
      } else {
        notifyReaderFirst();
      }
      break;
    default:
      throw std::logic_error("Unrecognized mode");
    }
  }
};

} // namespace core2