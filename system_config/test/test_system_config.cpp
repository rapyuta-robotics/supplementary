#include "essentials/SystemConfig.h"
#include "essentials/Configuration.h"
#include "essentials/IAlicaConfig.h"

#include <gtest/gtest.h>

#include <string>

// Declare a test
TEST(SystemConfigBasics, readValues)
{
    // bring up the SystemConfig with the corresponding path
    essentials::SystemConfig sc;
    sc.setRootPath(".");
    sc.setConfigPath("./etc");

    // read int
    unsigned short uShortTestValue = sc["Test"]->get<unsigned short>("uShortTestValue");
    EXPECT_EQ(3, uShortTestValue);

    // read int
    int intTestValue = sc["Test"]->get<int>("intTestValue");
    EXPECT_EQ(221, intTestValue);

    // read double
    double doubleTestValue = sc["Test"]->get<double>("doubleTestValue");
    EXPECT_DOUBLE_EQ(0.66234023823, doubleTestValue);

    // read float
    float floatTestValue = sc["Test"]->get<float>("floatTestValue");
    EXPECT_FLOAT_EQ(1.14f, floatTestValue);

    // read spaceTestValue
    int spaceTestValue = sc["Test"]->get<int>("spaceTestValue");
    EXPECT_FLOAT_EQ(6, spaceTestValue);

    // read list of strings
    std::vector<std::string> stringListTestValue = sc["Test"]->getList<std::string>("stringListTestValue");
    std::vector<std::string> referenceStringList{"asdf", "bla", "blub", "hust hust", "möp"};
    for (int i = 0; i < stringListTestValue.size(); i++) {
        EXPECT_STREQ(stringListTestValue[i].c_str(), referenceStringList[i].c_str());
    }

    // read TestSectionValue1
    std::string testSectionValue1 = sc["Test"]->get<std::string>("TestSection.TestSectionValue1");
    EXPECT_STREQ("TestSectionValue1", testSectionValue1.c_str());

    // read TestSectionValue2
    float testSectionValue2 = sc["Test"]->get<float>("TestSection.TestSectionValue2");
    EXPECT_FLOAT_EQ(0.66412f, testSectionValue2);
}
// Run all the tests that were declared with TEST()
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
