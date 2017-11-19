//
//  Claire.hpp
//  clarie
//
//  Created by Chrystian Guth on 11/11/17.
//

#ifndef Claire_hpp
#define Claire_hpp

#include <Light.hpp>
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
    
    struct GrowBox
    {
    public:
        explicit GrowBox() = default;
        GrowBox(GrowBox&&) = default;
        GrowBox(const GrowBox&) = default;
        GrowBox& operator=(GrowBox&&) = default;
        virtual ~GrowBox() = default;
        
        LightState light() const;
        GrowBox light(LightState state, std::chrono::system_clock::time_point time = std::chrono::system_clock::now(), std::chrono::seconds repeat = std::chrono::seconds{0}) const;
        std::vector<Plant> plants() const;
        
        GrowBox put(Plant plant) const;
        GrowBox removePlant(Plant plant) const;
        
        std::optional<GrowBox> update(std::chrono::system_clock::time_point time) const;
        GrowBox shutdown(std::chrono::system_clock::time_point time = std::chrono::system_clock::now()) const;
        Light::EventMapType lightEvents() const;
        Plant plant(std::string name) const;

        friend std::ostream& operator<<(std::ostream& out, GrowBox box);
        
        template <class ValueType>
        GrowBox sensor(std::shared_ptr<SerialPort> serial, Plant plant, PlantProperty property, std::chrono::system_clock::time_point time, std::chrono::seconds repeat = std::chrono::seconds{0}) const
        {
            auto now = std::chrono::system_clock::now();
            GrowBox box{*this};
            box.sensors_[plant].events[std::move(time)] = std::make_tuple(SensorEvent{std::move(property), std::move(serial)}, std::move(repeat));
            if (time <= now)
            {
                box.sensors_.at(plant) = std::move(box.sensors_.at(plant)).update(std::move(now));
            }
            return box;
        }
    private:
        std::vector<Plant> plants_;
        std::chrono::system_clock::time_point updatedAt_;
        std::optional<std::chrono::system_clock::time_point> shutdownTime_;
        Light light_;
        std::map<Plant, Event<SensorEvent>> sensors_;
        
    };
}

#endif /* Claire_hpp */
