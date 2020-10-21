#include "essentials/SystemConfig.h"
#include "essentials/Configuration.h"
#include "essentials/ConfigNode.h"

#include <gtest/gtest.h>

#include <string>

TEST(SystemConfigBasics, sections)
{
essentials::SystemConfig<essentials::Configuration> sc;
sc.setRootPath(".");
sc.setConfigPath("./etc");

auto result = sc["Test"].getSections("TestSection").get();
EXPECT_TRUE(result->empty());

result = sc["Test"].getNames("TestSection").get();
EXPECT_TRUE(!result->empty());
//Iterate over result, check if names are correct
}

// IAlicaConfig *ptr = new Configuration()
// Configuration *ptr = new Configuration()
// sc["Test"][Test][Path][intValue]
// sc["Test"][Test][Path][intValue] = value
// sc.tryGet("examplevalue", "Test", "path");
// sc.trySet(13, "Test", "Path", "intValue");
// sc.load()
// sc.store()

TEST(SystemConfigBasics, writeValues)
{
    essentials::SystemConfig<essentials::Configuration> sc;
    sc.setRootPath(".");
    sc.setConfigPath("./etc");

    sc["Test"].setCreateIfNotExistent<int>(13, "Test.Path.intValue");
    EXPECT_EQ(13, sc["Test"].get<int>("Test.Path.intValue"));

    sc["Test"].setCreateIfNotExistent<bool>(false, "Test.Path.boolValue");
    EXPECT_EQ(false, sc["Test"].get<bool>("Test.Path.boolValue"));

    sc["Test"].setCreateIfNotExistent<std::string>("hello world", "Test.Path.stringValue");
    EXPECT_STREQ("hello world", sc["Test"].get<std::string>("Test.Path.stringValue").c_str());

    sc["Test"].setCreateIfNotExistent<double>(17.3, "Test.Path.doubleValue");
    EXPECT_DOUBLE_EQ(17.3, sc["Test"].get<double>("Test.Path.doubleValue"));

    sc["Test"].setCreateIfNotExistent<float>(19.3f, "Test.Path.floatValue");
    EXPECT_FLOAT_EQ(19.3f, sc["Test"].get<float>("Test.Path.floatValue"));

    sc["Test"].setCreateIfNotExistent<unsigned short>(7, "Test.Path.ushortValue");
    EXPECT_EQ(7, sc["Test"].get<unsigned short>("Test.Path.ushortValue"));

    sc["Test"].set<int>(14, "Test.Path.intValue");
    EXPECT_EQ(14, sc["Test"].get<int>("Test.Path.intValue"));

    sc["Test"].set<bool>(true, "Test.Path.boolValue");
    EXPECT_EQ(true, sc["Test"].get<bool>("Test.Path.boolValue"));

    sc["Test"].set<std::string>("hello world", "Test.Path.stringValue");
    EXPECT_STREQ("hello world", sc["Test"].get<std::string>("Test.Path.stringValue").c_str());

    sc["Test"].set<double>(17.4, "Test.Path.doubleValue");
    EXPECT_DOUBLE_EQ(17.4, sc["Test"].get<double>("Test.Path.doubleValue"));

    sc["Test"].set<float>(19.4f, "Test.Path.floatValue");
    EXPECT_FLOAT_EQ(19.4f, sc["Test"].get<float>("Test.Path.floatValue"));

    sc["Test"].set<unsigned short>(8, "Test.Path.ushortValue");
    EXPECT_EQ(8, sc["Test"].get<unsigned short>("Test.Path.ushortValue"));
}

// Declare a test
TEST(SystemConfigBasics, readValues)
{
    // bring up the SystemConfig with the corresponding path
    essentials::SystemConfig<essentials::Configuration> sc;
    sc.setRootPath(".");
    sc.setConfigPath("./etc");

    // read int
    unsigned short uShortTestValue = sc["Test"].get<unsigned short>("uShortTestValue");
    EXPECT_EQ(3, uShortTestValue);

    // read int
    int intTestValue = sc["Test"].get<int>("intTestValue");
    EXPECT_EQ(221, intTestValue);

    // read double
    double doubleTestValue = sc["Test"].get<double>("doubleTestValue");
    EXPECT_DOUBLE_EQ(0.66234023823, doubleTestValue);

    // read float
    float floatTestValue = sc["Test"].get<float>("floatTestValue");
    EXPECT_FLOAT_EQ(1.14f, floatTestValue);

    // read spaceTestValue
    int spaceTestValue = sc["Test"].get<int>("spaceTestValue");
    EXPECT_FLOAT_EQ(6, spaceTestValue);

    // read list of strings
    std::vector<std::string> stringListTestValue = sc["Test"].getList<std::string>("stringListTestValue");
    std::vector<std::string> referenceStringList{"asdf", "bla", "blub", "hust hust", "möp"};
    for (int i = 0; i < stringListTestValue.size(); i++) {
        EXPECT_STREQ(stringListTestValue[i].c_str(), referenceStringList[i].c_str());
    }

    // read TestSectionValue1
    std::string testSectionValue1 = sc["Test"].get<std::string>("TestSection.TestSectionValue1");
    EXPECT_STREQ("TestSectionValue1", testSectionValue1.c_str());

    // read TestSectionValue2
    float testSectionValue2 = sc["Test"].get<float>("TestSection.TestSectionValue2");
    EXPECT_FLOAT_EQ(0.66412f, testSectionValue2);
}
// Run all the tests that were declared with TEST()
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
