#pragma once
#include <stdint.h>
namespace cfg {
// PPR is cycles/revolution BEFORE x4. Verify the encoder DIP switches.
const float ARM_PPR = 2048.0f;
const float DRUM_PPR = 2048.0f;
const int ARM_ENCODER_SIGN = -1; //反転
const int DRUM_ENCODER_SIGN = 1; //非反転

const float ARM_LOAD_DEG = 90.0f; //ドラムが回る前にアームが固定される角度(アームの初期位置から)
const float DRUM_STEP_DEG = 72.0f; //ドラムが一度に回る角度,多分変えない

// Initial release detection: legacy 200 x2 counts at assumed 2048 PPR.
const float ARM_RELEASE_DROP_DEG = 25.0f; //アームが解放されたって判定される角度の差分
const float POSITION_TOLERANCE_DEG = 5.0f; //目標角度との許容誤差
const float SLOW_ZONE_DEG = 15.0f; //アームがこの角度以下の時ゆっくりになる(このパラメータ必要か？)
const float STOP_SPEED_DEG_S = 5.0f; //アーム,ドラムが停止したと判定する速度の許容誤差スピード

const uint32_t SETTLE_US = 100000; //以下4つがタイムアウト時間(ある状態のとき、この時間[us]が経過したら失敗と判定する)
const uint32_t MOVE_TIMEOUT_US = 10000000; 
const uint32_t RELEASE_TIMEOUT_US = 5000000;
const uint32_t LINK_TIMEOUT_US = 500000;

const float ARM_POWER = 0.6f; //通常時のアーム速度duty比
const float ARM_SLOW_POWER = 0.2f; //スロー時のアーム速度duty比
const float DRUM_POWER = 0.105f; //通常時のドラム速度duty比
const float DRUM_SLOW_POWER = 0.05f; //スロー時のドラム速度duty比
const float RELEASE_POWER = 0.4f; //解放時のアーム速度duty比
const uint8_t FAIL_ABS_COUNT = 10; //角度調整のループ最大回数
}
