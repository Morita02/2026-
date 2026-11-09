#ifndef A_TB6643KQ_HPP_INCLUDE_GUARD
#define A_TB6643KQ_HPP_INCLUDE_GUARD

#include "mbed.h"

namespace rob{

class aTB6643KQ{
	private:
	PwmOut a,b;
	
	public:
	aTB6643KQ(PinName ap,PinName bp):
	a(ap),b(bp)
	{
		freq(23000);
	}
	
	float set(float);
	uint32_t freq(const uint32_t);
	
	float operator=(const float v){
		return set(v);
	}
};

}


#endif