#include "audio_toolbox/core/circular_buffer.hpp"

#include "test_utilities.hpp"

using namespace audio_toolbox;

class CircularBufferTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        for (uint32_t i = 0; i < SIZE - 1; i++)
        {
            cb.push(static_cast<real_type>(i));
        }
    }

    void TearDown() override {}

    static constexpr uint32_t SIZE = 8;
    CircularBuffer<real_type> cb{ SIZE };
};

// After SIZE-1 pushes, the buffer should be full.
TEST_F(CircularBufferTest, Constructor)
{
    EXPECT_EQ(cb.size(), SIZE - 1);
    for (uint32_t i = 0; i < SIZE - 1; i++)
    {
        EXPECT_NEAR(cb.pop(), static_cast<real_type>(i), 1e-6);
    }
}

TEST_F(CircularBufferTest, Clear)
{
    cb.clear();
    EXPECT_EQ(cb.size(), 0);
}

TEST_F(CircularBufferTest, Fill)
{
    cb.clear();
    cb.fill(1.0f);
    EXPECT_EQ(cb.size(), SIZE - 1);
    for (uint32_t i = 0; i < SIZE - 1; i++)
    {
        EXPECT_NEAR(cb.pop(), 1.0, 1e-6) << "i = " << i;
    }
}

TEST_F(CircularBufferTest, ClearPushPop)
{
    cb.clear();
    cb.push(1.1);
    EXPECT_EQ(cb.size(), 1);
    EXPECT_NEAR(cb.pop(), 1.1, 1e-6);
}

TEST_F(CircularBufferTest, Indexing)
{
    for (uint32_t i = 0; i < SIZE - 1; i++)
    {
        EXPECT_NEAR(cb[SIZE - 1 - i], static_cast<real_type>(i), 1e-6) << "i = " << i;
    }
}