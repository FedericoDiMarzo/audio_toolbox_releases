#pragma once

/*
 * Credits to
 * https://signalsmith-audio.co.uk/writing/2021/lets-write-a-reverb/
 * https://github.com/Signalsmith-Audio/reverb-example-code
 */

#include <array>
#include <cstdint>
#include <memory>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/math/matrix.hpp"
#include "audio_toolbox/math/random.hpp"
#include "audio_toolbox/processors/base_processor.hpp"
#include "audio_toolbox/processors/delay/fractional_delay.hpp"
#include "audio_toolbox/static/macros.hpp"
#include "audio_toolbox/static/templates_utilities.hpp"

namespace audio_toolbox
{

/**
 * @brief Combines a multichannel delay and mixer into a diffusion block.
 */
class DiffusionStep : public BaseProcessor<DiffusionStep>
{
    std::vector<std::unique_ptr<FractionalDelay>> delay_lines;

  public:
    AT_DISABLE_COPY_AND_MOVE_M(DiffusionStep);

    /**
     * @param delay_lines A list of fractional delay lines. The number of delay
     * lines determines the number of input and output channels.
     * @param min_delay Minimum delay in samples for randomization.
     * @param max_delay Maximum delay in samples for randomization.
     */
    DiffusionStep(std::initializer_list<std::unique_ptr<FractionalDelay>> delay_lines,
                  real_type min_delay = 0.0f,
                  real_type max_delay = 300.0f)
        : BaseProcessor(delay_lines.size(), delay_lines.size())
    {
        for (auto& dl : delay_lines)
        {
            this->delay_lines.push_back(std::move(dl));
            this->delay_lines.back()->set_random_range(min_delay, max_delay);
            this->delay_lines.back()->randomize();
        }
    }

    inline void reset() noexcept
    {
        for (auto& dl : delay_lines)
        {
            dl->reset();
        }
    }

    inline void process_sample(const real_type* input, real_type* output) noexcept
    {
        for (size_t i = 0; i < delay_lines.size(); i++)
        {
            output[i] = delay_lines[i]->process_mono_sample(input[i]);
        }
    }
};

} // namespace audio_toolbox

#include "audio_toolbox/processors/diffusion_blocks_imp.hpp"
