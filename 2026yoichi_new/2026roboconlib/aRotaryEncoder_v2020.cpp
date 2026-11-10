#include "aRotaryEncoder_v2020.hpp"

namespace rob{
namespace aRotaryEncoder_v2020_internal{
	/*
	PinMode list
	PullUp / PullDown / PullNone
	*/
	aRotaryEncoder::aRotaryEncoder(PinName AphsPin,PinName BphsPin,PinMode mode,bool isForward):
		Aphs(AphsPin),//ここで初期化
		Bphs(BphsPin)
	{
		Aphs.mode(mode);
		Bphs.mode(mode);
		if(isForward){
			Bphs.rise(callback(this, &aRotaryEncoder::BphsRiseProcF));
			Bphs.fall(callback(this, &aRotaryEncoder::BphsFallProcF));
			
			//Aphs.rise(callback(this, &aRotaryEncoder::AphsRiseProcF));
			//Aphs.fall(callback(this, &aRotaryEncoder::AphsFallProcF));
		}else{
			Bphs.fall(callback(this, &aRotaryEncoder::BphsRiseProcF));
			Bphs.rise(callback(this, &aRotaryEncoder::BphsFallProcF));
			
			//Aphs.fall(callback(this, &aRotaryEncoder::AphsRiseProcF));
			//Aphs.rise(callback(this, &aRotaryEncoder::AphsFallProcF));
		}
		val=0;
		diff=0;
	}

	void aRotaryEncoder::BphsRiseProcF(){
		if(Aphs)
			val++;
		else
			val--;
		return;
	}
	void aRotaryEncoder::BphsFallProcF(){
		if(Aphs)
			val--;
		else
			val++;
		return;
	}
	void aRotaryEncoder::AphsRiseProcF(){
		if(Bphs)
			val--;
		else
			val++;
		return;
	}
	void aRotaryEncoder::AphsFallProcF(){
		if(Bphs)
			val++;
		else
			val--;
		return;
	}

}//end of aRotaryEncoder_internal
}