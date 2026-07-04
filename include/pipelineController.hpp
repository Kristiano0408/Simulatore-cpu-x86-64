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

        void onMemoryDone(StageType stage);

        void onFetchComplete();

        void onDecodeComplete();

        void onOperandFetchComplete();

        void onExecuteComplete();

        void onMemoryStageComplete();

        void onWriteBackComplete();

        bool isPipelineStalledForGUI() const;

        inline EventHandler<EventHandlerPipelineEventType>& getEventHandler() { return eventHandler; }


    private:
        Pipeline& pipeline;
        EventHandler<EventHandlerPipelineEventType> eventHandler;

};

#endif // PIPELINECONTROLLER_HPP


