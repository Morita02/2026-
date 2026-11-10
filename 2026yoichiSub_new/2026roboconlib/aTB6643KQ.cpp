#include "aTB6643KQ.hpp"

namespace rob{
	
float aTB6643KQ::set(float v){
	//pc.printf("%6d",(int)(v*1000.0));
	//v*=0.5;
	if(v<-1.0){
		v=-1.0;
	}else if(v>1.0){
		v=1.0;
	}
	if(v<0.0){
		a=1.0+v;
		b=1.0;
	}else{
		a=1.0;
		b=1.0-v;
	}
	return v;
}

uint32_t aTB6643KQ::freq(uint32_t f){
	const int t=1000000/f;
	a.period_us(t);
	b.period_us(t);
	return f;
}

}