//
//  Sensor.cpp
//  claire
//
//  Created by Chrystian Guth on 17/11/17.
//

#include "Sensor.hpp"

namespace claire
{
    std::unique_ptr<DummySerialPort> DummySerialPort::create()
    {
        return std::make_unique<DummySerialPort>();
    }
    Integer<0, 1023> DummySerialPort::readNext() const
    {
        return Integer<0, 1023>{666};
    }
}
