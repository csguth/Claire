//
//  Claire.hpp
//  clarie
//
//  Created by Chrystian Guth on 11/11/17.
//

#ifndef Claire_hpp
#define Claire_hpp

#include <LightEvent.hpp>
#include <Plant.hpp>
#include <Integer.hpp>
#include <Sensor.hpp>
#include <experimental/optional>
#include <tuple>
#include <vector>
#include <istream>

namespace std {
    using namespace experimental;
}

namespace claire {
    
    class Claire;
    class GrowBox
    {
    public:
        friend Claire;
        GrowBox() = default;
        GrowBox(GrowBox&&) = default;
        GrowBox(const GrowBox&) = default;
        GrowBox& operator=(GrowBox&&) = default;
        virtual ~GrowBox() = default;
        
        LightState light() const;
        std::vector<Plant> plants() const;
        LightEvent::EventMapType lightEvents() const;
        Plant plant(std::string name) const;
        Plant moisture(double value, Plant plant) const;

        friend std::ostream& operator<<(std::ostream& out, GrowBox box);
        
        std::vector<Plant> plants_;
        std::chrono::system_clock::time_point updatedAt_;
        std::optional<std::chrono::system_clock::time_point> shutdownTime_;
        LightEvent light_;
        std::map<std::pair<Plant, PlantProperty>, SensorEvent> sensors_;
        
    };
    
    GrowBox light(LightState state, std::chrono::system_clock::time_point time, std::chrono::seconds repeat, GrowBox box);
    GrowBox put(Plant plant, GrowBox box);
    GrowBox removePlant(Plant plant, GrowBox box);
    std::optional<GrowBox> update(std::chrono::system_clock::time_point time, GrowBox box);
    GrowBox shutdown(std::chrono::system_clock::time_point time, GrowBox box);
    GrowBox sensor(std::shared_ptr<DummySerialPort> serial, Plant plant, PlantProperty property, std::chrono::system_clock::time_point time, std::chrono::seconds repeat, GrowBox box);
    GrowBox photoperiod(std::chrono::seconds lightTime, std::chrono::seconds dayDuration, LightState initialState, GrowBox box);
}

#endif /* Claire_hpp */
