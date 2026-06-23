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

        template<typename Targs, typename TReturn>
        void registerEvent(const std::string& eventName, std::function<TReturn(Targs)> callback) 
        { 
            if constexpr (std::is_same_v<TReturn, void>) 
            {
                if constexpr (std::is_same_v<Targs, void>)
                    eventMapTyped.insert({eventName, [callback](std::any){ callback(); return std::monostate{}; }}); 
                else
                eventMapTyped.insert({eventName, [callback](std::any arg){callback(std::any_cast<Targs>(arg)); return std::monostate{}; }}); 
            }
            else
            {   if constexpr (std::is_same_v<Targs, void>)
                    eventMapTyped.insert({eventName, [callback](std::any){ return callback(); }}); 
                else
                eventMapTyped.insert({eventName, [callback](std::any arg){ return callback(std::any_cast<Targs>(arg)); }}); 
            }
            
        }


        inline void triggerEvent(const std::string& eventName){ if(auto it = eventMap.find(eventName); it != eventMap.end()) { it->second(); } }

        template<typename Targs, typename TReturn>
        TReturn triggerEvent(const std::string& eventName, Targs arg)
        { 
            if(auto it = eventMapTyped.find(eventName); it != eventMapTyped.end()) 
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
        
        inline  std::function<void()> getCallback(const std::string& eventName) 
        {
            if(auto it = eventMap.find(eventName); it != eventMap.end()) {
                return it->second;
            }
            return nullptr;
        }

    private:
        std::map<std::string, std::function<void()>> eventMap;  //sostituire in futuro con unordered map o hash map per migliorare le prestazioni
        std::map<std::string, std::function<std::any(std::any)>> eventMapTyped; // Map to hold controller-specific events and callbacks

};


#endif // EVENT_HANDLER_HPP