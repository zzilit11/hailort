/**
 * Copyright (c) 2019-2026 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file eigen_wrapper.hpp
 * @brief Eigen library wrapper with warning suppressions
 **/

#ifndef _HAILOTRACKER_SRC_EIGEN_WRAPPER_HPP_
#define _HAILOTRACKER_SRC_EIGEN_WRAPPER_HPP_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#if defined(__GNUC__) && (__GNUC__ >= 11)
    #pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

#include <Eigen/Dense>

#pragma GCC diagnostic pop

#endif // _HAILOTRACKER_SRC_EIGEN_WRAPPER_HPP_
