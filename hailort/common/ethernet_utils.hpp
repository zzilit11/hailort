/**
 * Copyright (c) 2019-2025 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file ethernet_utils.hpp
 * @brief TODO
 *
 * TODO
 **/

#ifndef __OS_ETHERNET_UTILS_H__
#define __OS_ETHERNET_UTILS_H__

#include <string>
#include <hailo/hailort.h>
#include "hailo/expected.hpp"

#include <net/if.h>

namespace hailort
{

class EthernetUtils final
{
public:
    EthernetUtils() = delete;

    static const uint32_t MAX_INTERFACE_SIZE = IFNAMSIZ;

    static Expected<std::string> get_interface_from_board_ip(const std::string &board_ip);
    static Expected<std::string> get_ip_from_interface(const std::string &interface_name);

private:
    static Expected<std::string> get_interface_from_arp_entry(char *arp_entry);
};

} /* namespace hailort */

#endif /* __OS_ETHERNET_UTILS_H__ */
