#include "cache/cacheManager.hpp"
#include "bus.hpp"
#include "cache/cacheStorage.hpp"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

CacheManager::CacheManager(Bus& busReference, uint64_t l1Size, uint64_t l2Size, uint64_t l3Size, uint8_t l1Assoc,
                           uint8_t l2Assoc, uint8_t l3Assoc, uint8_t l1Latency, uint8_t l2Latency, uint8_t l3Latency,
                           uint8_t l1FillLatency, uint8_t l2FillLatency, uint8_t l3FillLatency)
    : L3Cache(l3Size, l3Assoc, l3Latency, l3FillLatency, busReference, nullptr, nullptr, nullptr, CacheLevelType::L3,
              {nullptr, nullptr}, {nullptr, nullptr}),
      L2Cache(l2Size, l2Assoc, l2Latency, l2FillLatency, busReference, nullptr, nullptr, nullptr, CacheLevelType::L2,
              {nullptr, nullptr}, {nullptr, nullptr}),
      L1ICache(l1Size, l1Assoc, l1Latency, l1FillLatency, busReference, nullptr, nullptr, nullptr, CacheLevelType::L1I,
               {nullptr, nullptr}, {nullptr, nullptr}),
      L1DCache(l1Size, l1Assoc, l1Latency, l1FillLatency, busReference, nullptr, nullptr, nullptr, CacheLevelType::L1D,
               {nullptr, nullptr}, {nullptr, nullptr}),
      memoryScheduler(busReference, memoryLatency), bus(busReference)
{
    DEBUG_LOG(debugLog("inizializzazione cache manager"));
    L3Cache.setParentLevel1(&L2Cache);

    L2Cache.setParentLevel1(&L1ICache);
    L2Cache.setParentLevel2(&L1DCache);
    L2Cache.setNextLevel(&L3Cache);

    L1ICache.setNextLevel(&L2Cache);
    L1DCache.setNextLevel(&L2Cache);

    L3Cache.setL1Ipath({&L2Cache, &L1ICache});
    L3Cache.setL1Dpath({&L2Cache, &L1DCache});
    L2Cache.setL1Ipath({nullptr, &L1ICache});
    L2Cache.setL1Dpath({nullptr, &L1DCache});
}

CacheManager::~CacheManager()
{
}

void CacheManager::enqueRequest(CacheRequest&& request, CacheLevelType cacheType)
{

    switch (cacheType)
    {
        case CacheLevelType::L1I:
            requestQueueL1I.push_back(std::move(request));
            break;
        case CacheLevelType::L1D:
            requestQueueL1D.push_back(std::move(request));
            break;
        default:
            break;
    }
}
void CacheManager::execute_operation()
{

    for (auto it = requestQueueL1I.begin(); it != requestQueueL1I.end();)
    {
        CacheRequest request = (*it);
        L1ICache.scheduleRequest(std::move(request));

        it = requestQueueL1I.erase(it);
    }

    for (auto it = requestQueueL1D.begin(); it != requestQueueL1D.end();)
    {
        CacheRequest request = (*it);
        L1DCache.scheduleRequest(std::move(request));

        it = requestQueueL1D.erase(it);
    }

    memoryScheduler.processMemoryRequests();
    L3Cache.execute_operation();
    L2Cache.execute_operation();
    L1ICache.execute_operation();
    L1DCache.execute_operation();
}

void CacheManager::flushAllCaches()
{
    L3Cache.getStorage().flush([&](uint64_t address, const LineData& line) { bus.getMemory().write(address, line); });
    L2Cache.getStorage().flush([&](uint64_t address, const LineData& line) { bus.getMemory().write(address, line); });
    L1ICache.getStorage().flush([&](uint64_t address, const LineData& line) { bus.getMemory().write(address, line); });
    L1DCache.getStorage().flush([&](uint64_t address, const LineData& line) { bus.getMemory().write(address, line); });
}