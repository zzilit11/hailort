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
`VdmaDevice::add_hef`. The function performs several backend-specific steps
before the resulting `ConfiguredNetworkGroup` handles are returned:

1. **Driver/session preparation** – The call starts by marking the device as
   used via `mark_as_used` and, if this is the first configuration on the
   process, clears any previously configured applications
   (`clear_configured_apps`). It then instantiates the shared `CacheManager`,
   the vDMA `InterruptsDispatcher`, and the `TransferLauncher` objects that will
   serve all configured CoreOps.
2. **Network-group assembly** – `create_networks_group_vector` iterates all HEF
   network groups. It merges user provided `ConfigureNetworkParams` (or builds
   defaults through `hef.create_configure_params`), validates the batch-size
   relationship, and calls `create_core_ops_metadata` to fetch the per-CoreOp
   metadata that matches the device architecture and partial cluster layout.
3. **CoreOp materialization** – For every network group,
   `create_configured_network_group` constructs the runtime objects. This
   routine creates caches from the selected CoreOp metadata, builds a
   `ResourcesManager` (responsible for descriptor/buffer allocations and engine
   reservations), instantiates a `VdmaConfigCoreOp`, and asks it to
   `create_streams_from_config_params`. It then validates that all boundary
   streams declared in the HEF were created before wrapping the CoreOp inside a
   `ConfiguredNetworkGroupBase` instance.

Through this sequence the PCIe/integrated backend binds the HEF definition to
the hardware engines, caches, and DMA infrastructure that the host runtime uses
for submission.

## Ethernet Devices
`hailort/libhailort/src/eth/eth_device.cpp` provides the Ethernet-specific
implementation `EthernetDevice::add_hef`. It builds the network groups by using
Ethernet control plane commands, programs the firmware via RPC, and prepares the
socket-based streaming endpoints before returning the configured handles.
