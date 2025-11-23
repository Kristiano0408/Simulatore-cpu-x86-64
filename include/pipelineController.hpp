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


    private:
        Pipeline& pipeline;
        EventHandler eventHandler;

};
#endif // PIPELINECONTROLLER_HPP


