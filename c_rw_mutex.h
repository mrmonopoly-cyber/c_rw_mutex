#ifndef __RW_MUTEX__
#define __RW_MUTEX__

#include <stdint.h>

typedef struct RW_mutex RW_mutex;
typedef struct RW_lock RW_lock;
typedef struct R_lock R_lock;

/*
 * INFO: The library does NOT allocate memory to create the mutex or the lock
 * also when unlcking it does not free any memory. If you allocate in 
 * HEAP the RW_mutex, the RW_lock or the R_lock you have to free them by yourself
 */

/*
 * INIT a mutex creating a wrapper for the variable 
 * data: the data to protect
 * o_mutex: a pointer to an already allocated mutex
 * (the reason for that is to allow the user to decide if the lock is going to be in HEAP or 
 * in STACK
 *
 * RETURN:
 *  - EXIT_SUCCESS if ok
 *  - EXIR_FAILURE if there was an error  
 */
int mutex_init(void* data, RW_mutex *o_mutex);

int mutex_r_lock(RW_mutex* mutex, R_lock* o_lock);
int mutex_w_lock(RW_mutex* mutex, RW_lock* o_lock);

int mutex_w_unlock(RW_lock* lock);
int mutex_r_unlock(R_lock* lock);

int lock_read(R_lock* lock, void* buffer, uint8_t buffer_size);
int lock_write(RW_lock* lock, void* buffer, uint8_t buffer_size);

#endif // !__RW_MUTEX__
