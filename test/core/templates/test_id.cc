#include <gtest/gtest.h>

#include <unordered_map>

#include "core/templates/id.h"

using namespace ho;

// Dummy
struct Test {};
using TestID = ID<Test>;

TEST(IDTest, Constructor) {
    TestID id;

    EXPECT_TRUE(id.IsNULL());
    EXPECT_FALSE(id.IsValid());
    EXPECT_EQ(id.id(), 0ull);
}

TEST(IDTest, StaticConstant) {
    EXPECT_TRUE(TestID::NULL_ID.IsNULL());
    EXPECT_EQ(TestID::NULL_ID.id(), 0ull);
}

TEST(IDTest, Increment) {
    TestID id;

    id.Increment();
    EXPECT_EQ(id.id(), 1ull);

    id.Increment().Increment().Increment();
    EXPECT_EQ(id.id(), 4ull);

    EXPECT_TRUE(id.IsValid());
}

TEST(IDTest, Decrement) {
    TestID id;
    id.Increment().Increment().Increment().Increment();

    id.Decrement();
    EXPECT_EQ(id.id(), 3ull);

    id.Decrement().Decrement().Decrement();
    EXPECT_EQ(id.id(), 0ull);
}

#ifdef DEBUG
TEST(IDTest, DecrementOverflow) {
    TestID id;
    EXPECT_DEATH(id.Decrement(), "");
}
#endif
TEST(IDTest, ComparisonOperators) {
    TestID a;
    TestID b;

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);

    a.Increment();
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);

    b.Increment();
    EXPECT_TRUE(a == b);
}

TEST(IDTest, HashWorkCheck) {
    std::unordered_map<TestID, int> map;

    TestID a;
    TestID b;

    a.Increment();
    b.Increment().Increment();

    map[a] = 10;
    map[b] = 20;

    EXPECT_EQ(map[a], 10);
    EXPECT_EQ(map[b], 20);

    // Ensure separate keys
    EXPECT_NE(map[a], map[b]);
}
