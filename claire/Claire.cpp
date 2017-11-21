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
    Light::EventMapType GrowBox::lightEvents() const
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
            for (auto event: box.sensors_.at(plant).events)
            {
                auto eventtimet = std::chrono::system_clock::to_time_t(event.first);
                std::string sensortype = (std::get<0>(event.second).property == PlantProperty::Moisture ? "moisture" : "sensor");
                sensorEvents.put(sensortype, std::ctime(&eventtimet));
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
    
    GrowBox Claire::light(LightState state, std::chrono::system_clock::time_point time, std::chrono::seconds repeat, GrowBox box) const
    {
        auto now = std::chrono::system_clock::now();
        box.light_.events[std::move(time)] = std::make_tuple(state, repeat);
        if (time <= now)
        {
            box.light_ = std::move(box.light_).update(std::move(now));
        }
        return box;
    }
    
    GrowBox Claire::removePlant(Plant plant, GrowBox box) const
    {
        box.plants_.erase(std::remove(box.plants_.begin(), box.plants_.end(), std::move(plant)), box.plants_.end());
        return box;
    }
    std::optional<GrowBox> Claire::update(std::chrono::system_clock::time_point time, GrowBox box) const
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
            
            if (sensor.second.now.serial && sensor.second.now.property == PlantProperty::Moisture)
            {
                auto value = sensor.second.now.serial->readNext();
                auto plant = box.moisture(static_cast<double>(sensor.second.now.serial->readNext().get())/value.max(), box.plant(sensor.first.name()));
                box = put(std::move(plant), std::move(box));
                //                sensor.second.now.serial.reset();
            }
        }
        
        box.light_ = std::move(box.light_).update(time);
        return box;
    }
    GrowBox Claire::put(Plant plant, GrowBox box) const
    {
        auto result = removePlant(plant, std::move(box));
        result.plants_.push_back(std::move(plant));
        return result;
    }
    
    GrowBox Claire::shutdown(std::chrono::system_clock::time_point time, GrowBox box) const
    {
        box.shutdownTime_ = std::move(time);
        return box;
    }
    
    GrowBox Claire::sensor(std::shared_ptr<SerialPort> serial, Plant plant, PlantProperty property, std::chrono::system_clock::time_point time, std::chrono::seconds repeat, GrowBox box) const
    
    {
        auto now = std::chrono::system_clock::now();
        box.sensors_[plant].events[std::move(time)] = std::make_tuple(SensorEvent{std::move(property), std::move(serial)}, std::move(repeat));
        if (time <= now)
        {
            box.sensors_.at(plant) = std::move(box.sensors_.at(plant)).update(std::move(now));
        }
        return box;
    }
    
}
