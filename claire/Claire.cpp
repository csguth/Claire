//
//  Claire.cpp
//  clarie
//
//  Created by Chrystian Guth on 11/11/17.
//

#include "Claire.hpp"
#include <ostream>
#include <iterator>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std::chrono_literals;
using std::chrono::system_clock;

namespace claire {
    
    LightState GrowBox::light() const
    {
        return light_.now;
    }
    std::vector<Plant> GrowBox::plants() const
    {
        return plants_;
    }
    Plant GrowBox::plant(std::string name) const
    {
        return *std::find_if(plants_.begin(), plants_.end(), [name](auto&& p){
            return p.name() == name;
        });
    }
    LightEvent::EventMapType GrowBox::lightEvents() const
    {
        return light_.events;
    }
    Plant GrowBox::moisture(double value, Plant plant) const
    {
        plant.moisture_ = value;
        return plant;
    }

    std::ostream& operator<<(std::ostream& out, GrowBox box)
    {
        auto timet = std::chrono::system_clock::to_time_t(std::move(box).updatedAt_);
        boost::property_tree::ptree root;
        root.put("last_updated", std::ctime(&timet));
        root.put("light", std::string{(box.light_.now == LightState::On ? "ON" : "OFF")});
        boost::property_tree::ptree& plants = root.add_child("plants", boost::property_tree::ptree{});
        for (auto plant: box.plants_)
        {
            boost::property_tree::ptree plant_pt;
            plant_pt.put("name", plant.name());
            plant_pt.put("moisture", plant.moisture());
            boost::property_tree::ptree& sensorEvents = plant_pt.add_child("sensors", boost::property_tree::ptree{});
            if (box.sensors_.count(std::make_pair(plant, PlantProperty::Moisture)) > 0)
            {
                for (auto event: box.sensors_.at(std::make_pair(plant, PlantProperty::Moisture)).events)
                {
                    auto eventtimet = std::chrono::system_clock::to_time_t(event.first);
                    sensorEvents.put("moisture", std::ctime(&eventtimet));
                }
            }
            plants.push_back(std::make_pair("", plant_pt));
        }
        boost::property_tree::ptree& lights = root.add_child("lights", boost::property_tree::ptree{});
        for (auto event: box.light_.events)
        {
            auto eventtimet = std::chrono::system_clock::to_time_t(event.first);
            boost::property_tree::ptree light_pt;
            light_pt.put("state", (std::get<0>(event.second) == LightState::On ? "ON" : "OFF"));
            light_pt.put("when", std::ctime(&eventtimet));
            lights.push_back(std::make_pair("", light_pt));
        }
        boost::property_tree::write_json(out, root);
        return out;
    }
    
    GrowBox light(LightState state, std::chrono::system_clock::time_point time, std::chrono::seconds repeat, GrowBox box)
    {
        auto now = std::chrono::system_clock::now();
        box.light_.events[std::move(time)] = std::make_tuple(state, repeat);
        if (time <= now)
        {
            box.light_ = std::move(box.light_).update(std::move(now));
        }
        return box;
    }
    
    GrowBox removePlant(Plant plant, GrowBox box)
    {
        box.plants_.erase(std::remove(box.plants_.begin(), box.plants_.end(), std::move(plant)), box.plants_.end());
        return box;
    }
    std::optional<GrowBox> update(std::chrono::system_clock::time_point time, GrowBox box)
    {
        if (box.shutdownTime_)
        {
            if (*box.shutdownTime_ < std::move(time))
            {
                return std::nullopt;
            }
        }
        box.updatedAt_ = time;
        
        auto sensors = box.sensors_;
        
        for (auto& sensor: sensors)
        {
            sensor.second = sensor.second.update(time);
            
            if (sensor.second.now)
            {
                auto value = sensor.second.now->readNext();
                Plant plant = box.plant(sensor.first.first.name());
                if (sensor.first.second == PlantProperty::Moisture)
                {
                    plant = box.moisture(static_cast<double>(sensor.second.now->readNext().get())/value.max(), std::move(plant));
                }
                box = put(std::move(plant), std::move(box));
                //                sensor.second.now.serial.reset();
            }
        }
        
        box.sensors_ = std::move(sensors);
        
        box.light_ = std::move(box.light_).update(time);
        return box;
    }
    GrowBox put(Plant plant, GrowBox box)
    {
        auto result = removePlant(plant, std::move(box));
        result.plants_.push_back(std::move(plant));
        return result;
    }
    
    GrowBox shutdown(std::chrono::system_clock::time_point time, GrowBox box)
    {
        box.shutdownTime_ = std::move(time);
        return box;
    }
    
    GrowBox sensor(std::shared_ptr<DummySerialPort> serial, Plant plant, PlantProperty property, std::chrono::system_clock::time_point time, std::chrono::seconds repeat, GrowBox box)
    
    {
        auto now = std::chrono::system_clock::now();
        box.sensors_[std::make_pair(plant, property)].events[std::move(time)] = std::make_tuple(serial, repeat);
        if (time <= now)
        {
            box.sensors_.at(std::make_pair(plant, property)) = std::move(box.sensors_.at(std::make_pair(plant, property))).update(std::move(now));
        }
        return box;
    }
    
    GrowBox photoperiod(std::chrono::seconds lightTime, std::chrono::seconds dayDuration, LightState initialState, GrowBox box)
    {
        auto now = system_clock::now();
        box.light_.events.clear();
        return light(initialState, now + (initialState == LightState::On?0s:lightTime), dayDuration,
                     light((initialState==LightState::Off?LightState::On:LightState::Off), now + lightTime, dayDuration, std::move(box)));
    }
    
}
