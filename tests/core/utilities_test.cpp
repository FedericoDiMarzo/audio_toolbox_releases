#include <gtest/gtest.h>

#include "test_utilities.hpp"

using namespace audio_toolbox;

class Updatable
{
  public:
    Updatable() = default;

    void update()
    {
        updated = true;
    };

    bool updated{ false };
};

using UpdatableDispatcher = UpdateDispatcher<Updatable>;

class UtilitiesTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        set_random_seed(0);
        for (auto& x : updatables)
        {
            dispatcher.add_observer(x);
        }
    }

    void TearDown() override {}

    std::vector<int> container{ 1, 50, -20, 8 };
    std::vector<Updatable> updatables{ Updatable(), Updatable(), Updatable() };
    UpdateDispatcher<Updatable> dispatcher;
};

TEST_F(UtilitiesTest, Contains)
{
    EXPECT_TRUE(contains(container, 1));
    EXPECT_TRUE(contains(container, 50));
    EXPECT_TRUE(contains(container, -20));
    EXPECT_TRUE(contains(container, 8));
    EXPECT_FALSE(contains(container, 0));
}

TEST_F(UtilitiesTest, IndexOf)
{
    EXPECT_EQ(index_of(container, 1), 0);
    EXPECT_EQ(index_of(container, 50), 1);
    EXPECT_EQ(index_of(container, -20), 2);
    EXPECT_EQ(index_of(container, 8), 3);
}

TEST_F(UtilitiesTest, LinearSpace)
{
    auto x = linear_space(-20.02f, 100.0f, 2);
    EXPECT_EQ(x.size(), 2);
    EXPECT_NEAR(x[0], -20.02f, 1e-6);
    EXPECT_NEAR(x[1], 100.0f, 1e-6);
}

TEST_F(UtilitiesTest, RemoveItem)
{
    remove_item(container, 1);
    EXPECT_EQ(container, std::vector<int>({ 50, -20, 8 }));
}

TEST_F(UtilitiesTest, UpdateDispatcher)
{
    bool none_updated =
        std::all_of(std::begin(updatables), std::end(updatables), [](const Updatable& x) { return !x.updated; });
    EXPECT_TRUE(none_updated);
    dispatcher.update();
    bool all_updated =
        std::all_of(std::begin(updatables), std::end(updatables), [](const Updatable& x) { return x.updated; });
    EXPECT_TRUE(all_updated);
}
