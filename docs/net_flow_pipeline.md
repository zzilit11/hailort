# `hailort/libhailort/src/net_flow/pipeline` module walkthrough

## Overview
The `net_flow/pipeline` directory implements the orchestration layer for asynchronous inference pipelines, covering buffer lifetime, execution threads, and post-processing operations. The pipeline is built around `PipelineBuffer` objects managed by `BufferPool` instances, while the `PipelineElement` hierarchy and the `AsyncPipeline` builder wire stream transformations to the hardware interfaces.
【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L31-L168】
【F:hailort/libhailort/src/net_flow/pipeline/async_infer_runner.cpp†L26-L149】

## Core buffer and pool abstractions
### PipelineBuffer
`PipelineBuffer` holds the payload as either a `MemoryView`, DMA buffer, or pixel buffer and captures the callback that returns the buffer to its originating pool when execution completes. It stores additional metadata such as start timestamp and user-supplied `AdditionalData`, and it maps DMA buffers on demand when `as_view` is invoked.
【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L85-L168】
【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L27-L195】

### BufferPool
`BufferPool` reuses frames through an SPSC queue together with a backing vector of buffers. During construction it can register an optional statistics collector and pre-allocate buffers according to DMA requirements. At runtime it exposes management hooks for resizing, VDevice mapping, and queue introspection.
【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L170-L199】
【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L196-L280】

## Pipeline element hierarchy
### PipelineElementInternal and IntermediateElement
`PipelineElementInternal` owns the shared pipeline state and duration metrics, signalling `AsyncPipeline` to shut down when fatal errors occur. `IntermediateElement` provides the default single-input/single-output forwarding node to simplify pad connections between elements.
【F:hailort/libhailort/src/net_flow/pipeline/pipeline_internal.hpp†L33-L58】
【F:hailort/libhailort/src/net_flow/pipeline/pipeline_internal.cpp†L15-L47】

### Queue-based elements
`BaseQueueElement` drives an SPSC queue on a background thread and handles activation, stop, and abort flows. `PushQueueElement` and `AsyncPushQueueElement` push buffers downstream, with the async variant customizing user buffer flushing and shutdown behavior. `PullQueueElement` and `UserBufferQueueElement` manage consumer-side buffers, and `MultiPushQueue` aligns multiple outputs towards the hardware elements.
【F:hailort/libhailort/src/net_flow/pipeline/queue_elements.hpp†L18-L200】

## Workload queueing data structure and preemption hook
### SpscQueue-based frame queueing
All buffer pools and queue elements use `SpscQueue<PipelineBuffer>` for the single-producer/single-consumer buffer flow. When a buffer pool is created it seeds the queue with the frame count so released `PipelineBuffer` objects can be reused.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L322-L375】 
Each queue element receives the same `SpscQueue` and couples it with activation and abort events.
【F:hailort/libhailort/src/net_flow/pipeline/queue_elements.cpp†L19-L205】 
`SpscQueue` itself wraps `moodycamel::ReaderWriterQueue` and supplements it with semaphores, blocking push/pop, timeout handling, and shutdown detection so the pipeline can exit safely.
【F:hailort/common/thread_safe_queue.hpp†L103-L207】

### Workload interception layer design
When `AsyncPipelineBuilder::add_push_queue_element` inserts an `AsyncPushQueueElement`, it rewires the upstream pad to the new queue element before connecting to the hardware element.
【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L191-L205】 
A preemption layer can therefore derive from `BaseQueueElement` and override `run_push_async` or `run_in_thread` to enqueue intercepted work into a policy queue, reorder it, and forward it when allowed. The existing `SpscQueue` continues to manage frame ownership, while the custom element focuses on scheduling and preemption rules.
The `net_flow/pipeline` directory implements the orchestration layer for asynchronous inference pipelines, covering buffer lifetime, execution threads, and post-processing operations. The pipeline is built around `PipelineBuffer` objects managed by `BufferPool` instances, while the `PipelineElement` hierarchy and the `AsyncPipeline` builder wire stream transformations to the hardware interfaces.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L31-L168】【F:hailort/libhailort/src/net_flow/pipeline/async_infer_runner.cpp†L26-L149】

## Core buffer and pool abstractions
### PipelineBuffer
`PipelineBuffer` holds the payload as either a `MemoryView`, DMA buffer, or pixel buffer and captures the callback that returns the buffer to its originating pool when execution completes. It stores additional metadata such as start timestamp and user-supplied `AdditionalData`, and it maps DMA buffers on demand when `as_view` is invoked.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L85-L168】【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L27-L195】

### BufferPool
`BufferPool` reuses frames through an SPSC queue together with a backing vector of buffers. During construction it can register an optional statistics collector and pre-allocate buffers according to DMA requirements. At runtime it exposes management hooks for resizing, VDevice mapping, and queue introspection.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L170-L199】【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L196-L280】

## Pipeline element hierarchy
### PipelineElementInternal and IntermediateElement
`PipelineElementInternal` owns the shared pipeline state and duration metrics, signalling `AsyncPipeline` to shut down when fatal errors occur. `IntermediateElement` provides the default single-input/single-output forwarding node to simplify pad connections between elements.【F:hailort/libhailort/src/net_flow/pipeline/pipeline_internal.hpp†L33-L58】【F:hailort/libhailort/src/net_flow/pipeline/pipeline_internal.cpp†L15-L47】

### Queue-based elements
`BaseQueueElement` drives an SPSC queue on a background thread and handles activation, stop, and abort flows. `PushQueueElement` and `AsyncPushQueueElement` push buffers downstream, with the async variant customizing user buffer flushing and shutdown behavior. `PullQueueElement` and `UserBufferQueueElement` manage consumer-side buffers, and `MultiPushQueue` aligns multiple outputs towards the hardware elements.【F:hailort/libhailort/src/net_flow/pipeline/queue_elements.hpp†L18-L200】

## Workload queueing data structure and preemption hook
### SpscQueue-based frame queueing
All buffer pools and queue elements use `SpscQueue<PipelineBuffer>` for the single-producer/single-consumer buffer flow. When a buffer pool is created it seeds the queue with the frame count so released `PipelineBuffer` objects can be reused.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L322-L375】 Each queue element receives the same `SpscQueue` and couples it with activation and abort events.【F:hailort/libhailort/src/net_flow/pipeline/queue_elements.cpp†L19-L205】 `SpscQueue` itself wraps `moodycamel::ReaderWriterQueue` and supplements it with semaphores, blocking push/pop, timeout handling, and shutdown detection so the pipeline can exit safely.【F:hailort/common/thread_safe_queue.hpp†L103-L207】

### Workload interception layer design
When `AsyncPipelineBuilder::add_push_queue_element` inserts an `AsyncPushQueueElement`, it rewires the upstream pad to the new queue element before connecting to the hardware element.【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L191-L205】 A preemption layer can therefore derive from `BaseQueueElement` and override `run_push_async` or `run_in_thread` to enqueue intercepted work into a policy queue, reorder it, and forward it when allowed. The existing `SpscQueue` continues to manage frame ownership, while the custom element focuses on scheduling and preemption rules.

### Multi-input/output and post-processing elements
`BaseMuxElement` and `BaseDemuxElement` implement shared logic for joining or splitting multiple streams. NMS-specific classes (`NmsPostProcessMuxElement`, `NmsMuxElement`) execute configurable post-processing on detection tensors, and `TransformDemuxElement` converts hardware frames to host formats before distributing them to downstream pipeline pads.【F:hailort/libhailort/src/net_flow/pipeline/multi_io_elements.hpp†L18-L200】

## Asynchronous pipeline assembly
### Role of AsyncPipelineBuilder
<<<<<<< ours
`AsyncPipelineBuilder` resolves AUTO values in the user-facing formats, prepares per-input preprocessing and queue elements, and connects them to the hardware elements. For outputs it inspects metadata such as NMS, IOU, or Softmax requirements, attaches the matching post-processing elements, and adds demultiplexers or transforms as needed.
【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L26-L200】
【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L200-L360】

### AsyncPipeline and AsyncInferRunner
`AsyncPipeline` tracks the assembled elements, entry/exit nodes, and build parameters, terminating the full pipeline and cleaning user buffers when errors occur. `AsyncInferRunnerImpl` invokes `AsyncPipelineBuilder` to create and activate the pipeline, binds the input/output buffers during inference, and disables the elements on shutdown.
【F:hailort/libhailort/src/net_flow/pipeline/async_infer_runner.cpp†L26-L200】

## Data flow summary
1. `AsyncPipelineBuilder` expands input formats, allocates preprocessing queues, and wires them into the hardware elements.【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L72-L164】
2. Hardware outputs traverse the configured NMS/demux elements, perform format transforms, and reach the final queue elements.
【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L191-L360】
3. `AsyncInferRunnerImpl` runs and stops the pipeline while `AsyncPipeline` performs the coordinated shutdown and buffer reclamation on errors.
【F:hailort/libhailort/src/net_flow/pipeline/async_infer_runner.cpp†L62-L200】
=======
`AsyncPipelineBuilder` resolves AUTO values in the user-facing formats, prepares per-input preprocessing and queue elements, and connects them to the hardware elements. For outputs it inspects metadata such as NMS, IOU, or Softmax requirements, attaches the matching post-processing elements, and adds demultiplexers or transforms as needed.【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L26-L200】【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L200-L360】

### AsyncPipeline and AsyncInferRunner
`AsyncPipeline` tracks the assembled elements, entry/exit nodes, and build parameters, terminating the full pipeline and cleaning user buffers when errors occur. `AsyncInferRunnerImpl` invokes `AsyncPipelineBuilder` to create and activate the pipeline, binds the input/output buffers during inference, and disables the elements on shutdown.【F:hailort/libhailort/src/net_flow/pipeline/async_infer_runner.cpp†L26-L200】

## Data flow summary
1. `AsyncPipelineBuilder` expands input formats, allocates preprocessing queues, and wires them into the hardware elements.【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L72-L164】
2. Hardware outputs traverse the configured NMS/demux elements, perform format transforms, and reach the final queue elements.【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L191-L360】
3. `AsyncInferRunnerImpl` runs and stops the pipeline while `AsyncPipeline` performs the coordinated shutdown and buffer reclamation on errors.【F:hailort/libhailort/src/net_flow/pipeline/async_infer_runner.cpp†L62-L200】
>>>>>>> theirs
