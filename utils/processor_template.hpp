#ifndef _AUDIO_TOOLBOX_TEMPLATE_PROCESSOR_H_
#define _AUDIO_TOOLBOX_TEMPLATE_PROCESSOR_H_

/*
  Intro comments.

  Possible paper citations.
 */

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/core/utilities.hpp"
#include "audio_toolbox/processors/base_processor.hpp"

namespace audio_toolbox {

/*
  We usually prefer static instead of dynamic initialization.
  Check out from the inc/ directory for more examples.
*/
template <uint32_t SAMPLE_RATE>
class TemplateProcessor : public MonoProcessor<TemplateProcessor> {
  real_type param;

 public:
  /*
    By default we prefer to disable copy and move operations.
    If you need to collect the processor in a container, you can
    exploit static initialization
  */
  AT_DISABLE_COPY_AND_MOVE_M(TemplateProcessor);

  TemplateProcessor(real_type param) : param(param) { reset(); };

  // Only for stateful processors
  void reset() noexcept;

  real_type process_sample(real_type input) noexcept;
};

}  // namespace audio_toolbox

#endif  // _AUDIO_TOOLBOX_TEMPLATE_PROCESSOR_H_