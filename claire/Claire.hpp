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
#include <experimental/optional>
#include <tuple>
#include <vector>

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
        
        std::tuple<GrowBox, Plant> addOrGetPlant(std::string name) const;
        GrowBox removePlant(Plant plant) const;
        
        std::optional<GrowBox> update(std::chrono::system_clock::time_point time) const;
        GrowBox shutdown(std::chrono::system_clock::time_point time = std::chrono::system_clock::now()) const;
        Light::EventMapType lightEvents() const;

        friend std::ostream& operator<<(std::ostream& out, GrowBox box);
    private:
        std::vector<Plant> plants_;
        std::chrono::system_clock::time_point updatedAt_;
        std::optional<std::chrono::system_clock::time_point> shutdownTime_;
        Light light_;
        
    };
}

#endif /* Claire_hpp */
