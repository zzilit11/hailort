# CoreOp, Context, and Network Group Overview

## CoreOp
- **Definition**: A CoreOp is the runtime abstraction that represents a configured network group on a device. It exposes lifecycle operations, stream accessors, cache management hooks, and scheduler controls for the group it owns.【F:hailort/libhailort/src/core_op/core_op.hpp†L47-L133】
- **Primary responsibilities**:
  - Activate/deactivate network groups and ensure all asynchronous transfers complete during shutdown.【F:hailort/libhailort/src/core_op/core_op.hpp†L80-L121】
  - Provide access to configured input/output streams, latency meters, and infer queue state for the group’s pipelines.【F:hailort/libhailort/src/core_op/core_op.hpp†L70-L123】
  - Manage cache buffers and boundary VDMA channels for dataflow orchestration.【F:hailort/libhailort/src/core_op/core_op.hpp†L123-L192】
- **Key components**:
  - `m_config_params` stores the applied `ConfigureNetworkParams` for recreating streams or querying batching data.【F:hailort/libhailort/src/core_op/core_op.hpp†L178-L183】
  - `m_metadata` points to `CoreOpMetadata`, which enumerates contexts, layers, and supported features required at runtime.【F:hailort/libhailort/src/core_op/core_op.hpp†L56-L58】【F:hailort/libhailort/src/hef/core_op_metadata.hpp†L113-L166】
  - `ActiveCoreOpHolder`, activation/deactivation accumulators, and stream maps track scheduling ownership and runtime statistics.【F:hailort/libhailort/src/core_op/core_op.hpp†L91-L135】【F:hailort/libhailort/src/core_op/core_op.hpp†L178-L192】
- **Concrete variants**: `VdmaConfigCoreOp`, `VDeviceCoreOp`, and `HcpConfigCoreOp` specialize CoreOp for single-device vDMA, virtual device aggregation, or lightweight host-controlled deployments respectively.【F:hailort/libhailort/src/core_op/core_op.hpp†L6-L16】

## Context
- **Definition**: A Context is a configuration stage inside a CoreOp. Its metadata captures the context-switch action list, configuration buffers, and per-layer transfer descriptors that must be executed together.【F:hailort/libhailort/src/hef/core_op_metadata.hpp†L63-L107】
- **Runtime form**: When materialized, a Context becomes `ContextResources`, which allocates config buffers, maps edge layers to VDMA channels, and records DDR channel usage for the device driver.【F:hailort/libhailort/src/core_op/resource_manager/resource_manager.hpp†L85-L137】
- **Role in execution**:
  - Contexts drive the device’s context-switch state machine, arranging how resources are reserved and released as the network group advances through its stages.【F:hailort/libhailort/src/core_op/resource_manager/resource_manager.hpp†L111-L133】
  - Each context controls its subset of boundary, inter-context, DDR, and cache layers, ensuring transfers are issued with the correct buffers and directions.【F:hailort/libhailort/src/hef/core_op_metadata.hpp†L75-L107】【F:hailort/libhailort/src/core_op/resource_manager/resource_manager.hpp†L108-L133】
- **Context types**: The firmware protocol defines four context types—Preliminary, Dynamic, Batch Switching, and Activation—which are addressed in a fixed order by the context-switch controller.【F:common/include/control_protocol.h†L1480-L1497】

## Network Group
- **Definition**: A Network Group is the user-visible container that bundles one or more CoreOps under a shared name, exposing activation, stream discovery, and v-stream creation APIs.【F:hailort/libhailort/src/network_group/network_group_internal.hpp†L47-L205】
- **Responsibilities**:
  - Activate/deactivate all contained CoreOps while propagating scheduler and batching parameters to each instance.【F:hailort/libhailort/src/network_group/network_group_internal.hpp†L59-L176】
  - Surface stream, v-stream, and latency metadata aggregated from the underlying CoreOps and their metadata.【F:hailort/libhailort/src/network_group/network_group_internal.hpp†L71-L205】
- **Metadata linkage**: `NetworkGroupMetadata` associates the group name with the architecture-specific CoreOp metadata map, sorted outputs, supported feature flags, and optional post-processing metadata.【F:hailort/libhailort/src/hef/core_op_metadata.hpp†L198-L219】
- **Composition**: A configured network group typically wraps a vector of CoreOps, allowing the runtime to coordinate multiple device partitions or physical devices under one activation handle.【F:hailort/libhailort/src/network_group/network_group_internal.hpp†L15-L205】
<<<<<<< ours
=======

## Relationship between HEF, Network Groups, and CoreOps
- **HEF as the source of truth**: `Hef::Impl` exposes the network groups and their CoreOp definitions parsed from the HEF file, letting the runtime ask for the groups by name and retrieve their metadata bundles.【F:hailort/libhailort/src/hef/hef_internal.hpp†L283-L288】
- **Network-group materialization**: Device backends iterate the HEF-defined network groups, choose or synthesize `ConfigureNetworkParams`, and then construct the runtime `ConfiguredNetworkGroup` objects. During this loop they also enforce batch-size rules and collect CoreOp metadata for the device’s partial-cluster layout.【F:hailort/libhailort/src/vdma/vdma_device.cpp†L341-L432】
- **CoreOp instantiation**: For each HEF network group, the backend builds CoreOps (usually a single `VdmaConfigCoreOp`) from the retrieved metadata, wires them to resource managers, and finally packages them into a `ConfiguredNetworkGroupBase` handle that the user manipulates.【F:hailort/libhailort/src/vdma/vdma_device.cpp†L380-L429】【F:hailort/libhailort/src/network_group/network_group.cpp†L193-L200】
- **Runtime relationship**: The resulting configured network group owns the CoreOp instances created from the HEF data, so activating the group implicitly activates its CoreOps, while deactivation tears them down and releases the resources originally described by the HEF.【F:hailort/libhailort/src/network_group/network_group_internal.hpp†L59-L176】【F:hailort/libhailort/src/network_group/network_group.cpp†L203-L220】
>>>>>>> theirs
