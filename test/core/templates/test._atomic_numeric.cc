#include <gtest/gtest.h>

#include "core/templates/atomic_numeric.h"

using namespace ho;

TEST(AtomicNumericTest, Constructor) {
    AtomicNumeric<int> num(10);
    EXPECT_EQ(num.Get(), 10);
}

TEST(AtomicNumericTest, SetAndGet) {
    AtomicNumeric<int> num(10);
    EXPECT_EQ(num.Get(), 10);

    num.Set(20);
    EXPECT_EQ(num.Get(), 20);
}

TEST(AtomicNumericTest, IncrementAndDecrement) {
    AtomicNumeric<int> num(0);

    EXPECT_EQ(num.Increment(), 1);
    EXPECT_EQ(num.Get(), 1);

    EXPECT_EQ(num.Increment(), 2);
    EXPECT_EQ(num.Get(), 2);

    EXPECT_EQ(num.Decrement(), 1);
    EXPECT_EQ(num.Get(), 1);
}

TEST(AtomicNumericTest, AddAndSub) {
    AtomicNumeric<int> num(10);

    EXPECT_EQ(num.Add(5), 15);
    EXPECT_EQ(num.Get(), 15);

    EXPECT_EQ(num.Sub(3), 12);
    EXPECT_EQ(num.Get(), 12);
}

TEST(AtomicNumericTest, ExchangeIfGreater_NoExchange) {
    AtomicNumeric<int> num(50);

    int result = num.ExchangeIfGreater(20);

    EXPECT_EQ(result, 50);
    EXPECT_EQ(num.Get(), 50);
}

TEST(AtomicNumericTest, ExchangeIfGreater_ExchangeOccurs) {
    AtomicNumeric<int> num(10);

    int result = num.ExchangeIfGreater(99);

    EXPECT_EQ(result, 99);
    EXPECT_EQ(num.Get(), 99);
}

TEST(AtomicNumericTest, ExchangeIfGreater_MultipleCASAttempts) {
    AtomicNumeric<int> num(5);

    EXPECT_EQ(num.ExchangeIfGreater(10), 10);
    EXPECT_EQ(num.Get(), 10);

    EXPECT_EQ(num.ExchangeIfGreater(10), 10);
    EXPECT_EQ(num.Get(), 10);
}

TEST(AtomicNumericTest, TestEqual) {
    AtomicNumeric<int> num(42);

    EXPECT_TRUE(num.TestEqual(42));
    EXPECT_FALSE(num.TestEqual(41));
}

TEST(AtomicNumericTest, TestNotEqual) {
    AtomicNumeric<int> num(42);

    EXPECT_TRUE(num.TestNotEqual(40));
    EXPECT_FALSE(num.TestNotEqual(42));
}

TEST(AtomicNumericTest, TestLess) {
    AtomicNumeric<int> num(50);

    EXPECT_TRUE(num.TestLess(40));
    EXPECT_FALSE(num.TestLess(50));
    EXPECT_FALSE(num.TestLess(60));
}

TEST(AtomicNumericTest, TestGreater) {
    AtomicNumeric<int> num(50);

    EXPECT_TRUE(num.TestGreater(60));
    EXPECT_FALSE(num.TestGreater(50));
    EXPECT_FALSE(num.TestGreater(40));
}

TEST(AtomicNumericTest, TestLessEqual) {
    AtomicNumeric<int> num(50);

    EXPECT_TRUE(num.TestLessEqual(50));
    EXPECT_TRUE(num.TestLessEqual(40));
    EXPECT_FALSE(num.TestLessEqual(60));
}

TEST(AtomicNumericTest, TestGreaterEqual) {
    AtomicNumeric<int> num(50);

    EXPECT_TRUE(num.TestGreaterEqual(50));
    EXPECT_TRUE(num.TestGreaterEqual(60));
    EXPECT_FALSE(num.TestGreaterEqual(40));
}
