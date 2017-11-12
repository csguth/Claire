//
//  LightsControl.hpp
//  claire
//
//  Created by Chrystian Guth on 11/11/17.
//

#ifndef Light_hpp
#define Light_hpp

#include <chrono>
#include <map>

namespace claire
{
    
    enum class LightState
    {
        On, Off
    };
    
    struct Light
    {
        using EventMapType = std::map<std::chrono::system_clock::time_point, std::tuple<LightState, std::chrono::seconds>>;
        EventMapType events;
        LightState now{LightState::Off};
        Light update(std::chrono::system_clock::time_point time) const;
    };
    
}

#endif /* Light_hpp */
