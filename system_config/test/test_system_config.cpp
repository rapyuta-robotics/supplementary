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

TEST(SystemConfigBasics, bracketAccess)
{
    essentials::SystemConfig<essentials::Configuration> sc;
    sc.setRootPath(".");
    sc.setConfigPath("./etc");

    EXPECT_EQ(14, sc["Test"]["Example"]["Path"]["intValue"].as<int>());

    sc["Test"]["Example"]["Path"]["intValue"] = "15";
    EXPECT_EQ(15, sc["Test"]["Example"]["Path"]["intValue"].as<int>());
}

TEST(SystemConfigBasics, readValuesWithBrackets)
{
    essentials::SystemConfig<essentials::Configuration> sc;
    sc.setRootPath(".");
    sc.setConfigPath("./etc");

    EXPECT_EQ("hello world", sc["Test"]["Example"]["Path"]["stringValue"].as<std::string>());
    EXPECT_EQ(14, sc["Test"]["Example"]["Path"]["intValue"].as<int>());
    EXPECT_EQ(true, sc["Test"]["Example"]["Path"]["boolValue"].as<bool>());
    EXPECT_EQ(17.4, sc["Test"]["Example"]["Path"]["doubleValue"].as<double>());
    EXPECT_EQ(19.4f, sc["Test"]["Example"]["Path"]["floatValue"].as<float>());
    EXPECT_EQ(8, sc["Test"]["Example"]["Path"]["ushortValue"].as<unsigned short>());
}

TEST(SystemConfigBasics, readValuesWithTryGet)
{
    essentials::SystemConfig<essentials::Configuration> sc;
    sc.setRootPath(".");
    sc.setConfigPath("./etc");

    EXPECT_EQ("hello world", sc["Test"].tryGetString("defaultValue", "Example", "Path", "stringValue", NULL));
    EXPECT_EQ("defaultValue", sc["Test"].tryGetString("defaultValue", "Wrong", "Path", NULL));

    EXPECT_EQ(14, sc["Test"].tryGetInt(19, "Example", "Path", "intValue", NULL));
    EXPECT_EQ(19, sc["Test"].tryGetInt(19, "Wrong", "Path", NULL));

    EXPECT_EQ(true, sc["Test"].tryGetBool(false, "Example", "Path", "boolValue", NULL));
    EXPECT_EQ(false, sc["Test"].tryGetBool(false, "Wrong", "Path", NULL));

    EXPECT_EQ(17.4, sc["Test"].tryGetDouble(17.4, "Example", "Path", "doubleValue", NULL));
    EXPECT_EQ(19.576, sc["Test"].tryGetDouble(19.576, "Wrong", "Path", NULL));

    EXPECT_EQ(19.4f, sc["Test"].tryGetFloat(12.451f, "Example", "Path", "floatValue", NULL));
    EXPECT_EQ(12.451f, sc["Test"].tryGetFloat(12.451f, "Wrong", "Path", NULL));

    EXPECT_EQ(8, sc["Test"].tryGetUShort(4, "Example", "Path", "ushortValue", NULL));
    EXPECT_EQ(4, sc["Test"].tryGetUShort(4, "Wrong", "Path", NULL));
}

TEST(SystemConfigBasics, writeValues)
{
    essentials::SystemConfig<essentials::Configuration> sc;
    sc.setRootPath(".");
    sc.setConfigPath("./etc");

    sc["Test"].setCreateIfNotExistentInt(13, "Test", "Path", "intValue", NULL);
    EXPECT_EQ(13, sc["Test"]["Test"]["Path"]["intValue"].as<int>());

    sc["Test"].setCreateIfNotExistentBool(false, "Test", "Path", "boolValue", NULL);
    EXPECT_EQ(false, sc["Test"]["Test"]["Path"]["boolValue"].as<bool>());

    sc["Test"].setCreateIfNotExistentString("hello world", "Test", "Path", "stringValue", NULL);
    EXPECT_STREQ("hello world", sc["Test"]["Test"]["Path"]["stringValue"].as<std::string>().c_str());

    sc["Test"].setCreateIfNotExistentDouble(17.3, "Test", "Path", "doubleValue", NULL);
    EXPECT_DOUBLE_EQ(17.3, sc["Test"]["Test"]["Path"]["doubleValue"].as<double>());

    sc["Test"].setCreateIfNotExistentFloat(19.3f, "Test", "Path", "floatValue", NULL);
    EXPECT_FLOAT_EQ(19.3f, sc["Test"]["Test"]["Path"]["floatValue"].as<float>());

    sc["Test"].setCreateIfNotExistentUShort(7, "Test", "Path", "ushortValue", NULL);
    EXPECT_EQ(7, sc["Test"]["Test"]["Path"]["ushortValue"].as<unsigned short>());

    sc["Test"]["Test"]["Path"]["intValue"] = "14";
    EXPECT_EQ(14, sc["Test"]["Test"]["Path"]["intValue"].as<int>());

    sc["Test"]["Test"]["Path"]["boolValue"] = "true";
    EXPECT_EQ(true, sc["Test"]["Test"]["Path"]["boolValue"].as<bool>());

    sc["Test"]["Test"]["Path"]["stringValue"] = "hello world";
    EXPECT_STREQ("hello world", sc["Test"]["Test"]["Path"]["stringValue"].as<std::string>().c_str());

    sc["Test"]["Test"]["Path"]["doubleValue"] = "17.4";
    EXPECT_DOUBLE_EQ(17.4, sc["Test"]["Test"]["Path"]["doubleValue"].as<double>());

    sc["Test"]["Test"]["Path"]["floatValue"] = "19.4";
    EXPECT_FLOAT_EQ(19.4f, sc["Test"]["Test"]["Path"]["floatValue"].as<float>());

    sc["Test"]["Test"]["Path"]["ushortValue"] = "8";
    EXPECT_EQ(8, sc["Test"]["Test"]["Path"]["ushortValue"].as<unsigned short>());
}

//// Declare a test
//TEST(SystemConfigBasics, readValues)
//{
//    // bring up the SystemConfig with the corresponding path
//    essentials::SystemConfig<essentials::Configuration> sc;
//    sc.setRootPath(".");
//    sc.setConfigPath("./etc");
//
//    // read int
//    unsigned short uShortTestValue = sc["Test"].get<unsigned short>("uShortTestValue");
//    EXPECT_EQ(3, uShortTestValue);
//
//    // read int
//    int intTestValue = sc["Test"].get<int>("intTestValue");
//    EXPECT_EQ(221, intTestValue);
//
//    // read double
//    double doubleTestValue = sc["Test"].get<double>("doubleTestValue");
//    EXPECT_DOUBLE_EQ(0.66234023823, doubleTestValue);
//
//    // read float
//    float floatTestValue = sc["Test"].get<float>("floatTestValue");
//    EXPECT_FLOAT_EQ(1.14f, floatTestValue);
//
//    // read spaceTestValue
//    int spaceTestValue = sc["Test"].get<int>("spaceTestValue");
//    EXPECT_FLOAT_EQ(6, spaceTestValue);
//
//    // read list of strings
//    std::vector<std::string> stringListTestValue = sc["Test"].getList<std::string>("stringListTestValue");
//    std::vector<std::string> referenceStringList{"asdf", "bla", "blub", "hust hust", "möp"};
//    for (int i = 0; i < stringListTestValue.size(); i++) {
//        EXPECT_STREQ(stringListTestValue[i].c_str(), referenceStringList[i].c_str());
//    }
//
//    // read TestSectionValue1
//    std::string testSectionValue1 = sc["Test"].get<std::string>("TestSection.TestSectionValue1");
//    EXPECT_STREQ("TestSectionValue1", testSectionValue1.c_str());
//
//    // read TestSectionValue2
//    float testSectionValue2 = sc["Test"].get<float>("TestSection.TestSectionValue2");
//    EXPECT_FLOAT_EQ(0.66412f, testSectionValue2);
//}
// Run all the tests that were declared with TEST()
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
