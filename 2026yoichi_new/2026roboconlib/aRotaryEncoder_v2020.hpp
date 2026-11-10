/*
	名前の通りrotaryEcoderを操るクラス
*/

#ifndef A_ROTARY_ENCODER_V2020_HPP_INCLUDE_GUARD
#define A_ROTARY_ENCODER_V2020_HPP_INCLUDE_GUARD

#include "mbed.h"
namespace rob{
namespace aRotaryEncoder_v2020_internal{
	class aRotaryEncoder {
	private:
		DigitalIn Aphs;
		//InterruptIn Aphs;
		InterruptIn Bphs;
		
		int val;
		int diff;
		
		void BphsRiseProcF();
		void BphsFallProcF();
		
		void AphsRiseProcF();
		void AphsFallProcF();
		
	public:
		aRotaryEncoder(PinName AphsPin,PinName BphsPin,PinMode mode=PullNone,bool isForward=true);
		//inline関数化
		void reset()	{val = 0;diff = 0;}
		int read()const {return val-diff;}
		void set(const int);
		int readRaw()const {return val;}

		operator int()const {return read();}
	};
	inline void aRotaryEncoder::set(const int comp){
		diff=val-comp;
		return;
	}

}//end of __aRotaryEncoder_internal__
using aRotaryEncoder_v2020_internal::aRotaryEncoder;
}
#endif