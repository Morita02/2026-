#pragma once
#include "mbed.h"
#include "2026roboconlib/KKMD.hpp"
#include "2026roboconlib/useful.hpp"
//#include "board_pins.hpp"
#include "hardware_encoder.hpp"

#define MD1_PINS PB_4, PC_8
#define MD2_PINS PC_9, PB_8
#define MD3_PINS PB_3, PA_6_ALT0
#define MD4_PINS PB_10, PB_5
#define MD5_PINS PA_7_ALT2, PB_2

extern Serial pc;
namespace rob {
extern HardwareEncoder Encoder1;
extern HardwareEncoder Encoder2;
}
