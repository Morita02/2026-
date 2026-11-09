#pragma once
#include "mbed.h"
// Both boards use the modified PCB. ALT names select timers in this SDK.
#define MD1_PINS PB_4, PC_8
#define MD2_PINS PC_9, PB_8
//#define MD3_PINS PB_3, PA_6_ALT0
#define MD3_PINS PB_3, PA_10
#define MD4_PINS PB_10, PB_5
#define MD5_PINS PA_7_ALT2, PB_2
// MD1/MD2/MD4: TIM3 and TIM2. MD3 second pin: TIM13.
// MD5 first pin: TIM14. TIM1 and TIM8 are reserved for encoders.
