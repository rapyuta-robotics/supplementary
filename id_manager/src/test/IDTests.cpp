#include <essentials/Identifier.h>
#include <essentials/IdentifierConstPtr.h>
#include <essentials/IDManager.h>
#include <essentials/WildcardID.h>

#include <gtest/gtest.h>
#include <vector>
#include <chrono>
#include <random>

TEST(Identifier, MeasureComparison)
{
    essentials::IDManager* idManager = new essentials::IDManager();
    int iterations = 100000;

    std::chrono::nanoseconds totaltime = std::chrono::nanoseconds(0);
    auto id1 = idManager->generateID();
    auto id2 = idManager->generateID();
    for (int i = 0; i < iterations; i++) {
        auto begin = std::chrono::high_resolution_clock::now();
        bool equal = id1 == id2;
        auto end = std::chrono::high_resolution_clock::now();
        totaltime += std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
    }
    std::cout << "[Identifier.MeasureComparison] Comparing 16Byte Identifier Pointers takes " << (totaltime / iterations).count() << " Nanoseconds!" << std::endl;

    totaltime = std::chrono::nanoseconds(0);
    id1 = idManager->generateID();
    id2 = idManager->generateID();
    for (int i = 0; i < iterations; i++) {
        auto begin = std::chrono::high_resolution_clock::now();
        bool equal = *id1 == *id2;
        auto end = std::chrono::high_resolution_clock::now();
        totaltime += std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
    }
    std::cout << "[Identifier.MeasureComparison] Comparing 16Byte Identifier takes " << (totaltime / iterations).count() << " Nanoseconds!" << std::endl;

    totaltime = std::chrono::nanoseconds(0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long long> dis(
            std::numeric_limits<std::uint64_t>::min(),
            std::numeric_limits<std::uint64_t>::max()
    );
    uint64_t idInt1 = dis(gen);
    uint64_t idInt2 = dis(gen);
    for (int i = 0; i < iterations; i++) {
        auto begin = std::chrono::high_resolution_clock::now();
        bool equal = idInt1 == idInt2;
        auto end = std::chrono::high_resolution_clock::now();
        totaltime += std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
    }
    std::cout << "[Identifier.MeasureComparison] Comparing uint64_t's takes " << (totaltime / iterations).count() << " Nanoseconds!" << std::endl;
}

TEST(Identifier, MeasureCopying)
{
    essentials::IDManager* idManager = new essentials::IDManager();
    int iterations = 100000;

    std::chrono::nanoseconds totaltime = std::chrono::nanoseconds(0);
    auto id1 = idManager->generateID();
    for (int i = 0; i < iterations; i++) {
        auto begin = std::chrono::high_resolution_clock::now();
        essentials::Identifier id2 = id1->toByteVector();
        auto end = std::chrono::high_resolution_clock::now();
        totaltime += std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
    }
    std::cout << "[Identifier.MeasureCopying] Identifier assignment constructor takes " << (totaltime / iterations).count() << " Nanoseconds!" << std::endl;

    totaltime = std::chrono::nanoseconds(0);
    id1 = idManager->generateID();
    for (int i = 0; i < iterations; i++) {
        auto begin = std::chrono::high_resolution_clock::now();
        const essentials::Identifier* id2 = id1;
        auto end = std::chrono::high_resolution_clock::now();
        totaltime += std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
    }
    std::cout << "[Identifier.MeasureCopying] Copying const Identifier* takes " << (totaltime / iterations).count() << " Nanoseconds!" << std::endl;

    totaltime = std::chrono::nanoseconds(0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long long> dis(
            std::numeric_limits<std::uint64_t>::min(),
            std::numeric_limits<std::uint64_t>::max()
    );
    uint64_t id1Int = dis(gen);
    for (int i = 0; i < iterations; i++) {

        auto begin = std::chrono::high_resolution_clock::now();
        uint64_t id2Int = id1Int;
        auto end = std::chrono::high_resolution_clock::now();
        totaltime += std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
    }
    std::cout << "[Identifier.MeasureComparison] Copying uint64_t's takes " << (totaltime / iterations).count() << " Nanoseconds!" << std::endl;
}

TEST(Identifier, ConstructorCopiesBytes)
{
    std::vector<uint8_t> bytes1;
    for (int i = 0; i < 20; i++) {
        bytes1.push_back(i);
    }
    essentials::Identifier* id1 = new essentials::Identifier(bytes1.data(), bytes1.size());

    ASSERT_FALSE(id1->getRaw() == bytes1.data());

    delete id1;
}

TEST(Identifier, ConstructionOfHugeID)
{
    std::vector<uint8_t> bytes1;
    int size = 100;
    for (int i = 0; i < size; i++) {
        bytes1.push_back(i);
    }
    essentials::Identifier* id1 = new essentials::Identifier(bytes1.data(), bytes1.size());

    ASSERT_EQ(id1->getSize(), size);

    delete id1;
}

TEST(Identifier, ToByteVectorReturnsCopy)
{
    std::vector<uint8_t> bytes1;
    int size = 100;
    for (int i = 0; i < size; i++) {
        bytes1.push_back(i);
    }
    essentials::Identifier* id1 = new essentials::Identifier(bytes1.data(), bytes1.size());
    auto byteVector = id1->toByteVector();

    ASSERT_NE(byteVector.data(), id1->getRaw());

    delete id1;
}

TEST(Identifier, HashEqualForSameIDs)
{
    std::vector<uint8_t> bytes1;
    std::vector<uint8_t> bytes2;
    for (int i = 0; i < 20; i++) {
        bytes1.push_back(i);
        bytes2.push_back(i);
    }
    essentials::Identifier* id1 = new essentials::Identifier(bytes1.data(), bytes1.size());
    essentials::Identifier* id2 = new essentials::Identifier(bytes2.data(), bytes2.size());

    ASSERT_EQ(id1->hash(), id2->hash());

    delete id1;
    delete id2;
}

TEST(Identifier, EqualWithSameID)
{
    std::vector<uint8_t> bytes1;
    std::vector<uint8_t> bytes2;
    for (int i = 0; i < 20; i++) {
        bytes1.push_back(i);
        bytes2.push_back(i);
    }
    essentials::Identifier* id1 = new essentials::Identifier(bytes1.data(), bytes1.size());
    essentials::Identifier* id2 = new essentials::Identifier(bytes2.data(), bytes2.size());

    ASSERT_TRUE(*id1 == *id2);

    delete id1;
    delete id2;
}

TEST(Identifier, NotEqualWithDifferentID)
{
    std::vector<uint8_t> bytes1;
    std::vector<uint8_t> bytes2;
    for (int i = 0; i < 20; i++) {
        bytes1.push_back(i);
        bytes2.push_back(i);
    }
    bytes2.push_back(2);
    essentials::Identifier* id1 = new essentials::Identifier(bytes1.data(), bytes1.size());
    essentials::Identifier* id2 = new essentials::Identifier(bytes2.data(), bytes2.size());

    ASSERT_FALSE(*id1 == *id2);

    delete id1;
    delete id2;
}

TEST(BroadCastID, NotEqualWithNormalID)
{
    std::vector<uint8_t> bytes1;
    for (int i = 0; i < 20; i++) {
        bytes1.push_back(i);
    }
    essentials::Identifier* normalID = new essentials::Identifier(bytes1.data(), bytes1.size());
    essentials::IDManager idManager;
    essentials::IdentifierConstPtr broadcastID = idManager.getWildcardID();

    ASSERT_FALSE(*broadcastID == *normalID);

    delete normalID;
}

TEST(BroadCastID, EqualWithBroadcastID)
{
    essentials::IDManager idManager1;
    essentials::IdentifierConstPtr broadcastID1 = idManager1.getWildcardID();
    essentials::IDManager idManager2;
    essentials::IdentifierConstPtr broadcastID2 = idManager2.getWildcardID();
    ASSERT_TRUE(*broadcastID1 == *broadcastID2);
}

TEST(IdentifierFactory, GenerateIDsOfVariousLength)
{
    essentials::IDManager factory;

    auto id1 = factory.generateID(1);
    ASSERT_EQ(id1->getSize(), 1);

    auto id4 = factory.generateID(4);
    ASSERT_EQ(id4->getSize(), 4);

    auto id15 = factory.generateID(15);
    ASSERT_EQ(id15->getSize(), 15);

    auto id18 = factory.generateID(18);
    ASSERT_EQ(id18->getSize(), 18);
}

TEST(IdentifierFactory, DuplicateIDs)
{
    essentials::IDManager idManager;
    auto id18 = idManager.generateID(18);
    auto id18Copy = idManager.getIDFromBytes(id18->toByteVector().data(), id18->toByteVector().size());
    ASSERT_TRUE(*id18 == *id18Copy);
}

TEST(IdentifierManager, CreateIDsFromIntegralTypes)
{
    essentials::IDManager* idManager = new essentials::IDManager();
    int idInt = 5;
    auto intId5 = idManager->getID<int>(idInt);

    std::vector<uint8_t> idBytes;
    idBytes.push_back(5);
    idBytes.push_back(0);
    idBytes.push_back(0);
    idBytes.push_back(0);
    essentials::Identifier* referenceId5 = new essentials::Identifier(idBytes.data(), idBytes.size());

    ASSERT_TRUE(*intId5 == *referenceId5);
}

TEST(IdentifierManager, GuarenteeSingleEntities)
{
    essentials::IDManager* idManager = new essentials::IDManager();
    int idInt = 5;
    auto intId1 = idManager->getID<int>(idInt);
    auto intId2 = idManager->getID<int>(idInt);

    ASSERT_EQ(intId1, intId2);
}

TEST(IdentifierManager, GenerateIDsOfVariousLength)
{
    essentials::IDManager* idManager = new essentials::IDManager();

    auto id1 = idManager->generateID(1);
    ASSERT_EQ(id1->getSize(), 1);

    auto id4 = idManager->generateID(4);
    ASSERT_EQ(id4->getSize(), 4);

    auto id15 = idManager->generateID(15);
    ASSERT_EQ(id15->getSize(), 15);

    auto id18 = idManager->generateID(18);
    ASSERT_EQ(id18->getSize(), 18);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
