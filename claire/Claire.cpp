//
//  Claire.cpp
//  clarie
//
//  Created by Chrystian Guth on 11/11/17.
//

#include "Claire.hpp"
#include <ostream>
#include <iterator>

namespace claire {
    LightState GrowBox::light() const
    {
        return light_.now;
    }
    
    GrowBox GrowBox::light(LightState state, std::chrono::system_clock::time_point time, std::chrono::seconds repeat) const
    {
        auto now = std::chrono::system_clock::now();
        GrowBox box{*this};
        box.light_.events[std::move(time)] = std::make_tuple(state, repeat);
        if (time <= now)
        {
            box.light_ = std::move(box.light_).update(std::move(now));
        }
        return box;
    }
    
    std::vector<Plant> GrowBox::plants() const
    {
        return plants_;
    }
    
    std::tuple<GrowBox, Plant> GrowBox::addOrGetPlant(std::string name) const
    {
        GrowBox box(*this);
        Plant plant{std::move(name)};
        box.plants_.push_back(plant);
        return std::make_tuple(std::move(box), std::move(plant));
    }
    GrowBox GrowBox::removePlant(Plant plant) const
    {
        GrowBox box(*this);
        box.plants_.erase(std::remove(box.plants_.begin(), box.plants_.end(), std::move(plant)), box.plants_.end());
        return box;
    }
    std::optional<GrowBox> GrowBox::update(std::chrono::system_clock::time_point time) const
    {
        if (shutdownTime_)
        {
            if (*shutdownTime_ < std::move(time))
            {
                return std::nullopt;
            }
        }
        GrowBox box{*this};
        box.updatedAt_ = time;
        box.light_ = std::move(box.light_).update(time);
        return box;
    }
    GrowBox GrowBox::shutdown(std::chrono::system_clock::time_point time) const
    {
        GrowBox box{*this};
        box.shutdownTime_ = std::move(time);
        return box;
    }
    Light::EventMapType GrowBox::lightEvents() const
    {
        return light_.events;
    }
    std::ostream& operator<<(std::ostream& out, GrowBox box)
    {
        auto timet = std::chrono::system_clock::to_time_t(std::move(box).updatedAt_);
        out << "[\n\tGrowBox " << std::ctime(&timet);
        out << "\t\tlight is " << (box.light_.now == LightState::On ? "ON" : "OFF") << "\n";
        out << "\t\tplants (" << box.plants_.size() << "):\n";
        for (auto plant: box.plants_)
        {
            out << "\t\t\t" << std::move(plant) << "\n";
        }
        out << "\t\tscheduled light events(" << box.light_.events.size() << "): " << "\n";
        for (auto event: box.light_.events)
        {
            auto eventtimet = std::chrono::system_clock::to_time_t(event.first);
            out << "\t\t\t" << (std::get<0>(event.second) == LightState::On ? "ON" : "OFF") << " at " << std::ctime(&eventtimet);
        }
        out << "]";
        return out;
    }
    
}
