#include "lib/ReaderWriterLock.h"
#include <array>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <math.h>
#include <queue>
#include <random>
#include <shared_mutex>
#include <sys/time.h>
#include <thread>
#include <time.h>
#include <vector>

using namespace std;
using namespace std::chrono;

long long millsecond() {
  auto millisecSinceEpoch =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();
  return millisecSinceEpoch;
}

static float millDuration() {
  static long long previousTime = 0;

  float retVal = (static_cast<float>(millsecond() - previousTime)) / 1000;
  previousTime = millsecond();
  return retVal;
}

constexpr const unsigned SIZE = 1 << 10;

struct Matrix {
  array<unsigned, SIZE> data;
  unsigned checkSum;
};

static unsigned randInt() {
  static std::random_device randomDevice;
  static std::default_random_engine engine(randomDevice());
  static std::uniform_int_distribution<unsigned> distribution(0, 0xffff);

  return distribution(engine);
}

constexpr const unsigned MOD = 1000000007;

void generate(Matrix &matrix) {
  matrix.checkSum = 0;

  for (unsigned i = 0; i < SIZE; ++i) {
    matrix.data[i] = randInt();
    matrix.checkSum = (matrix.checkSum + matrix.data[i]) % MOD;
  }
}

bool verify(const Matrix &matrix) {
  unsigned checkSum = 0;
  for (const unsigned value : matrix.data) {
    checkSum = (checkSum + value) % MOD;
  }

  return matrix.checkSum == checkSum;
}

ReaderWriterLock readFirstLock(ReaderWriterLock::ReaderWriterLockMode::READER_FIRST);
ReaderWriterLock writeFirstLock(ReaderWriterLock::ReaderWriterLockMode::WRITER_FIRST);
ReaderWriterLock balancedLock(ReaderWriterLock::ReaderWriterLockMode::BALANCED);
std::mutex regularLock;
std::shared_mutex regularSharedLock;

constexpr const unsigned STEP = 1 << 6;
constexpr const unsigned WRITERS = 1 << 4;
constexpr const unsigned READERS = 1 << 10;

void readFirstProducer(Matrix &matrix) {
  for (unsigned i = 0; i < STEP; ++i) {
    readFirstLock.writerLock();
    generate(matrix);
    readFirstLock.writerUnlock();
  }
}

void readFirstConsumer(const Matrix &matrix) {
  for (unsigned i = 0; i < STEP; ++i) {
    readFirstLock.readerLock();
    if (!verify(matrix)) {
      cerr << "Verification failed" << endl;
      exit(1);
    }
    readFirstLock.readerUnlock();
  }
}

void writeFirstProducer(Matrix& matrix) {
  for (unsigned i = 0; i < STEP; ++i) {
    writeFirstLock.writerLock();
    generate(matrix);
    writeFirstLock.writerUnlock();
  }
}

void writeFirstConsumer(const Matrix &matrix) {
  for (unsigned i = 0; i < STEP; ++i) {
    writeFirstLock.readerLock();
    if (!verify(matrix)) {
      cerr << "Verification failed" << endl;
      exit(1);
    }
    writeFirstLock.readerUnlock();
  }
}

void balancedProducer(Matrix& matrix) {
  for (unsigned i = 0; i < STEP; ++i) {
    balancedLock.writerLock();
    generate(matrix);
    balancedLock.writerUnlock();
  }
}

void balancedConsumer(const Matrix &matrix) {
  for (unsigned i = 0; i < STEP; ++i) {
    balancedLock.readerLock();
    if (!verify(matrix)) {
      cerr << "Verification failed" << endl;
      exit(1);
    }
    balancedLock.readerUnlock();
  }
}

void regularProducer(Matrix& matrix) {
  for (unsigned i = 0; i < STEP; ++i) {
    regularLock.lock();
    generate(matrix);
    regularLock.unlock();
  }
}

void regularConsumer(const Matrix &matrix) {
  for (unsigned i = 0; i < STEP; ++i) {
    regularLock.lock();
    if (!verify(matrix)) {
      cerr << "Verification failed" << endl;
      exit(1);
    }
    regularLock.unlock();
  }
}

void regularSharedProducer(Matrix& matrix) {
  for (unsigned i = 0; i < STEP; ++i) {
    regularSharedLock.lock();
    generate(matrix);
    regularSharedLock.unlock();
  }
}

void regularSharedConsumer(const Matrix &matrix) {
  for (unsigned i = 0; i < STEP; ++i) {
    regularSharedLock.lock_shared();
    if (!verify(matrix)) {
      cerr << "Verification failed" << endl;
      exit(1);
    }
    regularSharedLock.unlock_shared();
  }
}

void waitThreads(vector<thread> &threads) {
  for (auto &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

template <typename Producer, typename Consumer>
void benchmark(Matrix &matrix, Producer &&producer, Consumer &&consumer) {
  vector<thread> threads;

  for (unsigned i = 0; i < WRITERS; ++i) {
    threads.emplace_back(std::forward<Producer>(producer), std::ref(matrix));
  }

  for (unsigned i = 0; i < READERS; ++i) {
    threads.emplace_back(std::forward<Consumer>(consumer), std::ref(matrix));
  }

  waitThreads(threads);
}

int main() {
  millDuration();

  Matrix matrix;

  benchmark(matrix, readFirstProducer, readFirstConsumer);
  cout << "Read-first lock: " << millDuration() << endl;

  benchmark(matrix, writeFirstProducer, writeFirstConsumer);
  cout << "Write-first lock: " << millDuration() << endl;

  benchmark(matrix, balancedProducer, balancedConsumer);
  cout << "Balanced lock: " << millDuration() << endl;

  benchmark(matrix, regularProducer, regularConsumer);
  cout << "Regular lock: " << millDuration() << endl;

  benchmark(matrix, regularSharedProducer, regularSharedConsumer);
  cout << "Regular shared lock: " << millDuration() << endl;

  return 0;
}
