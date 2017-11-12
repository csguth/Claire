#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <Claire.hpp>
using namespace std::chrono_literals;


TEST_CASE("Create a GrowBox")
{
    claire::GrowBox box;
    REQUIRE(true);
}


struct LightsTest
{
    const claire::LightState On{claire::LightState::On};
    const claire::LightState Off{claire::LightState::Off};
};

TEST_CASE_METHOD(LightsTest, "Check light state")
{
    claire::GrowBox box;
    REQUIRE(box.light() == Off);
}

TEST_CASE_METHOD(LightsTest, "Set light state")
{
    claire::GrowBox box;
    claire::GrowBox box2 = box.light(On);
    INFO("Immutable");
    REQUIRE(box.light() == Off);
    REQUIRE(box2.light() == On);
}

TEST_CASE_METHOD(LightsTest, "Schedule repeating event")
{
    auto now = std::chrono::system_clock::now();
    std::optional<claire::GrowBox> box = claire::GrowBox{}.light(On, now + 1h, 1h);
    REQUIRE(box->light() == Off);
    REQUIRE_NOTHROW(std::get<0>(box->lightEvents().at(now + 1h)) == On);
    REQUIRE_NOTHROW(std::get<1>(box->lightEvents().at(now + 1h)) == 1h);
    auto box2 = box->update(now + 1h);
    REQUIRE_NOTHROW(std::get<0>(box2->lightEvents().at(now + 2h)) == On);
    REQUIRE_NOTHROW(std::get<1>(box2->lightEvents().at(now + 2h)) == 1h);
    

}

TEST_CASE_METHOD(LightsTest, "Schedule lights event")
{
    auto now = std::chrono::system_clock::now();
    std::optional<claire::GrowBox> box = claire::GrowBox{}.light(On, now + 100*24h).light(Off, now + 200*24h);
    REQUIRE(box->light() == Off);
    auto box2 = box->update(now + 50*24h);
    REQUIRE(box2->light() == Off);
    REQUIRE(box2->lightEvents().size() == 2);
    auto box3 = box->update(now + 99*24h);
    REQUIRE(box3->light() == Off);
    REQUIRE(box3->lightEvents().size() == 2);
    auto box4 = box->update(now + 100*24h);
    REQUIRE(box4->light() == On);
    REQUIRE(box4->lightEvents().size() == 1);
    auto box5 = box->update(now + 101*24h);
    REQUIRE(box5->light() == On);
    REQUIRE(box5->lightEvents().size() == 1);
    auto box6 = box->update(now + 200*24h);
    REQUIRE(box6->light() == Off);
    REQUIRE(box6->lightEvents().empty());
}

TEST_CASE_METHOD(LightsTest, "Photoperiod")
{
    auto now = std::chrono::system_clock::now();
    std::optional<claire::GrowBox> box = claire::GrowBox{}.light(On, now, 24h)
                                                          .light(Off, now + 16h, 24h);
    REQUIRE(box->light() == On);
    REQUIRE(box->lightEvents().size() == 2);
    auto box2 = box->update(now + 15h);
    REQUIRE(box2->light() == On);
    REQUIRE(box2->lightEvents().size() == 2);
    auto box3 = box->update(now + 16h);
    REQUIRE(box3->light() == Off);
    REQUIRE(box3->lightEvents().size() == 2);
    auto box4 = box->update(now + 23h + 59min + 59s);
    REQUIRE(box4->light() == Off);
    REQUIRE(box4->lightEvents().size() == 2);
    auto box5 = box->update(now + 24h);
    REQUIRE(box5->light() == On);
    REQUIRE(box5->lightEvents().size() == 2);
}

struct AddPlantTest
{
    claire::GrowBox box;
    claire::GrowBox boxAfter;
    claire::Plant plant;
    
    void addPlant()
    {
        std::tie(boxAfter, plant) = box.addOrGetPlant("MaryJ");
        INFO("Added plant " << plant);
    }
    
};

TEST_CASE_METHOD(AddPlantTest, "Check number of plants")
{
    claire::GrowBox box;
    REQUIRE(box.plants().empty());
    REQUIRE(box.plants().size() == 0);
}

TEST_CASE("Check plant name")
{
    claire::Plant plant;
    REQUIRE(plant.name().empty());
}

TEST_CASE_METHOD(AddPlantTest, "Add plant")
{
    addPlant();
    REQUIRE(box.plants().empty());
    REQUIRE(boxAfter.plants().size() == 1);
    REQUIRE(plant.name() == "MaryJ");
    REQUIRE(plant.name() == boxAfter.plants().front().name());
}

TEST_CASE_METHOD(AddPlantTest, "Plant as string")
{
    addPlant();
    std::string str = plant;
    REQUIRE(str == "\"Plant\" name=\"MaryJ\"");
    std::stringstream ss;
    ss << plant;
    REQUIRE(ss.str() == str);
}

TEST_CASE_METHOD(AddPlantTest, "Moisture")
{
    addPlant();
    REQUIRE(Approx(plant.moisture()) == 0.0);
    auto plantAfter = plant.moisture(1.12345);
    REQUIRE(Approx(plant.moisture()) == 0.0);
    REQUIRE(Approx(plantAfter.moisture()) == 1.12345);
}

struct RemovePlantTest: AddPlantTest
{
    RemovePlantTest()
    {
        addPlant();
        box = std::move(boxAfter);
        boxAfter = claire::GrowBox{};
    }
    
    void removePlant()
    {
        boxAfter = box.removePlant(plant);
    }
};

TEST_CASE_METHOD(RemovePlantTest, "Remove plant")
{
    removePlant();
    REQUIRE(box.plants().size() == 1);
    REQUIRE(boxAfter.plants().empty());
}

#include <thread>
TEST_CASE("application loop", "[grey]")
{
    using std::chrono::system_clock;
    auto now = system_clock::now();
    std::optional<claire::GrowBox> box = claire::GrowBox{}.shutdown(now + 168s)
                                                          .light(claire::LightState::On, now, 24s)
                                                          .light(claire::LightState::Off, now + 18s, 24s);
    std::tie(*box, std::ignore) = std::move(*box).addOrGetPlant("SensiSkunk#1");
    while ((box = std::move(*box).update(system_clock::now())))
    {
        std::clog << *box << std::endl;
        std::this_thread::sleep_for(1s);
    }
}
