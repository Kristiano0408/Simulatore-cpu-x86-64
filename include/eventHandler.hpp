#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include <functional>
#include <map>
#include <string>
#include <any>
#include "helpers.hpp"

//in futuro sostituire stringhe con enum class (se possibile far in modo eh ne psoos inserie diversi)
template<typename T>
concept EnumType = std::is_enum_v<T>;

template<EnumType T>
class EventHandler
{
    public:
        EventHandler() {};

        inline void registerEvent(const T eventType, std::function<void()> callback) { eventMap.insert({eventType, callback}); }

        template<typename Targs, typename TReturn>
        void registerEvent(const T eventType, std::function<TReturn(Targs)> callback) 
        {
            if constexpr (std::is_same_v<TReturn, void>) 
            {
                if constexpr (std::is_same_v<Targs, void>)
                    eventMapTyped.insert({eventType, [callback](std::any){ callback(); return std::monostate{}; }}); 
                else
                eventMapTyped.insert({eventType, [callback](std::any arg){callback(std::any_cast<Targs>(arg)); return std::monostate{}; }}); 
            }
            else
            {   if constexpr (std::is_same_v<Targs, void>)
                    eventMapTyped.insert({eventType, [callback](std::any){ return callback(); }}); 
                else
                    eventMapTyped.insert({eventType, [callback](std::any arg){ return callback(std::any_cast<Targs>(arg)); }}); 
            }
        }

        inline void triggerEvent(const T eventType){ if(auto it = eventMap.find(eventType); it != eventMap.end()) { it->second(); } }


        template<typename Targs, typename TReturn>
        TReturn triggerEvent(const T eventType, Targs arg)
        { 
            if(auto it = eventMapTyped.find(eventType); it != eventMapTyped.end()) 
            {
                if constexpr (std::is_same_v<TReturn, void>) 
                {
                    if constexpr (std::is_same_v<Targs, void>)
                        it->second(std::monostate{}); // Call the callback with a dummy argument for void
                    else
                        it->second(arg); // Call the callback and ignore the return value
                    return; // Return void
                }
                else
                {   
                    if constexpr (std::is_same_v<Targs, void>)
                        return std::any_cast<TReturn>(it->second(std::monostate{})); // Call the callback with a dummy argument for void and return the result
                    else
                        return std::any_cast<TReturn>(it->second(arg)); // Call the callback and return the result
                }
            }
        }

        
        inline void clearEvents() { eventMap.clear(); }

        inline  auto getEventMap() const { return eventMap; }
        
        inline  std::function<void()> getCallback(const T eventType) {
            if(auto it = eventMap.find(eventType); it != eventMap.end()) {
                return it->second;
            }
            return nullptr;
        }

    private:
        std::map<T, std::function<void()>> eventMap;  //sostituire in futuro con unordered map o hash map per migliorare le prestazioni
        std::map<T, std::function<std::any(std::any)>> eventMapTyped; // Map to hold controller-specific events and callbacks

};


#endif // EVENT_HANDLER_HPP