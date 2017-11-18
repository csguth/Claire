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

namespace claire
{
    class SensorInput
    {
    public:
        virtual ~SensorInput() = default;
        virtual std::unique_ptr<SensorInput> clone() const = 0;
        virtual Integer<0, 1023> readNext() const = 0;
    private:
    };
    
    class SerialPort: public SensorInput
    {
    public:
        explicit SerialPort(std::string path);
        std::unique_ptr<SensorInput> clone() const override;
        Integer<0, 1023> readNext() const override;
    private:
        std::string path_;
    };
    
    struct SensorEvent
    {
        Plant plant;
        PlantProperty property;
        std::shared_ptr<SensorInput> serial;
    };
}

#endif /* Sensor_hpp */
