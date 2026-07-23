/**
 * Copyright (c) 2019-2026 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file platform.h
 * @brief Platform dependent definitions for libhailopp
 **/

#ifndef _HAILOPP_PLATFORM_H_
#define _HAILOPP_PLATFORM_H_

#ifndef __linux__
#error "HailoPP supports Linux only"
#endif

#define HAILOPPAPI __attribute__ ((visibility ("default")))

#endif /* _HAILOPP_PLATFORM_H_ */
