# `hailort/libhailort/src/net_flow/pipeline` module walkthrough

## Overview
The `net_flow/pipeline` directory implements the orchestration layer for asynchronous inference pipelines, covering buffer lifetime, execution threads, and post-processing operations. The pipeline is built around `PipelineBuffer` objects managed by `BufferPool` instances, while the `PipelineElement` hierarchy and the `AsyncPipeline` builder wire stream transformations to the hardware interfaces.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L31-L168】【F:hailort/libhailort/src/net_flow/pipeline/async_infer_runner.cpp†L26-L149】

## Core buffer and pool abstractions
### PipelineBuffer
`PipelineBuffer` holds the payload as either a `MemoryView`, DMA buffer, or pixel buffer and captures the callback that returns the buffer to its originating pool when execution completes. It stores additional metadata such as start timestamp and user-supplied `AdditionalData`, and it maps DMA buffers on demand when `as_view` is invoked.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L85-L168】【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L27-L195】

### BufferPool
`BufferPool` reuses frames through an SPSC queue together with a backing vector of buffers. During construction it can register an optional statistics collector and pre-allocate buffers according to DMA requirements. At runtime it exposes management hooks for resizing, VDevice mapping, and queue introspection.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L170-L199】【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L196-L280]
