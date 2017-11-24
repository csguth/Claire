//
//  LightsControl.hpp
//  claire
//
//  Created by Chrystian Guth on 11/11/17.
//

#ifndef Light_hpp
#define Light_hpp

#include <EventBase.hpp>

namespace claire
{
    
    enum class LightState
    {
        On, Off
    };
    
    struct LightEvent: EventBase<LightState, LightEvent>
    {
        using EventBase<LightState, LightEvent>::operator=;
        
        LightEvent()
        {
            now = LightState::Off;
        }
    };

    
}

#endif /* Light_hpp */
