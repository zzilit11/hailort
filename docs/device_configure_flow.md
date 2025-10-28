# Device::configure Implementation Pointers

## `Hef::create` loading flow

### Overload entry points
- `Hef::create(const std::string &hef_path)` delegates to `Hef::Impl::create`
  and wraps the resulting implementation object in a `std::shared_ptr`. The
  helper allocates the implementation with `make_shared_nothrow` and returns a
  `Hef` facade over it.【F:hailort/libhailort/src/hef/hef.cpp†L166-L194】
- `Hef::create(const MemoryView &hef_buffer)` first copies the supplied buffer
  into a DMA-backed `Buffer::create_shared` instance before reusing the
  `std::shared_ptr<Buffer>` overload.【F:hailort/libhailort/src/hef/hef.cpp†L175-L193】

### `Hef::Impl::create` behaviour
- When a filesystem path is supplied, the implementation checks the
  `HAILO_COPY_HEF_CONTENT_TO_A_MAPPED_BUFFER_PRE_CONFIGURE_ENV_VAR`
  environment variable. If the flag is enabled, it reads the binary file into a
  DMA buffer and dispatches to the shared-buffer overload; otherwise it
  constructs `Hef::Impl` directly with the path and propagates any parsing
  status.【F:hailort/libhailort/src/hef/hef.cpp†L320-L337】
- The shared-buffer overload simply instantiates `Hef::Impl` with the provided
  buffer, logs a failure on error, and returns the initialized
  implementation.【F:hailort/libhailort/src/hef/hef.cpp†L340-L350】
- Both constructors call `GOOGLE_PROTOBUF_VERIFY_VERSION` and then invoke either
  `parse_hef_file` or `parse_hef_memview` to populate all metadata before
  reporting success.【F:hailort/libhailort/src/hef/hef.cpp†L1053-L1080】

### File-backed parsing
`parse_hef_file` creates a `SeekableBytesReader`, opens the HEF, and records the
reader inside the implementation object. It then parses the header prefix, sets
the HEF version, and applies version-specific validation:

1. V0 calculates an MD5 digest over the remainder of the stream and verifies the
   header checksum.【F:hailort/libhailort/src/hef/hef.cpp†L566-L576】
2. V1 reads extended header fields, computes CRC32 over the protobuf and CCW
   region, and records the offset to the configuration section.【F:hailort/libhailort/src/hef/hef.cpp†L578-L587】
3. V2 and V3 repeat the process using XXH3-64 checksums and account for padded
   CCW payloads before storing the hash.【F:hailort/libhailort/src/hef/hef.cpp†L588-L608】

After the header step the function parses the embedded `ProtoHEFHef`
message from the stream, transfers field ownership into the implementation, and
invokes `fill_core_ops_and_networks_metadata` to expand per-CoreOp metadata and
validate declared extensions. Finally, it closes the reader and emits a trace
event recording the SDK version and digest.【F:hailort/libhailort/src/hef/hef.cpp†L615-L628】【F:hailort/libhailort/src/hef/hef.cpp†L542-L551】

### In-memory parsing
`parse_hef_memview` mirrors the file-backed workflow for buffers. It stores the
shared buffer handle, reuses `SeekableBytesReader` to walk the header, runs the
same checksum logic for V0–V3, and calls `parse_hef_memview_internal` to decode
the protobuf payload. The internal helper feeds the resulting message through
`transfer_protobuf_field_ownership` and reuses `fill_core_ops_and_networks_metadata`
for metadata construction.【F:hailort/libhailort/src/hef/hef.cpp†L646-L742】【F:hailort/libhailort/src/hef/hef.cpp†L631-L641】

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
<<<<<<< ours
`VdmaDevice::add_hef`. This flow is responsible for parsing HEF network groups,
initializing the CoreOp controller, configuring stream descriptors, and preparing
DMA resources for host-to-device communication.
=======
>>>>>>> theirs
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
