//
//  Sensor.hpp
//  claire
//
//  Created by Chrystian Guth on 17/11/17.
//

#ifndef Sensor_hpp
#define Sensor_hpp

#include <boost/asio.hpp>
#include <EventBase.hpp>
#include <Integer.hpp>
#include <memory>
#include <Plant.hpp>

namespace claire
{
    class SensorInput
    {
    public:
        virtual ~SensorInput() = default;
        virtual Integer<0, 1023> readNext() const = 0;
    private:
    };
    
    class DummySerialPort: public SensorInput
    {
    public:
        static std::unique_ptr<DummySerialPort> create();
        Integer<0, 1023> readNext() const override;
    };
    
    struct SensorEvent: EventBase<std::shared_ptr<SensorInput>, SensorEvent>
    {
        using EventBase<std::shared_ptr<SensorInput>, SensorEvent>::operator=;

    };
}

#endif /* Sensor_hpp */
