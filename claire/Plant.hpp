//
//  Plant.hpp
//  claire
//
//  Created by Chrystian Guth on 11/11/17.
//

#ifndef Plant_hpp
#define Plant_hpp

#include <string>
#include <ostream>

namespace claire
{
    struct Plant
    {
        Plant() = default;
        Plant(Plant&&) = default;
        Plant(const Plant&) = default;
        Plant& operator=(Plant&&) = default;
        Plant& operator=(const Plant&) = default;
        explicit Plant(std::string name);
        std::string name() const;
        bool operator==(const Plant&) const;
        operator std::string() const;
        double moisture() const;
        Plant moisture(double value) const;
        friend std::ostream& operator<<(std::ostream& out, const Plant& p);
        
    private:
        std::string name_;
        double moisture_{0.0};
        
    };
}

#endif /* Plant_hpp */