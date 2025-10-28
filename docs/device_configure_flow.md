# Device::configure Implementation Pointers

## Entry Point
The public API `hailort::Device::configure(const Hef &hef)` is declared in
`hailort/libhailort/include/hailo/device.hpp`. The call immediately forwards to the
common implementation in `hailort/libhailort/src/device_common/device_internal.cpp`,
where `DeviceBase::configure` validates the HEF compatibility and invokes the
internal helper `add_hef` with the resolved configuration parameters.

## Common Base Implementation
The pure virtual method signature for `add_hef` lives in
`hailort/libhailort/src/device_common/device_internal.hpp`. Each concrete device
class overrides this method to construct `ConfiguredNetworkGroup` instances that
match the transport backend.

## PCIe / Integrated Devices
`hailort/libhailort/src/vdma/vdma_device.cpp` implements
`VdmaDevice::add_hef`. This flow is responsible for parsing HEF network groups,
initializing the CoreOp controller, configuring stream descriptors, and preparing
DMA resources for host-to-device communication.

## Ethernet Devices
`hailort/libhailort/src/eth/eth_device.cpp` provides the Ethernet-specific
implementation `EthernetDevice::add_hef`. It builds the network groups by using
Ethernet control plane commands, programs the firmware via RPC, and prepares the
socket-based streaming endpoints before returning the configured handles.
