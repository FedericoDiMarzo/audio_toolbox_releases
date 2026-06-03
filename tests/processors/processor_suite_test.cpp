#include <gtest/gtest.h>

#include "processors/processor_suite_test.hpp"

// Add one include per processor fixture
#include "processors_fixtures/basic_chorus_fixture.hpp"
#include "processors_fixtures/biquad_fixture.hpp"
#include "processors_fixtures/bitcrush_fixture.hpp"
#include "processors_fixtures/fractional_delay_fixture.hpp"
#include "processors_fixtures/integer_delay_fixture.hpp"
#include "processors_fixtures/paulstretch_fixture.hpp"
#include "processors_fixtures/virtual_analog_filter_ladder_fixture.hpp"
#include "processors_fixtures/virtual_analog_filter_one_pole_fixture.hpp"
#include "processors_fixtures/wavetable_oscillator_fixture.hpp"

using namespace audio_toolbox::test;

// Add one entry per fixture inside the samplate
using ProcessorTypes = ::testing::Types<BiquadFixture,
                                        BitCrushFixture,
                                        PaulStretchFixture,
                                        BasicChorusFixture,
                                        IntegerDelayFixture,
                                        FractionalDelayFixture,
                                        WavetableOscillatorFixture,
                                        VirtualAnalogFilter1PFixture,
                                        VirtualAnalogFilterLadderFixture>;

INSTANTIATE_TYPED_TEST_SUITE_P(Processors, ProcessorSuiteTest, ProcessorTypes);

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}