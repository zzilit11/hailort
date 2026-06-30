/**
 * Copyright (c) 2019-2025 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file driver_os_specific.hpp
 * @brief Contains some functions for hailort driver which have OS specific implementation.
 **/

#ifndef _HAILO_DRIVER_OS_SPECIFIC_HPP_
#define _HAILO_DRIVER_OS_SPECIFIC_HPP_

#include "hailo/expected.hpp"
#include "common/file_descriptor.hpp"
#include "vdma/driver/hailort_driver.hpp"

namespace hailort
{

Expected<FileDescriptor> open_device_file(const std::string &path);
Expected<HailoRTDriver::DeviceInfo> query_device_info(const std::string &device_name);
Expected<std::vector<HailoRTDriver::DeviceInfo>> scan_nnc_devices();
Expected<std::vector<HailoRTDriver::DeviceInfo>> scan_soc_devices();

hailo_status convert_errno_to_hailo_status(int err, const char* ioctl_name);

// Runs the ioctl, returns errno value (or 0 on success)
int run_hailo_ioctl(underlying_handle_t file, uint32_t ioctl_code, void *param);

} /* namespace hailort */

#endif /* _HAILO_DRIVER_OS_SPECIFIC_HPP_ */
