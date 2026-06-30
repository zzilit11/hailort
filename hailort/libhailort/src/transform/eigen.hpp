/**
 * Copyright (c) 2019-2025 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file eigen.hpp
 * @brief Includes the Eigen library with the required compiler instructions
 **/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#if defined(__GNUC__) && (__GNUC__ >= 11)
    #pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif // GCC version
#include <Eigen/Dense>
#pragma GCC diagnostic pop
