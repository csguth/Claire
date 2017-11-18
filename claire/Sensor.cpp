//
//  Sensor.cpp
//  claire
//
//  Created by Chrystian Guth on 17/11/17.
//

#include "Sensor.hpp"

namespace claire
{
    SerialPort::SerialPort(std::string)
    {
        
    }
    std::unique_ptr<SensorInput> SerialPort::clone() const
    {
        return std::make_unique<SerialPort>("dummy");
    }
    Integer<0, 1023> SerialPort::readNext() const
    {
        static Integer<0, 1023> curr;
        curr = std::move(curr).set(curr.get()+1);
        return curr;
    }
}
