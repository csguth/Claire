//
//  Plant.cpp
//  claire
//
//  Created by Chrystian Guth on 11/11/17.
//

#include "Plant.hpp"

namespace claire
{
    Plant::Plant(std::string name):
    name_(std::move(name))
    {
        
    }
    std::string Plant::name() const
    {
        return name_;
    }
    bool Plant::operator==(const Plant& other) const
    {
        return name_ == other.name_;
    }
    bool Plant::operator<(const Plant& other) const
    {
        return name_ < other.name_;
    }
    double Plant::moisture() const
    {
        return moisture_;
    }
    Plant::operator std::string() const
    {
        return "\"Plant\" name=\""+name()+"\" moisture=\"" + std::to_string(moisture())  + "\"";
    }
    std::ostream& operator<<(std::ostream& out, const Plant& p)
    {
        return (out << static_cast<std::string>(p));
    }
}
