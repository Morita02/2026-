#include "aRohmMD.hpp"

namespace rob{

float aRohmMD::set(float v){
	const float max=1.0;
	if(v<-max){
		v=-max;
	}else if(v>max){
		v=max;
	}
	
	if(v==0.0 && isBrakeWhenZero){
		out2=1;
		out3=1;
		pwm1=0.0;
	}else if(v<0.0){
		out2=1;
		out3=0;
		pwm1=-v;
	}else{
		out2=0;
		out3=1;
		pwm1=v;
	}
	return v;
}

uint32_t aRohmMD::freq(const uint32_t f){
	const int t=1000000/f;
	pwm1.period_us(t);
	return f;
}

}//namespace rob