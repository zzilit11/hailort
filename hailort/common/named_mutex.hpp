/**
 * Copyright (c) 2019-2026 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file named_mutex.hpp
 * @brief Named mutex guard
 **/

#ifndef _HAILO_NAMED_MUTEX_HPP_
#define _HAILO_NAMED_MUTEX_HPP_

#include "common/utils.hpp"
#include "hailo/hailort.h"
#include "hailo/expected.hpp"
#include "common/logger_macros.hpp"

#include "common/filesystem.hpp"

#include <memory>

namespace hailort
{

class NamedMutex final
{
public:
    static Expected<std::shared_ptr<NamedMutex>> create(const std::string &path);

    NamedMutex(const std::string &path, LockedFile &&locked_file) : m_path(path), m_locked_file(std::move(locked_file)) {}
    ~NamedMutex();

    const std::string &path() const { return m_path; }
    hailo_status lock(std::chrono::milliseconds timeout);
    hailo_status unlock();

private:
    const std::string m_path;

    LockedFile m_locked_file;

public:
    class LockGuard final
    {
    public:
        static Expected<LockGuard> create(std::shared_ptr<NamedMutex> mutex, std::chrono::milliseconds timeout)
        {
            auto status = mutex->lock(timeout);
            CHECK_SUCCESS_WITH_ACCEPTABLE_STATUS(HAILO_TIMEOUT, status);
            return LockGuard(mutex);
        }

        LockGuard(std::shared_ptr<NamedMutex> mutex) : m_mutex(mutex) {}
        ~LockGuard()
        {
            if (nullptr == m_mutex) {
                return;
            }
            auto status = m_mutex->unlock();
            if (HAILO_SUCCESS != status) {
                LOGGER__ERROR("Failed to unlock named mutex '{}': {}", m_mutex->path(), status);
            }
        }

        LockGuard &operator=(const LockGuard &other) = delete;
        LockGuard(const LockGuard &other) = delete;
        LockGuard &operator=(LockGuard &&other)
        {
            if (this != &other) {
                m_mutex = std::exchange(other.m_mutex, nullptr);
            }
            return *this;
        }
        LockGuard(LockGuard &&other) : m_mutex(std::exchange(other.m_mutex, nullptr)) {}

    private:
        std::shared_ptr<NamedMutex> m_mutex;
    };
};

} /* namespace hailort */

#endif /* _HAILO_NAMED_MUTEX_HPP_ */
