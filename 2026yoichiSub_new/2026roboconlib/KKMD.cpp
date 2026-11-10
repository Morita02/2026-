#include "KKMD.hpp"

namespace rob {
	float KKMD::set_power(float power) {
		const float max = 1.0;
		if(isReverse)power*=(-1.0f);
		
		if(power < -max){
			power = -max;
		}
		else if(power > max){
			power = max;
		}
		if(power < 0.0){
			power *= -1.0;
			pwm1 = power;
			pwm2 = 0.0;
		}
		else if(power == 0.0){
			pwm1 = 0.0;
			pwm2 = 1.0;
		}else{
			pwm1 = power;
			pwm2 = 1.0;
		}
		return power;
	}
	
	uint32_t KKMD::freq(const uint32_t f) {
		const int t=1000000/f;
		pwm1.period_us(t);
		pwm2.period_us(t);
		return f;
	}
	
}