#pragma once
#ifndef KKMD_HPP_INCLUDE_GUARD
#define KKMD_HPP_INCLUDE_GUARD

#include "mbed.h"

namespace rob {
	
class KKMD {
private:
	PwmOut pwm1, pwm2;
	bool isReverse;
public:
	KKMD(PinName pin1, PinName pin2, bool isReverseArg):
	pwm1(pin1), pwm2(pin2), isReverse(isReverseArg){
		freq(2000);
	}
	
	float set_power(float power);
	
	float operator=(const float power) {
		return set_power(power);
	}
	
	uint32_t freq(const uint32_t f);
};
	
}

#endif