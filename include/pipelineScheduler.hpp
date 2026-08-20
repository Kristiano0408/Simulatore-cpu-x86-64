#ifndef PIPELINE_SCHEDULER_HPP
#define PIPELINE_SCHEDULER_HPP

#include <cstdint>

class Pipeline;
class CPU;
class InstructionInfo;

class PipelineScheduler 
{
    public:
        PipelineScheduler(Pipeline& pipeline) : pipeline(pipeline) {}

        void processFetchStage(CPU& cpu);
        void processDecodeStage(CPU& cpu);
        void processOperandFetchStage();
        void processExecuteStage();
        void processMemoryStage();
        void processWriteBackStage();

    private:
        Pipeline& pipeline;

        void sendWriteBackRequestToExecuteEngine();
        void sendMemoryAccessRequestToExecuteEngine();
        void sendExecutionRequestToExecuteEngine();
        void sendOperandFetchRequestToExecuteEngine();
        void decodeInstruction(CPU& cpu);
        void startFetch(CPU& cpu);
        void updateFetch(CPU& cpu);
        InstructionInfo fetchInstruction(CPU& cpu);

        // Tracking instruction IDs in fetch for setting it in decode and using it to track instructions
        uint64_t fetchStageInstructionId {};

        //necessary varaible for fetching instruction
        uint64_t index = 0; //index for fetching instruction from memory (in future could be part of fetch stage)
};















#endif // PIPELINE_SCHEDULER_HPP