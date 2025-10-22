#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
// --- Configuration ---
#define BLOCK_SIZE_KB 64  // Size of each block in Kilobytes
#define BLOCK_SIZE (BLOCK_SIZE_KB * 1024)
#define PROC_MAPS_PATH "/proc/self/maps"

// Structure to hold the details of each memory block
typedef struct {
  char name[20];
  int* address;
  size_t size;
  char os_map_range[30];  // Stores the range read from /proc/maps
} MemoryBlock;

// Global array to store block information
MemoryBlock blocks[4];

// --- Helper Functions ---

/**
 * @brief Reads /proc/self/maps and finds the mapped range for a given address.
 * @param addr The start address reported by mmap.
 * @param output_buffer Buffer to store the resulting range (e.g.,
 * 7fff0000-7fff4000).
 */
void Get_os_map_range(void* addr, char* output_buffer, size_t bufsize) {
  FILE* f = fopen(PROC_MAPS_PATH, "r");
  char line[256];
  uint64_t start, end;

  // Default value if not found
  if (bufsize > 0) snprintf(output_buffer, bufsize, "NOT_FOUND");

  if (!f) {
    if (bufsize > 0) snprintf(output_buffer, bufsize, "FILE_ERROR");
    return;
  }

  // Read the maps file line by line
  while (fgets(line, sizeof(line), f)) {
    // Read the start and end addresses from the line using 64-bit hex scans
    if (sscanf(line,
               "%" SCNx64 "-"
               "%" SCNx64,
               &start, &end) == 2) {
      // Check if the reported address falls within this kernel-mapped range
      if ((uintptr_t)addr >= (uintptr_t)start &&
          (uintptr_t)addr < (uintptr_t)end) {
        // Found the corresponding line in /proc/self/maps
        // Format the output range using 64-bit hex
        if (bufsize > 0)
          snprintf(output_buffer, bufsize,
                   "%" PRIx64
                   "-"
                   "%" PRIx64,
                   start, end);
        fclose(f);
        return;
      }
    }
  }

  fclose(f);
}

// --- Core Task Function ---

/**
 * @brief Creates four distinct memory blocks using mmap().
 * * REQUIRED POSIX FUNCTION: mmap(void *addr, size_t length, int prot, int
 * flags, int fd, off_t offset)
 * * TASK: Create the following four memory blocks, storing the returned address
 * in the blocks array:
 * * 1. BLOCK 0 (Auto-Assigned): Use NULL for the addr parameter.
 * 2. BLOCK 1 (Another Auto-Assigned): Use NULL for the addr parameter again.
 * 3. BLOCK 2 (Hinted Address 1): Use a specific, high-address hint (e.g.,
 * 0x70000000) for the addr parameter, using the MAP_FIXED flag to make the
 * request mandatory.
 * 4. BLOCK 3 (Hinted Address 2): Use a different specific, high-address hint
 * (e.g., 0x80000000) for the addr parameter, using the MAP_FIXED flag.
 * * NOTE: MAP_FIXED is aggressive and may fail if the address is already in
 * use.
 */
void Create_memory_blocks() {
  // Define the configuration for each block
  const char* names[4] = {"BLOCK_0_AUTO_A", "BLOCK_1_AUTO_B", "BLOCK_2_HINT_1",
                          "BLOCK_3_HINT_2"};
  size_t sizes[4] = {BLOCK_SIZE, BLOCK_SIZE * 2, BLOCK_SIZE, BLOCK_SIZE};
  void* addrs[4] = {NULL, NULL, (void*)0x70000000UL, (void*)0x80000000UL};
  int flags[4] = {MAP_PRIVATE | MAP_ANONYMOUS, MAP_PRIVATE | MAP_ANONYMOUS,
                  MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                  MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED};
  int prot = PROT_READ | PROT_WRITE;
  int data_values[4] = {100, 200, 300, 400};

  for (int i = 0; i < 4; i++) {
    // Initialize name and size
    strncpy(blocks[i].name, names[i], sizeof(blocks[i].name) - 1);
    blocks[i].name[sizeof(blocks[i].name) - 1] = '\0';
    blocks[i].size = sizes[i];

    // Attempt to map the memory
    void* result = mmap(addrs[i], blocks[i].size, prot, flags[i], -1, 0);

    if (result == MAP_FAILED) {
      // Mapping failed. Provide a safe fallback so the program doesn't crash
      // when dereferencing the pointer for logging. Record failure in
      // os_map_range.
      blocks[i].address = malloc(sizeof(int));
      if (blocks[i].address) {
        blocks[i].address[0] = data_values[i];
      }
      strncpy(blocks[i].os_map_range, "MAP_FAILED",
              sizeof(blocks[i].os_map_range) - 1);
      blocks[i].os_map_range[sizeof(blocks[i].os_map_range) - 1] = '\0';
    } else {
      // Success: store the mapped address and write the first integer
      blocks[i].address = (int*)result;
      blocks[i].address[0] = data_values[i];

      // Gather the OS-reported range for this mapping
      Get_os_map_range(result, blocks[i].os_map_range,
                       sizeof(blocks[i].os_map_range));
    }
  }
}

/**
 * @brief Prints a single block entry for the final log line.
 * @param pid The PID of the current process (kept for compatibility if needed).
 * @param block Pointer to the MemoryBlock to print.
 */
void Print_result_log(pid_t pid, MemoryBlock* block) {
  printf("%s=%p/%zu/%d/%s|", block->name, (void*)block->address, block->size,
         block->address[0], block->os_map_range);
}

int main() {
  pid_t pid = getpid();

  // Create the memory blocks
  Create_memory_blocks();

  // Print the required log line using a per-block function
  printf("LOG_START|PID=%d|", pid);
  for (int i = 0; i < 4; i++) {
    Print_result_log(pid, &blocks[i]);
  }
  printf("LOG_END\n");

  // Wait for validation script to run. The script will kill the process.
  // We use a safe sleep here to prevent the process from exiting immediately.
  sleep(10);

  // Clean up
  for (int i = 0; i < 4; i++) {
    if (blocks[i].address != MAP_FAILED) {
      munmap(blocks[i].address, blocks[i].size);
    }
  }

  return 0;
}
