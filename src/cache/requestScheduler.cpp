#include "cache/requestScheduler.hpp"




void RequestScheduler::processRequests()
{
    for (auto it = pendingRequests.begin(); it != pendingRequests.end();)
    {
        if (it->state == RequestState::WAITING_LATENCY) // Check if the request is in WAITING state
        {
            --(it->remainingLatency); // Decrement the remaining latency for the request( IT CANT BE ZERO)

            if (it->remainingLatency == 0) // If the request is ready to be processed
            {
                it->state = RequestState::READY_TO_PROCESS; // Update the state to READY_TO_PROCESS
                DEBUG_LOG(debugLog("n di richeiste: " + std::to_string(pendingRequests.size())));
                cacheControllerCallback(CallbackContext, *it); // Call the callback function to process the request using the provided context
                it = pendingRequests.erase(it); // Remove the request from the pending requests vector after processing
                DEBUG_LOG(debugLog("n di richeiste dopo esecuzione: " + std::to_string(pendingRequests.size())));

            }
            else
            {
                ++it; // Move to the next request in the vector
            }
        }
        else
        {
            ++it; // Move to the next request in the vector if it's not in WAITING state
        }
    }
}

void RequestScheduler::scheduleRequest(CacheRequest&& request)
{
    DEBUG_LOG(debugLog("richiesta inserita"));
    uint8_t latency = ((request.type == (RequestType::FILL)) || (request.type == (RequestType::READ_AFTER_FILL)) || (request.type == (RequestType::WRITE_AFTER_FILL))) ? this->fillLatency : this->latency; 
    pendingRequests.emplace_back(std::move(request), latency, RequestState::WAITING_LATENCY); // Add a new request to the pending requests vector with the specified latency
}

void RequestScheduler::schedulePendingRequest(PendingRequest&& pendingRequest)
{   
    pendingRequest.state = RequestState::WAITING_LATENCY; // Set the state of the pending request to WAITING_LATENCY before scheduling
    pendingRequest.remainingLatency = ((pendingRequest.request.type == (RequestType::FILL)) || (pendingRequest.request.type == (RequestType::READ_AFTER_FILL)) || (pendingRequest.request.type == (RequestType::WRITE_AFTER_FILL)) ) ? this->fillLatency : this->latency; 
    pendingRequests.push_back(std::move(pendingRequest)); // Add a pending request to the pending requests vector for processing
}
