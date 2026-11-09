#pragma once
#include "mbed.h"
namespace rob {
	
class HardwareEncoder {
    TIM_TypeDef *tim;
    int sign; //方向
    uint16_t previous;
    int64_t total;
    float velocity;
    float rawVelocity;
    float countsPerRev;
	
	static int32_t encoderDelta(uint16_t current, uint16_t previous) {
		int32_t d = (int32_t)current - (int32_t)previous;
		
		if (d > 32767) {
			d -= 65536; //例えば3から65530とかに飛んだらマイナス方向にすすんでるから-9カウントと補正
		} else if (d < -32768) {
			d += 65536; //例えば3から65530とかに飛んだらマイナス方向にすすんでるから+9カウントと補正
		}
		//CNTは0から65535で折り返すため補正する必要がある
		return d;
	}
public:
    HardwareEncoder(TIM_TypeDef *timer, int direction, float ppr)
        : tim(timer), sign(direction), previous(0), total(0), velocity(0), rawVelocity(0),
          countsPerRev(ppr * 4.0f) {}
    void setup();
    void update(float seconds) {
        uint16_t current = (uint16_t)tim->CNT; //CNTはuint32レジスタとして宣言されているため、キャスト
        int32_t delta = encoderDelta(current, previous) * sign; //差分計算
        previous = current;
        total += delta;
        if (seconds > 0) {
            rawVelocity = delta / seconds; //瞬時値
            float alpha = seconds / (0.02f + seconds);
            velocity += alpha * (rawVelocity - velocity); //第一次ローパスフィルタらしい(平滑化して速度をゆっくり追従)
        }
    }
    void reset() { previous = (uint16_t)tim->CNT; total = 0; velocity = rawVelocity = 0; }
    int64_t read() const { return total; } //readの前に必ずupdateを呼ぶこと
    float speed() const { return velocity; } // count/s, filtered
    float angleDeg() const { return total * 360.0 / countsPerRev; }
    float speedDegPerSec() const { return velocity * 360.0f / countsPerRev; }
    float rawSpeedDegPerSec() const { return rawVelocity * 360.0f / countsPerRev; }
};

}
