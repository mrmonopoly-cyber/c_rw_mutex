#include "c_rw_mutex.h"
#include <stdlib.h>
#include <string.h>

#define STATIC_ASSERT(COND, MSG) typedef char static_assertion_##MSG[(COND) ? 1 : -1]

typedef struct {
    void *data_ptr;
    uint8_t data_size;
    uint8_t reading;
    uint8_t writing : 1;
    uint8_t mu_lock : 1;
}RW_mutex_t;

struct common_lock{
    RW_mutex_t* mutex;
};

typedef union{
    RW_mutex* raw_data;
    RW_mutex_t* full_data;
}mutex_conv;

typedef union{
    W_lock* w_lock;
    R_lock* r_lock;
    struct common_lock* c_lock;
}lock_conv;

STATIC_ASSERT(sizeof(RW_mutex) == sizeof(RW_mutex_t), mutex_header_imp_same_size);
STATIC_ASSERT(sizeof(R_lock) == sizeof(lock_conv), mutex_header_imp_same_size);
STATIC_ASSERT(sizeof(W_lock) == sizeof(lock_conv), mutex_header_imp_same_size);

//private
#define CHECK_INIT(MUTEX) if (!MUTEX || !MUTEX->data_ptr) { return EXIT_FAILURE;}


static int 
common_lock_data_read(struct common_lock* lock, void* o_buffer, uint8_t buffer_size)
{
    if (!lock || !o_buffer || !buffer_size || lock->mutex->writing || 
            buffer_size < lock->mutex->data_size) {
        return EXIT_FAILURE;
    }

    memcpy(o_buffer, lock->mutex->data_ptr, lock->mutex->data_size);
    return EXIT_SUCCESS;
}
static int 
common_lock_data_cmp(struct common_lock* lock, void* buffer, uint8_t buffer_size)
{
    uint8_t res = -1; 

    if (!lock || !buffer || !buffer_size || lock->mutex->writing || 
            buffer_size < lock->mutex->data_size) {
        return EXIT_FAILURE;
    }

    res = memcmp(buffer, lock->mutex->data_ptr, lock->mutex->data_size);

    return !!res;
}

static int 
common_lock_data_write(struct common_lock* lock, void* buffer, uint8_t buffer_size)
{
    if (!lock || !buffer || !buffer_size || lock->mutex->mu_lock || 
            lock->mutex->writing || lock->mutex->reading || buffer_size < lock->mutex->data_size) {
        return EXIT_FAILURE;
    }

    lock->mutex->mu_lock = 1;
    lock->mutex->writing= 1;
    lock->mutex->mu_lock = 0;
    memcpy(lock->mutex->data_ptr, buffer, lock->mutex->data_size);

    return EXIT_SUCCESS;
}

//public
int mutex_init(void* data,uint8_t data_size, RW_mutex *o_mutex)
{
    if (!o_mutex || !data || !data_size) {
        return EXIT_FAILURE;
    }

    mutex_conv conv = {.raw_data =o_mutex};

    if (conv.full_data->mu_lock || conv.full_data->data_ptr) {
        return EXIT_FAILURE;
    }

    conv.full_data->mu_lock= 1;

    conv.full_data->data_size = data_size;
    conv.full_data->data_ptr = data;
    conv.full_data->reading = 0;
    conv.full_data->writing = 0;


    conv.full_data->mu_lock= 0;
    return EXIT_SUCCESS;
}

int mutex_r_lock(RW_mutex* mutex, R_lock* o_lock)
{
    if (!mutex || !o_lock) {
        return EXIT_FAILURE;
    }
    
    mutex_conv conv = {.raw_data =mutex};
    lock_conv conv_l = {.r_lock = o_lock};
    CHECK_INIT(conv.full_data);

    conv.full_data->mu_lock = 1;
    if (conv.full_data->writing) {
        return EXIT_FAILURE;
    }
    conv.full_data->reading++;

    conv_l.c_lock->mutex = conv.full_data;

    conv.full_data->mu_lock = 0;
    return EXIT_SUCCESS;
}

int mutex_w_lock(RW_mutex* mutex, W_lock* o_lock)
{
    if (!mutex || !o_lock) {
        return EXIT_FAILURE;
    }
    
    mutex_conv conv = {.raw_data =mutex};
    lock_conv conv_l = {.w_lock = o_lock};
    CHECK_INIT(conv.full_data);

    conv.full_data->mu_lock = 1;
    if (conv.full_data->writing || conv.full_data->reading) {
        return EXIT_FAILURE;
    }
    conv.full_data->writing=1;

    conv_l.c_lock->mutex = conv.full_data;

    conv.full_data->mu_lock = 0;
    return EXIT_SUCCESS;
}

int mutex_w_unlock(W_lock* lock)
{
    lock_conv conv = { .w_lock = lock};
    if (!lock || !conv.c_lock->mutex) {
        return EXIT_FAILURE;
    }

    CHECK_INIT(conv.c_lock->mutex);

    if (conv.c_lock->mutex->mu_lock) {
        return EXIT_FAILURE;
    }

    conv.c_lock->mutex->mu_lock =1;
    conv.c_lock->mutex->writing =0;
    conv.c_lock->mutex->mu_lock =0;
    conv.c_lock->mutex = NULL;

    return EXIT_SUCCESS;
}

int mutex_r_unlock(R_lock* lock)
{
    if (!lock) {
        return EXIT_FAILURE;
    }
    lock_conv conv_l = {.r_lock = lock};

    CHECK_INIT(conv_l.c_lock->mutex);

    if (conv_l.c_lock->mutex->mu_lock) {
        return EXIT_FAILURE;
    }

    conv_l.c_lock->mutex->mu_lock =1;
    conv_l.c_lock->mutex->reading--;
    conv_l.c_lock->mutex->mu_lock =0;
    conv_l.c_lock->mutex = NULL;

    return EXIT_SUCCESS;
}

int r_lock_data_read(R_lock* lock, void* o_buffer, uint8_t buffer_size)
{
    if (!lock) {
        return EXIT_FAILURE;
    }
    lock_conv conv = {.r_lock = lock};
    CHECK_INIT(conv.c_lock->mutex);
    return common_lock_data_read(conv.c_lock, o_buffer, buffer_size);
}

int r_lock_data_cmp(R_lock* lock, void* buffer, uint8_t buffer_size)
{
    if (!lock) {
        return EXIT_FAILURE;
    }
    lock_conv conv = {.r_lock = lock};
    CHECK_INIT(conv.c_lock->mutex);
    return common_lock_data_cmp(conv.c_lock, buffer, buffer_size);
}

int rw_lock_data_read(W_lock* lock, void* o_buffer, uint8_t buffer_size)
{
    if (!lock) {
        return EXIT_FAILURE;
    }
    lock_conv conv = {.w_lock = lock};
    CHECK_INIT(conv.c_lock->mutex);
    return common_lock_data_read(conv.c_lock, o_buffer, buffer_size);
}

int rw_lock_data_cmp(W_lock* lock, void* buffer, uint8_t buffer_size)
{
    if (!lock) {
        return EXIT_FAILURE;
    }
    lock_conv conv = {.w_lock = lock};
    CHECK_INIT(conv.c_lock->mutex);
    return common_lock_data_cmp(conv.c_lock, buffer, buffer_size);
}

int rw_lock_data_write(W_lock* lock, void* buffer, uint8_t buffer_size)
{
    if (!lock) {
        return EXIT_FAILURE;
    }
    lock_conv conv = {.w_lock = lock};
    CHECK_INIT(conv.c_lock->mutex);
    return common_lock_data_write(conv.c_lock, buffer, buffer_size);
}
