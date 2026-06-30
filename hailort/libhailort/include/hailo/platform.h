/**
 * Copyright (c) 2019-2025 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file platform.h
 * @brief Platform dependent includes and definitions
 **/

#ifndef _HAILO_PLATFORM_H_
#define _HAILO_PLATFORM_H_

/** Exported symbols define */

#define HAILORTAPI __attribute__ ((visibility ("default")))


/** Includes */

#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>


/** Typedefs */

// underlying_handle_t
#ifndef underlying_handle_t
typedef int underlying_handle_t;
#endif

// port_t
#ifndef port_t
typedef in_port_t port_t;
#endif

// socket_t
#ifndef socket_t
typedef int socket_t;
#endif

// timeval_t
#ifndef timeval_t
typedef struct timeval timeval_t;
#endif


/** Defines and Macros */

// TODO: Fix this hack
#ifndef MSG_CONFIRM
#define MSG_CONFIRM (0)
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (socket_t)(-1)
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (int)(-1)
#endif

#define DEPRECATED(msg) __attribute((deprecated(msg)))

#define EMPTY_STRUCT_PLACEHOLDER uint8_t reserved;

#ifndef MILLISECONDS_IN_SECOND
#define MILLISECONDS_IN_SECOND (1000)
#endif
#ifndef MICROSECONDS_IN_MILLISECOND
#define MICROSECONDS_IN_MILLISECOND (1000)
#endif

#endif /* _HAILO_PLATFORM_H_ */
