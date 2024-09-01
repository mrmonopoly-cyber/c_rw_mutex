#ifndef __RW_MUTEX__
#define __RW_MUTEX__

#include <stdint.h>

typedef struct {uint8_t data[sizeof(void*) + 8];} RW_mutex;
typedef struct RW_lock RW_lock;
typedef struct R_lock R_lock;

/*
 * INFO: The library does NOT allocate memory to create the mutex or the lock.
 * Also, when unlocking, it does not free any memory. If you allocate in 
 * HEAP the RW_mutex, the RW_lock, or the R_lock, you have to free them by yourself.
 */

/*
 * INIT a mutex, creating a wrapper for the variable.
 * data: the data to protect.
 * o_mutex: a pointer to an already allocated mutex.
 *
 * RETURN:
 *  - EXIT_SUCCESS if ok.
 *  - EXIT_FAILURE if there was an error.  
 */
int mutex_init(void* data, uint8_t data_size, RW_mutex *o_mutex);

/*
 * Acquire a read lock on the mutex.
 * mutex: Pointer to the mutex.
 * o_lock: Pointer to an already allocated R_lock structure.
 *
 * RETURN:
 *  - EXIT_SUCCESS if the lock was successfully acquired.
 *  - EXIT_FAILURE if there was an error.
 */
int mutex_r_lock(RW_mutex* mutex, R_lock* o_lock);

/*
 * Acquire a write lock on the mutex.
 * mutex: Pointer to the mutex.
 * o_lock: Pointer to an already allocated RW_lock structure.
 *
 * RETURN:
 *  - EXIT_SUCCESS if the lock was successfully acquired.
 *  - EXIT_FAILURE if there was an error.
 */
int mutex_w_lock(RW_mutex* mutex, RW_lock* o_lock);

/*
 * Release a previously acquired write lock.
 * lock: Pointer to the RW_lock structure holding the write lock.
 *
 * RETURN:
 *  - EXIT_SUCCESS if the lock was successfully released.
 *  - EXIT_FAILURE if there was an error.
 */
int mutex_w_unlock(RW_lock* lock);

/*
 * Release a previously acquired read lock.
 * lock: Pointer to the R_lock structure holding the read lock.
 *
 * RETURN:
 *  - EXIT_SUCCESS if the lock was successfully released.
 *  - EXIT_FAILURE if there was an error.
 */
int mutex_r_unlock(R_lock* lock);

/*
 * Read the protected data using the read lock.
 * lock: Pointer to the R_lock structure.
 * o_buffer: Buffer where the read data will be stored.
 * buffer_size: Size of the output buffer.
 *
 * RETURN:
 *  - EXIT_SUCCESS if the data was successfully read.
 *  - EXIT_FAILURE if there was an error.
 */
int r_lock_data_read(R_lock* lock, void* o_buffer, uint8_t buffer_size);

/*
 * Compare the protected data with the data in the provided buffer using the read lock.
 * lock: Pointer to the R_lock structure.
 * buffer: Buffer containing the data to compare with.
 * buffer_size: Size of the buffer to compare.
 *
 * RETURN:
 *  - EXIT_SUCCESS if the data matches.
 *  - EXIT_FAILURE if there is a mismatch or an error occurs.
 */
int r_lock_data_cmp(R_lock* lock, void* buffer, uint8_t buffer_size);

/*
 * Read the protected data using the write lock.
 * lock: Pointer to the RW_lock structure.
 * o_buffer: Buffer where the read data will be stored.
 * buffer_size: Size of the output buffer.
 *
 * RETURN:
 *  - EXIT_SUCCESS if the data was successfully read.
 *  - EXIT_FAILURE if there was an error.
 */
int rw_lock_data_read(RW_lock* lock, void* o_buffer, uint8_t buffer_size);

/*
 * Compare the protected data with the data in the provided buffer using the write lock.
 * lock: Pointer to the RW_lock structure.
 * buffer: Buffer containing the data to compare with.
 * buffer_size: Size of the buffer to compare.
 *
 * RETURN:
 *  - EXIT_SUCCESS if the data matches.
 *  - EXIT_FAILURE if there is a mismatch or an error occurs.
 */
int rw_lock_data_cmp(RW_lock* lock, void* buffer, uint8_t buffer_size);

/*
 * Write data to the protected area using the write lock.
 * lock: Pointer to the RW_lock structure.
 * buffer: Buffer containing the data to write.
 * buffer_size: Size of the data to write.
 *
 * RETURN:
 *  - EXIT_SUCCESS if the data was successfully written.
 *  - EXIT_FAILURE if there was an error.
 */
int rw_lock_data_write(RW_lock* lock, void* buffer, uint8_t buffer_size);

#endif // !__RW_MUTEX__

