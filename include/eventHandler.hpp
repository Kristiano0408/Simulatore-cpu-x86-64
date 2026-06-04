#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include <functional>
#include <map>
#include <string>
#include <any>
//in futuro sostituire stringhe con enum class (se possibile far in modo eh ne psoos inserie diversi)

class EventHandler
{
    public:
        EventHandler() {};

        inline void registerEvent(const std::string& eventName, std::function<void()> callback) { eventMap.insert({eventName, callback}); }

        template<typename T>
        inline void registerEvent(const std::string& eventName, std::function<void(T)> callback) { eventMapTyped.insert({eventName, [callback](std::any arg){ callback(std::any_cast<T>(arg)); }}); }

        inline void triggerEvent(const std::string& eventName){ if(auto it = eventMap.find(eventName); it != eventMap.end()) { it->second(); } }

        template<typename T>
        inline void triggerEvent(const std::string& eventName, T arg){ if(auto it = eventMapTyped.find(eventName); it != eventMapTyped.end()) { it->second(arg); } }

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
        std::map<std::string, std::function<void()>> eventMap;  //sostituire in futuro con unordered map o hash map per migliorare le prestazioni
        std::map<std::string, std::function<void(std::any)>> eventMapTyped; // Map to hold controller-specific events and callbacks

};


#endif // EVENT_HANDLER_HPP