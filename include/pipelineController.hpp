#ifndef PIPELINECONTROLLER_HPP
#define PIPELINECONTROLLER_HPP

#include <vector>
#include "helpers.hpp"
#include "pipeline.hpp"
#include "eventHandler.hpp"

class CPU; // Forward declaration
class Bus; // Forward declaration


class PipelineController
{
    public:

        PipelineController(Pipeline& pipelineRef);

        void setupEvents();

        void onStageComplete(StageType stage);

        void onWaitingMemory(StageType stage);

        void onMemoryDone(StageType stage);

        void stallForGUI();

        inline EventHandler<EventHandlerPipelineEventType>& getEventHandler() { return eventHandler; }


    private:
        Pipeline& pipeline;
        EventHandler<EventHandlerPipelineEventType> eventHandler;

};
#endif // PIPELINECONTROLLER_HPP


