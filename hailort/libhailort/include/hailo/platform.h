/**
 * Copyright (c) 2019-2026 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file platform.h
 * @brief Platform dependent includes and definitions
 **/

#ifndef _HAILO_PLATFORM_H_
#define _HAILO_PLATFORM_H_

#ifndef __linux__
#error "HailoRT supports Linux only"
#endif


/** Exported symbols define */

#define HAILORTAPI __attribute__ ((visibility ("default")))

/** Includes and Typedefs */
// underlying_handle_t
#ifndef underlying_handle_t
#include <unistd.h>
typedef int underlying_handle_t;
#endif


/** Defines and Macros */

#define DEPRECATED(msg) __attribute((deprecated(msg)))

#define EMPTY_STRUCT_PLACEHOLDER uint8_t reserved;

#ifndef MILLISECONDS_IN_SECOND
#define MILLISECONDS_IN_SECOND (1000)
#endif
#ifndef MICROSECONDS_IN_MILLISECOND
#define MICROSECONDS_IN_MILLISECOND (1000)
#endif

#endif /* _HAILO_PLATFORM_H_ */
