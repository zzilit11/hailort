/**
 * Copyright (c) 2019-2025 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file dma_able_buffer.cpp
 * @brief A Buffer that can be mapped to some device for dma operations.
 *        See hpp for more information.
 **/

#include "hailo/hailort_common.hpp"
#include "dma_able_buffer.hpp"
#include "common/os_utils.hpp"
#include "common/mmap_buffer.hpp"

#include <sys/mman.h>

namespace hailort {
namespace vdma {

// User buffer. This class does not own the buffer.
class UserAllocatedDmaAbleBuffer : public DmaAbleBuffer {
public:
    static Expected<DmaAbleBufferPtr> create(void *user_address, size_t size)
    {
        CHECK_ARG_NOT_NULL_AS_EXPECTED(user_address);
        CHECK_AS_EXPECTED(0 != size, HAILO_INVALID_ARGUMENT);

        const auto dma_able_alignment = OsUtils::get_dma_able_alignment();

        CHECK_AS_EXPECTED(0 == (reinterpret_cast<size_t>(user_address) % dma_able_alignment),
            HAILO_INVALID_ARGUMENT, "User address mapped as dma must be aligned (alignment value {})", dma_able_alignment);

        auto buffer = make_shared_nothrow<UserAllocatedDmaAbleBuffer>(user_address, size);
        CHECK_NOT_NULL_AS_EXPECTED(buffer, HAILO_OUT_OF_HOST_MEMORY);

        return std::static_pointer_cast<DmaAbleBuffer>(buffer);
    }

    UserAllocatedDmaAbleBuffer(void *user_address, size_t size) :
        m_size(size),
        m_user_address(user_address)
    {}

    virtual size_t size() const override { return m_size; }
    virtual void *user_address() override { return m_user_address; }
    virtual vdma_mapped_buffer_driver_identifier buffer_identifier() override { return HailoRTDriver::INVALID_MAPPED_BUFFER_DRIVER_IDENTIFIER; }

private:
    const size_t m_size;
    void *m_user_address;
};

class PageAlignedDmaAbleBuffer : public DmaAbleBuffer {
public:
    static Expected<DmaAbleBufferPtr> create(size_t size)
    {
        // Shared memory to allow python fork.
        auto mmapped_buffer = MmapBuffer<void>::create_shared_memory(size);
        CHECK_EXPECTED(mmapped_buffer);

        auto buffer =  make_shared_nothrow<PageAlignedDmaAbleBuffer>(mmapped_buffer.release());
        CHECK_NOT_NULL_AS_EXPECTED(buffer, HAILO_OUT_OF_HOST_MEMORY);
        return std::static_pointer_cast<DmaAbleBuffer>(buffer);
    }

    PageAlignedDmaAbleBuffer(MmapBuffer<void> &&mmapped_buffer) :
        m_mmapped_buffer(std::move(mmapped_buffer))
    {}

    virtual void* user_address() override { return m_mmapped_buffer.address(); }
    virtual size_t size() const override { return m_mmapped_buffer.size(); }
    virtual vdma_mapped_buffer_driver_identifier buffer_identifier() override { return HailoRTDriver::INVALID_MAPPED_BUFFER_DRIVER_IDENTIFIER; }

private:
    // Using mmap instead of aligned_alloc to enable MEM_SHARE flag - used for multi-process fork.
    MmapBuffer<void> m_mmapped_buffer;
};

// Allocate low memory buffer using HailoRTDriver.
class DriverAllocatedDmaAbleBuffer : public DmaAbleBuffer {
public:
    static Expected<DmaAbleBufferPtr> create(HailoRTDriver &driver, size_t size)
    {
        auto driver_buffer_handle = driver.vdma_low_memory_buffer_alloc(size);
        CHECK_EXPECTED(driver_buffer_handle);

        auto mmapped_buffer = MmapBuffer<void>::create_file_map(size, driver.fd(), driver_buffer_handle.value());
        if (!mmapped_buffer) {
            auto free_status = driver.vdma_low_memory_buffer_free(driver_buffer_handle.value());
            if (HAILO_SUCCESS != free_status) {
                LOGGER__ERROR("Failed free vdma low memory with status {}", free_status);
                // Continue
            }

            return make_unexpected(mmapped_buffer.status());
        }
        CHECK_EXPECTED(mmapped_buffer);

        auto buffer = make_shared_nothrow<DriverAllocatedDmaAbleBuffer>(driver, driver_buffer_handle.value(),
            mmapped_buffer.release());
        CHECK_NOT_NULL_AS_EXPECTED(buffer, HAILO_OUT_OF_HOST_MEMORY);
        return std::static_pointer_cast<DmaAbleBuffer>(buffer);
    }

    DriverAllocatedDmaAbleBuffer(HailoRTDriver &driver, vdma_mapped_buffer_driver_identifier driver_allocated_buffer_id,
        MmapBuffer<void> &&mmapped_buffer) :
        m_driver(driver),
        m_driver_allocated_buffer_id(driver_allocated_buffer_id),
        m_mmapped_buffer(std::move(mmapped_buffer))
    {}

    DriverAllocatedDmaAbleBuffer(const DriverAllocatedDmaAbleBuffer &) = delete;
    DriverAllocatedDmaAbleBuffer &operator=(const DriverAllocatedDmaAbleBuffer &) = delete;

    ~DriverAllocatedDmaAbleBuffer()
    {
        auto status = m_mmapped_buffer.unmap();
        if (HAILO_SUCCESS != status) {
            LOGGER__ERROR("Failed to unmap buffer");
            // continue
        }

        status = m_driver.vdma_low_memory_buffer_free(m_driver_allocated_buffer_id);
        if (HAILO_SUCCESS != status) {
            LOGGER__ERROR("Failed to free low memory buffer");
            // continue
        }
    }

    virtual void* user_address() override { return m_mmapped_buffer.address(); }
    virtual size_t size() const override { return m_mmapped_buffer.size(); }
    virtual vdma_mapped_buffer_driver_identifier buffer_identifier() override { return m_driver_allocated_buffer_id; }

private:
    HailoRTDriver &m_driver;
    const vdma_mapped_buffer_driver_identifier m_driver_allocated_buffer_id;

    MmapBuffer<void> m_mmapped_buffer;
};

Expected<DmaAbleBufferPtr> DmaAbleBuffer::create_from_user_address(void *user_address, size_t size)
{
    return UserAllocatedDmaAbleBuffer::create(user_address, size);
}

Expected<DmaAbleBufferPtr> DmaAbleBuffer::create_by_allocation(size_t size)
{
    return PageAlignedDmaAbleBuffer::create(size);
}

Expected<DmaAbleBufferPtr> DmaAbleBuffer::create_by_allocation(size_t size, HailoRTDriver &driver)
{
    if (driver.allocate_driver_buffer()) {
        return DriverAllocatedDmaAbleBuffer::create(driver, size);
    } else {
        // The driver is not needed.
        return create_by_allocation(size);
    }
}

} /* namespace vdma */
} /* namespace hailort */
