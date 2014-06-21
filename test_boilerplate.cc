#include <vector>
#include <queue>

#include <gtest/gtest.h>
#include <glog/logging.h>

TEST(GTestTest, Trivial) { 
    EXPECT_EQ(42, 42);
    ASSERT_TRUE(42 == 42);
}

TEST(GTestTest, Passing) { 
    EXPECT_EQ(4, 2 + 2);
    EXPECT_GT(2 * 2, 3);
    EXPECT_GE(2 * 2, 4);
    EXPECT_LE(2 * 2, 4);
    EXPECT_LT(2 * 2, 5);
    EXPECT_NE(2 * 2, 0);
    EXPECT_TRUE(2 * 2 == 4);
    EXPECT_FALSE(2 * 2 != 4);
}

TEST(GTestTest, DeathTest) {
    auto die = []() {
        const int tmp = 42;
        LOG(FATAL) << "Example " << tmp << " error message.";
    };
    ASSERT_DEATH(die(), "Example .* message\\.");
}

template<typename T> class GTestTemplatedTest : public ::testing::Test {};

typedef ::testing::Types<std::vector<int>, std::deque<int>> ContainersTypeList;
TYPED_TEST_CASE(GTestTemplatedTest, ContainersTypeList);

TYPED_TEST(GTestTemplatedTest, TemplatedContainersTest) {
    TypeParam container;
    EXPECT_TRUE(container.empty());
    EXPECT_EQ(0, container.size());
    container.push_back(42);
    EXPECT_FALSE(container.empty());
    EXPECT_EQ(1, container.size());
    EXPECT_EQ(42, container.back());
    container.pop_back();
    EXPECT_TRUE(container.empty());
    EXPECT_EQ(0, container.size());
}
