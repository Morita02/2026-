#include "2026roboconlib_setting.hpp"
#include "control_config.hpp"

namespace rob {
HardwareEncoder Encoder1(TIM8, cfg::ARM_ENCODER_SIGN, cfg::ARM_PPR);
HardwareEncoder Encoder2(TIM1, cfg::DRUM_ENCODER_SIGN, cfg::DRUM_PPR);
}
