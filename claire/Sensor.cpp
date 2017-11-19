//
//  Sensor.cpp
//  claire
//
//  Created by Chrystian Guth on 17/11/17.
//

#include "Sensor.hpp"

namespace claire
{
    std::unique_ptr<SerialPort> SerialPort::create(boost::asio::io_service& io, std::string path)
    {
        return std::unique_ptr<SerialPort>(new SerialPort{io, std::move(path)});
    }
    SerialPort::SerialPort(boost::asio::io_service& io, std::string path):
        port_(io)
    {
        
    }
    Integer<0, 1023> SerialPort::readNext() const
    {
        return Integer<0, 1023>{666};
    }
}
