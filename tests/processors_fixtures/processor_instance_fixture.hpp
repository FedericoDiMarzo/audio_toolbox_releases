#pragma once
#include "audio_toolbox/core/types.hpp"
#include <functional>
#include <string>
#include <vector>

namespace audio_toolbox::test
{

/**
 * @brief Defines a specific processor instance with a given parametrization and name.
 * The factory function is used to create an instance of the processor in the test cases.
 *
 * To create a fixture for a specific processor, you should define the following method
 * inside the processor fixture class/struct:
 *
 * std::vector<ProcessorConfig<YourProcessor>> create_configurations() const;
 *
 * The method should return a vector of ProcessorConfig objects, each containing a name and
 * a factory function for creating an instance of the processor with specific parameters.
 * The processor will be created lazily at test runtime.
 *
 * @tparam PROCESSOR Template parameter for the processor type.
 */
template <typename PROCESSOR>
struct ProcessorInstanceFixture
{
    std::string name;
    std::function<PROCESSOR()> factory;
};

} // namespace audio_toolbox::test