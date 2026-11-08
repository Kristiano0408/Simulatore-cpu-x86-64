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

        

        void onWaitingMemory(StageType stage);

        

        

        



        void onMemoryDone(StageType stage);

        



        void onFetchComplete();
        void onDecodeComplete();
        void onOperandFetchComplete();
        void onExecuteComplete();
        void onMemoryStageComplete();
        void onWriteBackComplete();

        
        bool isPipelineStalledForGUI() const;

        


    private:
        Pipeline& pipeline;
        

};

#endif // PIPELINECONTROLLER_HPP


