#ifndef PIPELINECONTROLLER_HPP
#define PIPELINECONTROLLER_HPP

#include <vector>
#include "helpers.hpp"
#include "pipeline.hpp"
#include "eventHandler.hpp"


class PipelineController
{
    public:

        PipelineController(Pipeline& pipelineRef);

        void setupEvents();

        void onWaitingMemory(StageType stage);

        static void onWaitingMemoryWrapperFetch(void* context);

        static void onWaitingMemoryWrapperExecute(void* context);

        static void onWaitingMemoryWrapperMemoryStage(void* context);



        void onMemoryDone(StageType stage);

        static void onMemoryDoneWrapperFetch(void* context);

        static void onMemoryDoneWrapperExecute(void* context);

        static void onMemoryDoneWrapperMemoryStage(void* context);



        void onFetchComplete();

        static void onFetchCompleteWrapper(void* context);

        void onDecodeComplete();

        static void onDecodeCompleteWrapper(void* context);

        void onOperandFetchComplete();

        static void onOperandFetchCompleteWrapper(void* context);

        void onExecuteComplete();

        static void onExecuteCompleteWrapper(void* context);

        void onMemoryStageComplete();

        static void onMemoryStageCompleteWrapper(void* context);

        void onWriteBackComplete();

        static void onWriteBackCompleteWrapper(void* context);

        bool isPipelineStalledForGUI() const;

        inline PipelineEventHandler* getEventHandler() { return &eventHandler; }


    private:
        Pipeline& pipeline;
        PipelineEventHandler eventHandler;

};

#endif // PIPELINECONTROLLER_HPP


