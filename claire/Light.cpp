//
//  Light.cpp
//  claire
//
//  Created by Chrystian Guth on 11/11/17.
//

#include "Light.hpp"

namespace claire
{
    using namespace std::chrono_literals;
    Light Light::update(std::chrono::system_clock::time_point time) const
    {
        Light light{*this};
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

}
