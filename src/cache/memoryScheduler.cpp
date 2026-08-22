#include "cache/memoryScheduler.hpp"    
#include "bus.hpp"


void MemoryScheduler::scheduleMemoryRequest(CacheRequest&& request)
{
    DEBUG_LOG(debugLog("richeista inserita in ram"));
    PendingRequest pendingRequest = PendingRequest(); // Create a new pending memory request to be scheduled for processing by the memory scheduler
    pendingRequest.request = request;   
    pendingRequest.remainingLatency = memoryLatency; // Set the remaining latency for the memory request based on the specified memory latency
    pendingRequest.state = RequestState::WAITING_LATENCY; // Set the initial state of the memory request to WAITING_LATENCY to indicate that it is waiting for the specified memory latency before being processed
    memoryRequestQueue.push_back(pendingRequest); // Add the pending memory request to the queue of pending requests to be processed by the memory scheduler
}

void MemoryScheduler::schedulePendingRequest(PendingRequest&& pendingRequest)
{
    pendingRequest.remainingLatency = memoryLatency; // Set the remaining latency for the pending request based on the specified memory latency
    pendingRequest.state = RequestState::WAITING_LATENCY; // Set the initial state of the pending request to WAITING_LATENCY to indicate that it is waiting for the specified memory latency before being processed
    memoryRequestQueue.push_back(pendingRequest); // Add the pending request to the queue of pending requests to be processed by the memory scheduler
}

void MemoryScheduler::processMemoryRequests()
{
    for (auto it = memoryRequestQueue.begin(); it != memoryRequestQueue.end();)
    {
        PendingRequest& pendingRequest = *it;

        if (pendingRequest.state == RequestState::WAITING_LATENCY) // If the pending request is in the WAITING_LATENCY state
        {
            if (pendingRequest.remainingLatency > 0) // If there is remaining latency for the pending request
            {
                --pendingRequest.remainingLatency; // Decrement the remaining latency for the pending request
            }
            if (pendingRequest.remainingLatency == 0) // If the remaining latency for the pending request has reached zero
            {
                pendingRequest.state = RequestState::READY_TO_PROCESS; // Set the state of the pending request to READY_TO_PROCESS to indicate that it is ready to be processed by the memory scheduler
            }
        }

        if (pendingRequest.state == RequestState::READY_TO_PROCESS) // If the pending request is in the READY_TO_PROCESS state
        {   
            processMemoryRequest(std::move(pendingRequest)); // Process the memory request using the processMemoryRequest function, which will handle the actual processing of the memory request based on its type and data
            it = memoryRequestQueue.erase(it); // Remove the processed memory request from the queue of pending requests
        }
        else
            ++it; // Move to the next pending request in the queue if the current request is not ready to be processed
    }
    
}

void MemoryScheduler::processMemoryRequest(PendingRequest&& pendingRequest)
{
    DEBUG_LOG(debugLog("esecuzione ram richeista:" + std::to_string(pendingRequest.request.requestID)));
    CacheRequest& originalRequest = pendingRequest.request; // Get the cache request from the pending request to be processed by the memory scheduler
    LineData readResponse;

    //calculating the adress for the start of the line
    uint64_t lineAddress = originalRequest.address & ~(CACHE_LINE_SIZE - 1); //BITMASK FOR REMOVING 6BITS FINALS TAHT RAPPRESENTS THE OFFSET

    

    switch (originalRequest.type) // Process the memory request based on its type (READ or WRITE) and handle the data accordingly
    {
        case RequestType::READ:
        {
            DEBUG_LOG(debugLog("lettura ram"));
            readResponse = bus.getMemory().read(lineAddress);
            
            DEBUG_LOG(debugLog("lettura ram completata"));

                auto makeFillPending =[&]() ->PendingRequest
                {
                    auto fillRequest = CacheRequest();
                    fillRequest.address = lineAddress;
                    fillRequest.dataType = TypeofData::ARRAY_64B;
                    fillRequest.type = RequestType::FILL;

                    CacheLine line = CacheLine();
                    line.data = readResponse;

                    auto fillPendingRequest = PendingRequest();
                    fillPendingRequest.line= line;
                    fillPendingRequest.request = fillRequest;

                    return fillPendingRequest;

                };

               
              

                bus.getCPU().getCacheManager().getL3Cache().schedulePendingRequest(makeFillPending());

        
                bus.getCPU().getCacheManager().getL2Cache().schedulePendingRequest(makeFillPending());

                switch (originalRequest.typeofL1)
                {
                    
                    case CacheLevelType::L1D:
                        bus.getCPU().getCacheManager().getL1DCache().schedulePendingRequest(makeFillPending());
                        break;
                    case CacheLevelType::L1I:
                        bus.getCPU().getCacheManager().getL1ICache().schedulePendingRequest(makeFillPending());
                        break;
                    default:
                        break;
                }
                
                //sending to L1 also teh original request to hit and responde to cpu
                
                switch (originalRequest.typeofL1)
                {
                case CacheLevelType::L1D:
                    bus.getCPU().getCacheManager().getL1DCache().schedulePendingRequest(std::move(pendingRequest));
                    break;
                case CacheLevelType::L1I:
                    bus.getCPU().getCacheManager().getL1ICache().schedulePendingRequest(std::move(pendingRequest));
                    break;
                default:
                    break;
                }

                DEBUG_LOG(debugLog("invio originale"));

            break;
        }
        case RequestType::READ_MEMORY_FOR_WRITE_MISS:
        {
            readResponse = bus.getMemory().read(lineAddress);
           
                auto fillRequest = CacheRequest();
                fillRequest.address = lineAddress;
                fillRequest.dataType = TypeofData::ARRAY_64B;
                fillRequest.type = RequestType::FILL;

                CacheLine line = CacheLine();
                line.data = readResponse;

                auto fillPendingRequest = PendingRequest();
                fillPendingRequest.request = fillRequest;
                fillPendingRequest.line = line;

                DEBUG_LOG(debugLog("invio fill L3"));
                bus.getCPU().getCacheManager().getL3Cache().schedulePendingRequest(std::move(fillPendingRequest));

                //sending to L1 also teh original request to hit and responde to cpu
                originalRequest.type = RequestType::WRITE;
                DEBUG_LOG(debugLog("invio richeista originale"));
                bus.getCPU().getCacheManager().getL3Cache().schedulePendingRequest(std::move(pendingRequest));
                
            break;
        }
        case RequestType::WRITE:
        {
            bus.getMemory().write(lineAddress, pendingRequest.line.data);

            //gestire in futuro errori

            break;
        }
        default:
            break;
    }

    
}
