#include <gtest/gtest.h>

#include <string>

#include "core/templates/id.h"
#include "core/templates/id_owner.h"

using namespace ho;

struct TestValue {
  int a;
  std::string b;

  TestValue(int p_a, std::string p_b) : a(p_a), b(std::move(p_b)) {}
};

// Dummy
struct Test {};
using TestID = ID<Test>;
using TestOwner = ID_Owner<TestID, TestValue>;

TEST(IDOwnerTest, Constructor) {
  TestOwner owner;

  EXPECT_EQ(owner.Size(), 0);
}

TEST(IDOwnerTest, EnrollCopy) {
  TestOwner owner;

  TestValue v1(10, "first");
  TestValue v2(20, "second");
  TestValue v3(30, "third");

  TestID id1 = owner.Enroll(v1);
  TestID id2 = owner.Enroll(v2);
  TestID id3 = owner.Enroll(v3);

  EXPECT_TRUE(id1.IsValid());
  EXPECT_TRUE(id2.IsValid());
  EXPECT_TRUE(id3.IsValid());
  EXPECT_EQ(owner.Size(), 3);

  TestValue* p1 = owner.Get(id1);
  TestValue* p2 = owner.Get(id2);
  TestValue* p3 = owner.Get(id3);

  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);
  ASSERT_NE(p3, nullptr);

  EXPECT_EQ(p1->a, 10);
  EXPECT_EQ(p1->b, "first");

  EXPECT_EQ(p2->a, 20);
  EXPECT_EQ(p2->b, "second");

  EXPECT_EQ(p3->a, 30);
  EXPECT_EQ(p3->b, "third");

  // IDs must be sequential
  EXPECT_EQ(id2.id(), id1.id() + 1);
  EXPECT_EQ(id3.id(), id2.id() + 1);
}

TEST(IDOwnerTest, EnrollCopySameValue) {
  TestOwner owner;

  TestValue v1(10, "same");
  TestValue v2(10, "same");

  TestID id1 = owner.Enroll(v1);
  TestID id2 = owner.Enroll(v2);

  EXPECT_TRUE(id1.IsValid());
  EXPECT_TRUE(id2.IsValid());
  EXPECT_EQ(owner.Size(), 2);

  TestValue* p1 = owner.Get(id1);
  TestValue* p2 = owner.Get(id2);

  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);

  EXPECT_EQ(p1->a, 10);
  EXPECT_EQ(p1->b, "same");

  EXPECT_EQ(p2->a, 10);
  EXPECT_EQ(p2->b, "same");

  EXPECT_EQ(id2.id(), id1.id() + 1);
}

TEST(IDOwnerTest, EnrollMove) {
  TestOwner owner;

  TestValue v1(100, "first");
  TestValue v2(200, "second");
  TestValue v3(300, "third");

  TestID id1 = owner.Enroll(std::move(v1));
  TestID id2 = owner.Enroll(std::move(v2));
  TestID id3 = owner.Enroll(std::move(v3));

  EXPECT_TRUE(id1.IsValid());
  EXPECT_TRUE(id2.IsValid());
  EXPECT_TRUE(id3.IsValid());
  EXPECT_EQ(owner.Size(), 3);

  TestValue* p1 = owner.Get(id1);
  TestValue* p2 = owner.Get(id2);
  TestValue* p3 = owner.Get(id3);

  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);
  ASSERT_NE(p3, nullptr);

  EXPECT_EQ(p1->a, 100);
  EXPECT_EQ(p1->b, "first");

  EXPECT_EQ(p2->a, 200);
  EXPECT_EQ(p2->b, "second");

  EXPECT_EQ(p3->a, 300);
  EXPECT_EQ(p3->b, "third");

  // IDs must be sequential
  EXPECT_EQ(id2.id(), id1.id() + 1);
  EXPECT_EQ(id3.id(), id2.id() + 1);
}

TEST(IDOwnerTest, EnrollMoveSameValue) {
  TestOwner owner;

  TestValue v1(20, "same");
  TestValue v2(20, "same");

  TestID id1 = owner.Enroll(std::move(v1));
  TestID id2 = owner.Enroll(std::move(v2));

  EXPECT_TRUE(id1.IsValid());
  EXPECT_TRUE(id2.IsValid());
  EXPECT_EQ(owner.Size(), 2);

  TestValue* p1 = owner.Get(id1);
  TestValue* p2 = owner.Get(id2);

  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);

  EXPECT_EQ(p1->a, 20);
  EXPECT_EQ(p1->b, "same");

  EXPECT_EQ(p2->a, 20);
  EXPECT_EQ(p2->b, "same");

  EXPECT_EQ(id2.id(), id1.id() + 1);
}

TEST(IDOwnerTest, Emplace) {
  TestOwner owner;

  TestID id1 = owner.Emplace(101, "first");
  TestID id2 = owner.Emplace(202, "second");
  TestID id3 = owner.Emplace(303, "third");

  EXPECT_TRUE(id1.IsValid());
  EXPECT_TRUE(id2.IsValid());
  EXPECT_TRUE(id3.IsValid());

  EXPECT_EQ(owner.Size(), 3);

  const TestValue* p1 = owner.Get(id1);
  const TestValue* p2 = owner.Get(id2);
  const TestValue* p3 = owner.Get(id3);

  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);
  ASSERT_NE(p3, nullptr);

  EXPECT_EQ(p1->a, 101);
  EXPECT_EQ(p1->b, "first");

  EXPECT_EQ(p2->a, 202);
  EXPECT_EQ(p2->b, "second");

  EXPECT_EQ(p3->a, 303);
  EXPECT_EQ(p3->b, "third");

  // IDs must be sequential
  EXPECT_EQ(id2.id(), id1.id() + 1);
  EXPECT_EQ(id3.id(), id2.id() + 1);
}

TEST(IDOwnerTest, EmplaceSameValue) {
  TestOwner owner;

  TestID id1 = owner.Emplace(77, "same");
  TestID id2 = owner.Emplace(77, "same");

  EXPECT_TRUE(id1.IsValid());
  EXPECT_TRUE(id2.IsValid());
  EXPECT_EQ(owner.Size(), 2);

  const TestValue* p1 = owner.Get(id1);
  const TestValue* p2 = owner.Get(id2);

  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);

  EXPECT_EQ(p1->a, 77);
  EXPECT_EQ(p1->b, "same");

  EXPECT_EQ(p2->a, 77);
  EXPECT_EQ(p2->b, "same");

  EXPECT_EQ(id2.id(), id1.id() + 1);
}

TEST(IDOwnerTest, Has) {
  TestOwner owner;
  EXPECT_FALSE(owner.Has(TestID()));

  TestID id1 = owner.Emplace(101, "first");
  TestID id2 = owner.Emplace(202, "second");
  TestID id3 = owner.Emplace(303, "third");

  EXPECT_TRUE(owner.Has(id1));
  EXPECT_TRUE(owner.Has(id2));
  EXPECT_TRUE(owner.Has(id3));
}

TEST(IDOwnerTest, Get) {
  TestOwner owner;
  EXPECT_EQ(owner.Get(TestID()), nullptr);

  TestID id1 = owner.Emplace(101, "first");
  TestID id2 = owner.Emplace(202, "second");
  TestID id3 = owner.Emplace(303, "third");

  EXPECT_NE(owner.Get(id1), nullptr);
  EXPECT_NE(owner.Get(id2), nullptr);
  EXPECT_NE(owner.Get(id3), nullptr);
}

TEST(IDOwnerTest, Delete) {
  TestOwner owner;
  EXPECT_FALSE(owner.Delete(TestID()));

  TestID id1 = owner.Emplace(101, "first");
  TestID id2 = owner.Emplace(202, "second");
  TestID id3 = owner.Emplace(303, "third");

  EXPECT_TRUE(owner.Delete(id1));
  EXPECT_EQ(owner.Size(), 2);
  EXPECT_EQ(owner.Get(id1), nullptr);

  EXPECT_TRUE(owner.Delete(id2));
  EXPECT_EQ(owner.Size(), 1);
  EXPECT_EQ(owner.Get(id2), nullptr);

  EXPECT_TRUE(owner.Delete(id3));
  EXPECT_EQ(owner.Size(), 0);
  EXPECT_EQ(owner.Get(id3), nullptr);
}

TEST(IDOwnerTest, Clear) {
  TestOwner owner;

  TestID id1 = owner.Emplace(101, "first");
  TestID id2 = owner.Emplace(202, "second");
  TestID id3 = owner.Emplace(303, "third");

  EXPECT_EQ(owner.Size(), 3);

  owner.Clear();
  EXPECT_EQ(owner.Size(), 0);

  EXPECT_FALSE(owner.Has(id1));
  EXPECT_FALSE(owner.Has(id2));
  EXPECT_FALSE(owner.Has(id3));
  EXPECT_EQ(owner.Get(id1), nullptr);
  EXPECT_EQ(owner.Get(id2), nullptr);
  EXPECT_EQ(owner.Get(id3), nullptr);
}
