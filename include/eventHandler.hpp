#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include <functional>
#include <map>
#include <string>


class EventHandler
{
    public:
        EventHandler() {};

        inline void registerEvent(const std::string& eventName, std::function<void()> callback) { eventMap.insert({eventName, callback}); }

        inline void triggerEvent(const std::string& eventName){ if(auto it = eventMap.find(eventName); it != eventMap.end()) { it->second(); } }

        inline void clearEvents() { eventMap.clear(); }

        inline  auto getEventMap() const { return eventMap; }
        
        inline  std::function<void()> getCallback(const std::string& eventName) 
        {
            if(auto it = eventMap.find(eventName); it != eventMap.end()) {
                return it->second;
            }
            return nullptr;
        }

    private:
        std::map<std::string, std::function<void()>> eventMap;

};


#endif // EVENT_HANDLER_HPP