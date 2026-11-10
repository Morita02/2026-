#pragma once
#include "mbed.h"
#include "control_config.hpp"
#include <cmath>

extern Serial LEDs;

class Limitswitch{
private:
  DigitalIn limitswitch;
  bool ispositive;
public:
  Limitswitch(PinName pin, bool ispositive_arg = true): limitswitch(pin), ispositive(ispositive_arg){}
  bool isp(){
    if(ispositive){
      if(limitswitch.read() == 1){
        return true;
      }else{
        return false;
      }
    }else{
      if(limitswitch.read() == 0){
        return true;
      }else{
        return false;
      }
    }
  }
  operator bool(){return isp();}
};


class LaunchController {
public:
    enum State { 
		WAITING, 
		MOVE_ARM, 
		ARM_SETTLE, 
		MOVE_DRUM,
	    DRUM_SETTLE, 
		RELEASE, 
		RELEASE_SETTLE,
		DRUM_ADJUST,
		FAULT
	};
    State state;
    float armPower, drumPower;
    bool resetArm;
    const char *faultReason;
	Limitswitch SW;
private:
    bool armed, wasFire, settling;
	uint8_t failNum;
	bool JustMove;
    uint32_t stateSince, stableSince;
    float drumTarget, releaseStart;
    static float magnitude(float x) { return fabs(x); }
	
    void enter(State next, uint32_t now) { //次の状態へ行く、またその状態になった時刻を保存
		state = next; stateSince = now; settling = false;
	}
	
    bool stable(bool condition, uint32_t now) {
        if (!condition) { settling = false; return false; }
        if (!settling) { settling = true; stableSince = now; }
        return (uint32_t)(now - stableSince) >= cfg::SETTLE_US;
    }
    void fail(const char *reason, uint32_t now) {
        faultReason = reason; 
		armed = false; 
		enter(FAULT, now);
        armPower = drumPower = 0;
    }
public:
    LaunchController() : state(WAITING), armPower(0), drumPower(0), resetArm(false),
        faultReason("none"), armed(false), wasFire(false), settling(false),
        stateSince(0), stableSince(0), drumTarget(0), releaseStart(0), failNum(0), JustMove(false),SW(PA_14, false){}
	
    void tick(uint32_t now, bool alive, char button, float arm,
              float drum, float armSpeed, float drumSpeed) {
        armPower = 0;
		drumPower = 0; //最初に毎回出力を0にする処理をいれるため、状態の終わりに出力0を代入しなくてよい
		
		resetArm = false;
        bool fire = alive && button == 'i'; //発射条件
        bool edge = fire && !wasFire; 
        wasFire = fire;
        if (button == '!' && alive) {
            armed = false;
            if (state != WAITING && state != FAULT) fail("radio timeout", now);
            return;
        }
        if (!alive) {
            armed = false;
            if (state != WAITING && state != FAULT) fail("UART timeout", now);
            return;
        }
        
        if (state == WAITING) {
            if (button == 'n') armed = true;
            if (edge && armed) {
                armed = false; 
				resetArm = true;
                enter(MOVE_ARM, now); 
				armPower = cfg::ARM_POWER;
            }
			if(button == 'h') {
				enter(DRUM_ADJUST, now);
			}
            return;
        }
        uint32_t limit = (state == RELEASE || state == RELEASE_SETTLE) //時間制限 タイムアウト時間
            ? cfg::RELEASE_TIMEOUT_US : cfg::MOVE_TIMEOUT_US;
		
        if ((uint32_t)(now - stateSince) >= limit) { fail("motion timeout", now); return; }
		
        switch (state) {
			
        case MOVE_ARM:
            if (arm >= (cfg::ARM_LOAD_DEG-5)) 
				enter(ARM_SETTLE, now);
			
            else armPower = cfg::ARM_LOAD_DEG - arm <= (cfg::SLOW_ZONE_DEG+10)
                ? cfg::ARM_SLOW_POWER : cfg::ARM_POWER;
			
            break;

        case ARM_SETTLE:
			
			//ここで角度をちょっと調整
			//目標より小さいor大きいとアームを順方向または逆回転させて調整
			//このループが10回を超えると強制的に次のstateへ
			// if(abs(cfg::ARM_LOAD_DEG - arm) < cfg::POSITION_TOLERANCE_DEG) {
				// drumTarget += cfg::DRUM_STEP_DEG; 
				// failNum = 0;
				// enter(MOVE_DRUM, now); 
			// } else {
				// failNum++;
				// if((cfg::ARM_LOAD_DEG - arm) > 0.0) {
					// armPower = cfg::ARM_SLOW_POWER;
				// } else if((cfg::ARM_LOAD_DEG - arm) < 0.0) {
					// armPower = -1*(cfg::ARM_SLOW_POWER);
				// }
			// }
			
            if (stable(fabs(armSpeed) <= cfg::STOP_SPEED_DEG_S, now)) {
                if (magnitude(arm - cfg::ARM_LOAD_DEG) > cfg::POSITION_TOLERANCE_DEG)
                    fail("arm stop position", now);
                else { 
					drumTarget += cfg::DRUM_STEP_DEG; 
					enter(MOVE_DRUM, now);
				}
            }
            break;
			
        case MOVE_DRUM:
			
            if (drum >= drumTarget) {
				enter(DRUM_SETTLE, now);
			}
            else drumPower = drumTarget - drum <= cfg::SLOW_ZONE_DEG
                ? cfg::DRUM_SLOW_POWER : cfg::DRUM_POWER;
			
            break;
			
        case DRUM_SETTLE:
			//ここで角度をちょっと調整
			//目標より小さいor大きいとアームを順方向または逆回転させて調整
			//このループが10回を超えると強制的に次のstateへ
			if(abs(drumTarget - drum) < cfg::POSITION_TOLERANCE_DEG || failNum > cfg::FAIL_ABS_COUNT) {
				releaseStart = arm;
				failNum = 0;
				enter(RELEASE, now);
				if(JustMove == true) {
					JustMove = false;
					enter(WAITING, now);
				} else {
					enter(RELEASE, now);
				}
			} else {
				failNum++;
				if((drumTarget - drum) > 0.0) {
					drumPower = cfg::DRUM_SLOW_POWER;
				} else if((drumTarget - drum) < 0.0) {
					drumPower = -1*(cfg::DRUM_SLOW_POWER);
				}
			} 
			
			//below lines are made in AI.
			
            // if (stable(fabs(drumSpeed) <= cfg::STOP_SPEED_DEG_S &&
                       // fabs(armSpeed) <= cfg::STOP_SPEED_DEG_S, now)) {
                // if (magnitude(drum - drumTarget) > cfg::POSITION_TOLERANCE_DEG ||
                    // magnitude(arm - cfg::ARM_LOAD_DEG) > cfg::POSITION_TOLERANCE_DEG)
                    // fail("loading stop position", now);
                // else { 
					// releaseStart = arm; 
					// enter(RELEASE, now); 
				// }
		    // }
			
            break;
			
        case RELEASE:
            if (fabs(releaseStart - arm) >= cfg::ARM_RELEASE_DROP_DEG)
                enter(RELEASE_SETTLE, now);
            else armPower = cfg::RELEASE_POWER;
			
            break;
			
        case RELEASE_SETTLE:
            if (stable(fabs(armSpeed) <= cfg::STOP_SPEED_DEG_S &&
                       fabs(drumSpeed) <= cfg::STOP_SPEED_DEG_S, now))
                enter(WAITING, now);
            break;
			
		case FAULT:
			break;
		
		case DRUM_ADJUST:
			if(SW) {
				drumTarget = drum+77.4; //いちばんかかった77.4,78
				JustMove = true;
				enter(MOVE_DRUM, now);
			} else {
				drumPower = 0.1;
			}
			break;
        default: 
			fail("invalid state", now); 
			break;
        }
    }
};
