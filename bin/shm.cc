#include "lib/Lib.h"

using namespace std;

constexpr const unsigned SIZE = 1 << 16;

void createSharedMemory(const char* const shmName) {
  int memId = shm_open(shmName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  if (memId == -1) {
    perror("shm_open");
    exit(1);
  }

  ftruncate(memId, SIZE);

  void *memory =
      mmap(nullptr, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memId, 0);

  close(memId);

  if (memory == nullptr) {
    perror("mmap");
    exit(1);
  }

  memcpy(memory, "Hello world!", 13);
  shm_unlink(shmName);
}

void openSharedMemory(const char* const shmName) {
  int memId = shm_open(shmName, O_RDONLY);

  if (memId == -1) {
    perror("shm_open");
    exit(1);
  }

  void *memory =
      mmap(nullptr, SIZE, PROT_READ, MAP_SHARED, memId, 0);

  close(memId);

  if (memory == nullptr) {
    perror("mmap");
    exit(1);
  }

  cout << static_cast<char *>(memory) << endl;
  munmap(memory, SIZE);
}

int main(int argc, char **argv) {
  assert(argc == 3);

  string_view command(argv[1]);

  if (command == "create") {
    createSharedMemory(argv[2]);
  } else if (command == "open") {
    openSharedMemory(argv[2]);
  } else {
    throw logic_error("Should not reach here");
  }

  return 0;
}
