#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <Claire.hpp>
using namespace std::chrono_literals;
using namespace claire;
using std::chrono::system_clock;

TEST_CASE("Create a GrowBox")
{
    GrowBox box;
    REQUIRE(true);
}


struct LightsTest
{
    const LightState On{LightState::On};
    const LightState Off{LightState::Off};
};

TEST_CASE_METHOD(LightsTest, "Check light state")
{
    GrowBox box;
    REQUIRE(box.light() == Off);
}

TEST_CASE_METHOD(LightsTest, "Set light state")
{
    const GrowBox box;
    GrowBox box2 = Claire{}.light(On, system_clock::now(), 0h, box);
    INFO("Immutable");
    REQUIRE(box.light() == Off);
    REQUIRE(box2.light() == On);
}

TEST_CASE_METHOD(LightsTest, "Schedule repeating event")
{
    auto now = system_clock::now();
    auto box = Claire{}.light(On, now + 1h, 1h, GrowBox{});
    REQUIRE(box.light() == Off);
    REQUIRE_NOTHROW(std::get<0>(box.lightEvents().at(now + 1h)) == On);
    REQUIRE_NOTHROW(std::get<1>(box.lightEvents().at(now + 1h)) == 1h);
    auto box2 = Claire{}.update(now + 1h, box);
    REQUIRE_NOTHROW(std::get<0>(box2->lightEvents().at(now + 2h)) == On);
    REQUIRE_NOTHROW(std::get<1>(box2->lightEvents().at(now + 2h)) == 1h);
    

}

TEST_CASE_METHOD(LightsTest, "Schedule lights event")
{
    auto now = std::chrono::system_clock::now();
    auto box = Claire{}.light(On, now + 100*24h, 0h,
                                      Claire{}.light(Off, now + 200*24h, 0h,
                                                             GrowBox{}));
    REQUIRE(box.light() == Off);
    auto box2 = Claire{}.update(now + 50*24h, box);
    REQUIRE(box2->light() == Off);
    REQUIRE(box2->lightEvents().size() == 2);
    auto box3 = Claire{}.update(now + 99*24h, box);
    REQUIRE(box3->light() == Off);
    REQUIRE(box3->lightEvents().size() == 2);
    auto box4 = Claire{}.update(now + 100*24h, box);
    REQUIRE(box4->light() == On);
    REQUIRE(box4->lightEvents().size() == 1);
    auto box5 = Claire{}.update(now + 101*24h, box);
    REQUIRE(box5->light() == On);
    REQUIRE(box5->lightEvents().size() == 1);
    auto box6 = Claire{}.update(now + 200*24h, box);
    REQUIRE(box6->light() == Off);
    REQUIRE(box6->lightEvents().empty());
}

TEST_CASE_METHOD(LightsTest, "Photoperiod")
{
    auto now = std::chrono::system_clock::now();
    auto box = Claire{}.light(On, now, 24h,
                                      Claire{}.light(Off, now + 16h, 24h,
                                                             GrowBox{}));
    REQUIRE(box.light() == On);
    REQUIRE(box.lightEvents().size() == 2);
    auto box2 = Claire{}.update(now + 15h, box);
    REQUIRE(box2->light() == On);
    REQUIRE(box2->lightEvents().size() == 2);
    auto box3 = Claire{}.update(now + 16h, box);
    REQUIRE(box3->light() == Off);
    REQUIRE(box3->lightEvents().size() == 2);
    auto box4 = Claire{}.update(now + 23h + 59min + 59s, box);
    REQUIRE(box4->light() == Off);
    REQUIRE(box4->lightEvents().size() == 2);
    auto box5 = Claire{}.update(now + 24h, box);
    REQUIRE(box5->light() == On);
    REQUIRE(box5->lightEvents().size() == 2);
}

struct AddPlantTest
{
    GrowBox boxAfter;
    Plant plant{"MaryJ"};
    
    void addPlant()
    {
        boxAfter = Claire{}.put(plant, GrowBox{});
        INFO("Added plant " << plant);
    }
    
};

TEST_CASE_METHOD(AddPlantTest, "Check number of plants")
{
    GrowBox box;
    REQUIRE(box.plants().empty());
    REQUIRE(box.plants().size() == 0);
}

TEST_CASE("Check plant name")
{
    Plant plant;
    REQUIRE(plant.name().empty());
}

TEST_CASE_METHOD(AddPlantTest, "Add plant")
{
    addPlant();
    REQUIRE(GrowBox{}.plants().empty());
    REQUIRE(boxAfter.plants().size() == 1);
    REQUIRE(plant.name() == "MaryJ");
    REQUIRE(plant.name() == boxAfter.plants().front().name());
}

TEST_CASE_METHOD(AddPlantTest, "Plant as string")
{
    addPlant();
    std::string str = plant;
    REQUIRE(str == "\"Plant\" name=\"MaryJ\" moisture=\"0.000000\"");
    std::stringstream ss;
    ss << plant;
    REQUIRE(ss.str() == str);
}

TEST_CASE_METHOD(AddPlantTest, "Moisture")
{
    addPlant();
    REQUIRE(Approx(plant.moisture()) == 0.0);
    auto plantAfter = GrowBox{}.moisture(1.12345, plant);
    REQUIRE(Approx(plant.moisture()) == 0.0);
    REQUIRE(Approx(plantAfter.moisture()) == 1.12345);
}

struct RemovePlantTest: AddPlantTest
{
    RemovePlantTest()
    {
        addPlant();
    }
    
    void removePlant()
    {
        boxAfter = Claire{}.removePlant(plant, std::move(boxAfter));
    }
};

TEST_CASE_METHOD(RemovePlantTest, "Remove plant")
{
    removePlant();
    REQUIRE(GrowBox{}.plants().empty());
    REQUIRE(boxAfter.plants() == GrowBox{}.plants());
}

#include <thread>
#include <boost/asio.hpp>

TEST_CASE("application loop", "[grey]")
{
    auto now = system_clock::now();
    boost::asio::io_service io;
    auto initialState = Claire{}.sensor(SerialPort::create(io, "__dummy"), Plant{"skunk__1"}, PlantProperty::Moisture, now, 2s,
                                        Claire{}.put(Plant{"skunk__1"},
                                                     Claire{}.light(LightState::Off, now + 18s, 24s,
                                                                    Claire{}.light(LightState::On, now, 24s,
                                                                                   Claire{}.shutdown(now + 48s, GrowBox{})))));
    std::optional<GrowBox> box{initialState};
    while ((box = Claire{}.update(system_clock::now(), *box)))
    {
        io.run_one();
        std::clog << *box << std::endl;
        std::this_thread::sleep_for(1s);
    }
}

TEST_CASE("sensor test", "[sensor]")
{
    boost::asio::io_service io;
    auto now = system_clock::now();
    auto box = Claire{}.sensor(SerialPort::create(io, "__dummy"), Plant{"skunk#1"}, PlantProperty::Moisture, now + 1s, 5min,
                               Claire{}.put(Plant{"skunk#1"},
                                            GrowBox{}));
    auto box_after = Claire{}.update(now + 1s, box);
    io.run();
    REQUIRE(Approx(box.plant("skunk#1").moisture()) == 0.0);
    REQUIRE(Approx(box_after->plant("skunk#1").moisture()) == (666.0/1023.0));
}

TEST_CASE("photoperiod", "[photoperiod]")
{
    auto now = system_clock::now();
    auto firstDay = Claire{}.photoperiod(16h, LightState::On, GrowBox{});
    auto firstNight = Claire{}.update(now + 1h + 16h, firstDay);
    auto secondDay = Claire{}.update(now + 1h + 24h, firstDay);
    auto secondNight = Claire{}.update(now + 1h + 16h + 24h, firstDay);
    REQUIRE(firstDay.light() == LightState::On);
    REQUIRE(firstNight->light() == LightState::Off);
    REQUIRE(secondDay->light() == LightState::On);
    REQUIRE(secondNight->light() == LightState::Off);
    REQUIRE(firstDay.lightEvents().size() == 2);
    REQUIRE(firstNight->lightEvents().size() == 2);
    REQUIRE(secondDay->lightEvents().size() == 2);
    REQUIRE(secondNight->lightEvents().size() == 2);
}
