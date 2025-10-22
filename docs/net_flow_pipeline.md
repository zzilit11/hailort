# `hailort/libhailort/src/net_flow/pipeline` 모듈 구조 분석

## 개요
`net_flow/pipeline` 디렉터리는 비동기 추론 파이프라인의 버퍼 수명, 실행 스레드, 후처리 연산을 조율하는 상위 계층을 제공한다. 파이프라인은 `PipelineBuffer`와 `BufferPool`이 관리하는 데이터 소유권 위에, `PipelineElement` 계열 클래스와 `AsyncPipeline` 조립기가 스트림 변환과 하드웨어 상호작용을 연결하는 구조로 구축된다.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L31-L168】【F:hailort/libhailort/src/net_flow/pipeline/async_infer_runner.cpp†L26-L149】

## 핵심 버퍼 및 풀 추상화
### PipelineBuffer
`PipelineBuffer`는 데이터 페이로드를 `MemoryView`, DMA 버퍼, 픽스 버퍼 중 하나로 보관하며, 실행 완료 시 버퍼 풀에 반환하는 콜백을 캡처한다. 추가 메타데이터로 시작 시간과 사용자 정의 `AdditionalData`를 저장하고, DMA 버퍼는 `as_view` 호출 시 동적으로 메모리 매핑을 수행한다.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L85-L168】【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L27-L195】

### BufferPool
`BufferPool`은 SPSC 큐와 백업 버퍼 벡터를 사용해 프레임을 재사용한다. 생성 시 통계 수집기를 옵션으로 등록하고, DMA 가능 여부에 따라 버퍼를 예약한다. 런타임에는 버퍼 크기 변경, VDevice 매핑, 큐 크기 측정 등 관리 기능을 노출한다.【F:hailort/libhailort/src/net_flow/pipeline/pipeline.hpp†L170-L199】【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L196-L280】

## 파이프라인 요소 계층
### PipelineElementInternal과 IntermediateElement
`PipelineElementInternal`은 공통 지속 시간 수집과 파이프라인 상태를 보유하며, 비복구 오류 발생 시 전체 파이프라인을 종료하도록 `AsyncPipeline`에 신호한다. `IntermediateElement`는 단일 입력·출력을 가진 기본 전달 노드로 파이프라인 패드 간 연결을 단순화한다.【F:hailort/libhailort/src/net_flow/pipeline/pipeline_internal.hpp†L33-L58】【F:hailort/libhailort/src/net_flow/pipeline/pipeline_internal.cpp†L15-L47】

### 큐 기반 요소
`BaseQueueElement`는 백그라운드 스레드에서 SPSC 큐를 구동하며, 활성화·중지·중단 시나리오를 처리한다. 이를 확장한 `PushQueueElement`와 `AsyncPushQueueElement`는 입력 데이터를 다운스트림으로 밀어 넣고, 비동기 실행 시 사용자 버퍼 청소와 종료 흐름을 맞춤 구현한다. `PullQueueElement`와 `UserBufferQueueElement`는 소비자 측 버퍼를 관리하며, `MultiPushQueue`는 복수 출력을 HW 요소에 정렬한다.【F:hailort/libhailort/src/net_flow/pipeline/queue_elements.hpp†L18-L200】

## 작업 큐잉 자료구조와 프리엠션 확장 포인트
### SpscQueue 기반 프레임 큐잉
파이프라인의 모든 버퍼 풀과 큐 요소는 단일 생산자·단일 소비자용 `SpscQueue<PipelineBuffer>`를 사용한다. 버퍼 풀 생성 시 프레임 수만큼 큐를 초기화하여 반납된 `PipelineBuffer`를 재사용하도록 하고,【F:hailort/libhailort/src/net_flow/pipeline/pipeline.cpp†L322-L375】 큐 요소는 동일한 `SpscQueue`를 받아 활성화·중단 이벤트와 함께 관리한다.【F:hailort/libhailort/src/net_flow/pipeline/queue_elements.cpp†L19-L205】 `SpscQueue` 자체는 `moodycamel::ReaderWriterQueue` 위에서 세마포어를 이용해 블로킹 생산·소비를 구현하며, 타임아웃과 셧다운 이벤트 감지를 제공해 파이프라인 정지 시 안전하게 빠져나올 수 있도록 한다.【F:hailort/common/thread_safe_queue.hpp†L103-L207】

### 워크로드 인터셉트 레이어 설계
`AsyncPushQueueElement`를 추가할 때 `AsyncPipelineBuilder::add_push_queue_element`가 큐 요소를 파이프라인에 삽입하고 이전 요소의 패드를 새 큐에 연결한다.【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L191-L205】 따라서 프리엠션을 위한 인터셉트 레이어는 `BaseQueueElement`를 상속한 사용자 정의 요소를 작성하고, 해당 요소의 `run_push_async` 또는 `run_in_thread`에서 작업 요청을 별도 정책 큐에 저장·재정렬하도록 구현하면 된다. 새 요소를 파이프라인에 삽입하면 기존 `SpscQueue`가 프레임 버퍼의 수명 관리를 맡고, 상위 레이어는 요청 스케줄링이나 선점 취소 조건만 집중해 구현할 수 있다.

### 다중 입출력 및 후처리 요소
`BaseMuxElement`와 `BaseDemuxElement`는 여러 스트림을 병합·분기하는 공통 로직을 제공한다. NMS 관련 클래스(`NmsPostProcessMuxElement`, `NmsMuxElement`)는 임계값을 조정 가능한 후처리를 수행하고, `TransformDemuxElement`는 하드웨어 프레임을 호스트 포맷으로 변환하여 후속 파이프라인에 분배한다.【F:hailort/libhailort/src/net_flow/pipeline/multi_io_elements.hpp†L18-L200】

## 비동기 파이프라인 조립
### AsyncPipelineBuilder의 역할
`AsyncPipelineBuilder`는 사용자 형식에 포함된 AUTO 설정을 실제 스트림 형식으로 확장하고, 각 입력 스트림에 대해 전처리/큐 요소를 배치한다. 후처리 단계에서는 NMS, IOU, Softmax 등 연산 메타데이터를 해석해 대응 요소를 추가하고, 필요 시 디멀티플렉서와 변환을 구성한다.【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L26-L200】【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L200-L360】

### AsyncPipeline과 AsyncInferRunner
`AsyncPipeline`은 조립된 요소, 진입·종료 노드, 빌드 파라미터를 유지하며, 오류 발생 시 전체 파이프라인을 종료하고 사용자 버퍼를 정리한다. `AsyncInferRunnerImpl`은 `AsyncPipelineBuilder`를 통해 파이프라인을 구성·활성화하고, 추론 실행 중 입출력 버퍼를 연결하며, 종료 시 요소를 비활성화한다.【F:hailort/libhailort/src/net_flow/pipeline/async_infer_runner.cpp†L26-L200】

## 데이터 흐름 요약
1. `AsyncPipelineBuilder`가 입력 형식 확장과 전처리 큐를 생성하고, 하드웨어 요소와 링크한다.【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L72-L164】
2. 하드웨어 출력은 NMS/디멕스 요소를 거쳐 포맷 변환 후 최종 큐로 전달된다.【F:hailort/libhailort/src/net_flow/pipeline/async_pipeline_builder.cpp†L191-L360】
3. `AsyncInferRunnerImpl`이 파이프라인을 실행·중단하며, 오류 시 `AsyncPipeline`이 일괄 종료와 버퍼 반환을 수행한다.【F:hailort/libhailort/src/net_flow/pipeline/async_infer_runner.cpp†L62-L200】
