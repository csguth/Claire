//
//  Sensor.hpp
//  claire
//
//  Created by Chrystian Guth on 17/11/17.
//

#ifndef Sensor_hpp
#define Sensor_hpp

#include <memory>
#include <Integer.hpp>
#include <Plant.hpp>
#include <boost/asio.hpp>

namespace claire
{
    class SensorInput
    {
    public:
        virtual ~SensorInput() = default;
        virtual Integer<0, 1023> readNext() const = 0;
    private:
    };
    
    class SerialPort: public SensorInput
    {
    public:
        static std::unique_ptr<SerialPort> create(boost::asio::io_service& io, std::string path);
        Integer<0, 1023> readNext() const override;
    private:
        SerialPort() = default;
        SerialPort(SerialPort&&) = default;
        SerialPort& operator=(SerialPort&&) = default;
        explicit SerialPort(boost::asio::io_service& io, std::string path);
        
        boost::asio::serial_port port_;
    };
    
    struct SensorEvent
    {
        PlantProperty property;
        std::shared_ptr<SensorInput> serial;
    };
}

#endif /* Sensor_hpp */
