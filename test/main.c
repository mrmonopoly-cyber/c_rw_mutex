#include "../c_rw_mutex.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Helper function for printing test status
void print_test_result(const char *test_name, int result) {
  if (result == EXIT_SUCCESS) {
    printf("%s: PASSED\n", test_name);
  } else {
    printf("%s: FAILED\n", test_name);
  }
}

void test_mutex_init() {
  int data = 42;
  RW_mutex mutex;
  int result = mutex_init(&data, sizeof(data), &mutex);
  print_test_result("Test mutex_init", result);
  assert(result == EXIT_SUCCESS);
}

void test_read_lock() {
  int data = 42;
  RW_mutex mutex;
  R_lock r_lock;
  mutex_init(&data, sizeof(data), &mutex);

  int result = mutex_r_lock(&mutex, &r_lock);
  print_test_result("Test mutex_r_lock", result);
  assert(result == EXIT_SUCCESS);

  int read_data = 0;
  result = r_lock_data_read(&r_lock, &read_data, sizeof(read_data));
  print_test_result("Test r_lock_data_read", result);
  assert(result == EXIT_SUCCESS);
  assert(read_data == data);

  result = mutex_r_unlock(&r_lock);
  print_test_result("Test mutex_r_unlock", result);
  assert(result == EXIT_SUCCESS);
}

void test_write_lock() {
  int data = 42;
  RW_mutex mutex;
  W_lock w_lock;
  mutex_init(&data, sizeof(data), &mutex);

  int result = mutex_w_lock(&mutex, &w_lock);
  print_test_result("Test mutex_w_lock", result);
  assert(result == EXIT_SUCCESS);

  // Write new data
  int new_data = 100;
  result = rw_lock_data_write(&w_lock, &new_data, sizeof(new_data));
  print_test_result("Test rw_lock_data_write", result);
  assert(result == EXIT_SUCCESS);

  // Read the newly written data
  int read_data = 0;
  result = rw_lock_data_read(&w_lock, &read_data, sizeof(read_data));
  print_test_result("Test rw_lock_data_read", result);
  assert(result == EXIT_SUCCESS);
  assert(read_data == new_data);

  result = mutex_w_unlock(&w_lock);
  print_test_result("Test mutex_w_unlock", result);
  assert(result == EXIT_SUCCESS);
}

void test_read_after_write() {
  int data = 42;
  RW_mutex mutex;
  R_lock r_lock;
  W_lock w_lock;
  mutex_init(&data, sizeof(data), &mutex);

  // Acquire write lock and modify data
  int result = mutex_w_lock(&mutex, &w_lock);
  assert(result == EXIT_SUCCESS);

  int new_data = 100;
  result = rw_lock_data_write(&w_lock, &new_data, sizeof(new_data));
  assert(result == EXIT_SUCCESS);
  result = mutex_w_unlock(&w_lock);
  assert(result == EXIT_SUCCESS);

  // Acquire read lock and check if the new data is visible
  result = mutex_r_lock(&mutex, &r_lock);
  assert(result == EXIT_SUCCESS);

  int read_data = 0;
  result = r_lock_data_read(&r_lock, &read_data, sizeof(read_data));
  print_test_result("Test read after write", result);
  assert(result == EXIT_SUCCESS);
  assert(read_data == new_data);

  result = mutex_r_unlock(&r_lock);
  assert(result == EXIT_SUCCESS);
}

void test_data_cmp() {
  int data = 12345;
  RW_mutex mutex;
  R_lock r_lock;
  W_lock w_lock;

  mutex_init(&data, sizeof(data), &mutex);

  // Acquire read lock and compare data
  int result = mutex_r_lock(&mutex, &r_lock);
  assert(result == EXIT_SUCCESS);

  result = r_lock_data_cmp(&r_lock, &data, sizeof(data));
  print_test_result("Test r_lock_data_cmp", result);
  assert(result == EXIT_SUCCESS);

  result = mutex_r_unlock(&r_lock);
  assert(result == EXIT_SUCCESS);

  // Modify data and compare with write lock
  int new_data = 54321;
  result = mutex_w_lock(&mutex, &w_lock);
  assert(result == EXIT_SUCCESS);
  result = rw_lock_data_write(&w_lock, &new_data, sizeof(new_data));
  assert(result == EXIT_SUCCESS);

  result = rw_lock_data_cmp(&w_lock, &new_data, sizeof(new_data));
  print_test_result("Test rw_lock_data_cmp", result);
  assert(result == EXIT_SUCCESS);

  result = mutex_w_unlock(&w_lock);
  assert(result == EXIT_SUCCESS);
}

int main() {
  printf("Running RW_mutex tests...\n");

  test_mutex_init();
  test_read_lock();
  test_write_lock();
  test_read_after_write();
  test_data_cmp();

  printf("All tests completed.\n");
  return 0;
}
