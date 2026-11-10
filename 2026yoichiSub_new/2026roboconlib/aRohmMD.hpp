#ifndef A_ROHM_MD_HPP_INCLUDE_GUARD
#define A_ROHM_MD_HPP_INCLUDE_GUARD
#include "mbed.h"

namespace rob{
	
//後でインターフェースを書こう！！


class aRohmMD{
	private:
	PwmOut pwm1;
	DigitalOut out2,out3;
	bool isBrakeWhenZero;
	
	//pwmのチャンネルをまたいでいるので勝手に変更した場合の動作を保証できないためprivate
	uint32_t freq(const uint32_t f);
	
	public:
	aRohmMD(const PinName pinPwm1,const PinName pinOut2,const PinName pinOut3):
		pwm1(pinPwm1),out2(pinOut2),out3(pinOut3),isBrakeWhenZero(true)
	{
		freq(2000);	
		setIsBrakeWhenZero(true);
		set(0.0);
	}
	
	//出力する
	float set(const float v);
	
	//出力0のときにブレーキするかどうかをtrueかfalseで指定する
	bool setIsBrakeWhenZero(const bool v){
		return isBrakeWhenZero=v;
	}
	
	float operator=(const float v){
		return set(v);
	}
};

}//namespace rob



#endif