//
//  EventBase.hpp
//  claire
//
//  Created by Chrystian Guth on 23/11/17.
//

#ifndef EventBase_hpp
#define EventBase_hpp

#include <chrono>
#include <map>

namespace claire
{
    template <class ValueType, class EventType>
    struct EventBase
    {
        using EventMapType = std::map<std::chrono::system_clock::time_point, std::tuple<ValueType, std::chrono::seconds>>;
        ValueType now;
        EventMapType events;
        EventBase update(std::chrono::system_clock::time_point time) const
        {
            using namespace std::chrono_literals;
            
            EventBase light{*this};
            bool unrolling{true};
            while (unrolling)
            {
                EventMapType schedule;
                auto it = light.events.begin();
                for (; it != light.events.end() && it->first <= time; ++it)
                {
                    light.now = std::get<0>(it->second);
                    auto repeat = std::get<1>(it->second);
                    if (repeat != 0h)
                    {
                        schedule[it->first + repeat] = it->second;
                    }
                }
                if (schedule.empty())
                {
                    unrolling = false;
                }
                light.events.erase(light.events.begin(), it);
                light.events.insert(std::make_move_iterator(schedule.begin()), std::make_move_iterator(schedule.end()));
            }
            
            auto now = light.events.find(time);
            if (now != light.events.end())
            {
                light.now = std::get<0>(now->second);
                auto repeat = std::get<1>(now->second);
                if (repeat != 0h)
                    light.events[time + repeat] = now->second;
            }
            return light;
        }
    };
}

#endif /* EventBase_hpp */
