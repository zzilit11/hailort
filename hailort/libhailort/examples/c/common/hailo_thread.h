/**
 * Copyright (c) 2019-2025 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file hailo_thread.h
 * Common Linux thread related functions.
 **/

#ifndef _HAILO_THREAD_H_
#define _HAILO_THREAD_H_

#include "hailo/hailort.h"

#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

typedef pthread_t hailo_thread;
typedef void* thread_return_type;
typedef atomic_int hailo_atomic_int;

#define MICROSECONDS_PER_MILLISECOND (1000)

hailo_status hailo_create_thread(thread_return_type(*func_ptr)(void*), void* args, hailo_thread *thread_out)
{
    int creation_results = pthread_create(thread_out, NULL, func_ptr, args);
    if (0 != creation_results) {
        return HAILO_INTERNAL_FAILURE;
    }
    return HAILO_SUCCESS;
}

hailo_status hailo_join_thread(hailo_thread *thread)
{
    uintptr_t join_results = 0;
    int err = pthread_join(*thread, (void*)&join_results);
    if (0 != err) {
        return HAILO_INTERNAL_FAILURE;
    }
    return (hailo_status)join_results;
}

void hailo_atomic_init(hailo_atomic_int *atomic, int value)
{
    atomic_init(atomic, value);
}

int hailo_atomic_load(hailo_atomic_int *atomic)
{
    return atomic_load(atomic);
}

int hailo_atomic_fetch_add(hailo_atomic_int *atomic, int value)
{
    return atomic_fetch_add(atomic, value);
}

void hailo_atomic_increment(hailo_atomic_int *atomic)
{
    atomic_fetch_add(atomic, 1);
}

void hailo_atomic_store(hailo_atomic_int *atomic, int value)
{
    atomic_store(atomic, value);
}

#endif /* _HAILO_THREAD_H_ */
