#include "cache/cacheManager.hpp"
#include "bus.hpp"
#include "cache/cacheStorage.hpp"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

CacheManager::CacheManager(Bus& busReference, uint64_t l1Size, uint64_t l2Size, uint64_t l3Size, uint8_t l1Assoc, uint8_t l2Assoc, uint8_t l3Assoc, uint8_t l1Latency,
                           uint8_t l2Latency, uint8_t l3Latency, uint8_t l1FillLatency, uint8_t l2FillLatency, uint8_t l3FillLatency)
    : L3Cache(l3Size, l3Assoc, l3Latency, l3FillLatency, busReference, nullptr, nullptr, nullptr, CacheLevelType::L3, {nullptr, nullptr}, {nullptr, nullptr}),
      L2Cache(l2Size, l2Assoc, l2Latency, l2FillLatency, busReference, nullptr, nullptr, nullptr, CacheLevelType::L2, {nullptr, nullptr}, {nullptr, nullptr}),
      L1ICache(l1Size, l1Assoc, l1Latency, l1FillLatency, busReference, nullptr, nullptr, nullptr, CacheLevelType::L1I, {nullptr, nullptr}, {nullptr, nullptr}),
      L1DCache(l1Size, l1Assoc, l1Latency, l1FillLatency, busReference, nullptr, nullptr, nullptr, CacheLevelType::L1D, {nullptr, nullptr}, {nullptr, nullptr}),
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
    // L1D → L2
    L1DCache.getStorage().flush([&](uint64_t address, const CacheLine& line)
        {
            AddressInfo info = L2Cache.getController().decodeAddress(address);

            int8_t lineIndex = L2Cache.getStorage().findLineIndex(info.setIndex, info.tag);
            if (lineIndex == -1)
            {
                AddressInfo infoL3 = L3Cache.getController().decodeAddress(address);
                // If the line is not found in L2, we need to write to L3
                int8_t lineIndexL3 = L3Cache.getStorage().findLineIndex(infoL3.setIndex, infoL3.tag);
                if (lineIndexL3 == -1)
                {
                    // If the line is not found in L3, we need to write to RAM
                    bus.getMemory().write(address, line.data);
                }
                else
                {
                    
                    // If the line is found in L3, we write to L3
                    L3Cache.getStorage().loadLine(infoL3.setIndex, line, static_cast<uint8_t>(lineIndexL3));
                }
            }
            else
            {
                // If the line is found in L2, we write to L2
                L2Cache.getStorage().loadLine(info.setIndex, line, static_cast<uint8_t>(lineIndex));
            }
        });

    // L2 → L3
    L2Cache.getStorage().flush([&](uint64_t address, const CacheLine& line)
        {
            AddressInfo info = L3Cache.getController().decodeAddress(address);

            int8_t lineIndex = L3Cache.getStorage().findLineIndex(info.setIndex, info.tag);
            if (lineIndex == -1)
            {
                // If the line is not found in L3, we need to write to RAM
                bus.getMemory().write(address, line.data);
            }
            else
            {
                // If the line is found in L3, we write to L3
                L3Cache.getStorage().loadLine(info.setIndex, line, static_cast<uint8_t>(lineIndex));
            }
        });

    // L3 → RAM
    L3Cache.getStorage().flush([&](uint64_t address, const CacheLine& line) { bus.getMemory().write(address, line.data); });
}
