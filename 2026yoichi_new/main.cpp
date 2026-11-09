#include "mbed.h"
#include "2026roboconlib_setting.hpp"
#include "limitswitch.hpp"
#include <cmath>

const uint32_t RADIO_TIMEOUT_US = 500000; //500ms
const uint32_t SEND_INTERVAL_US = 20000; //20ms

rob::KKMD motor1(MD1_PINS, false);
rob::KKMD motor2(MD2_PINS, true);
rob::KKMD motor3(MD3_PINS, false);
rob::KKMD motor4(MD4_PINS, true);

Serial tombed(PB_6, PB_7);
mu3 mu3a;
Timer clockTimer;

class Mecanum {
    rob::KKMD **motors;
    const float motor_max_power;
    enum { MAX_MOTOR = 4, MOTOR_COUNT = 4 };
    void calculate_outputs(float x, float y, float rotation, float *outputs) {
        outputs[0] = y + x + rotation;
        outputs[1] = y - x - rotation;
        outputs[2] = y - x + rotation;
        outputs[3] = y + x - rotation;
    }
    void normalize_outputs(float *outputs) {
        float max_power = 1.0f;
        for (int i = 0; i < MOTOR_COUNT; ++i)
            if (max_power < fabs(outputs[i])) max_power = fabs(outputs[i]);
        for (int i = 0; i < MOTOR_COUNT; ++i)
            outputs[i] = outputs[i] / max_power * motor_max_power;
    }
public:
    Mecanum(rob::KKMD *(&array)[4]) : motors(array), motor_max_power(0.95f) {}
    void set_motor_outputs(const float *outputs) {
        for (int i = 0; i < MOTOR_COUNT; ++i) *motors[i] = outputs[i];
    }
    void move(float x, float y, float rotation) {
        float outputs[4];
        calculate_outputs(x, y, rotation, outputs); //変更前
		
		//calculate_outputs(y, -x, rotation, outputs); //元の配置の右側面を正面としたとき
        normalize_outputs(outputs);
        set_motor_outputs(outputs);
    }
};

rob::KKMD *motors[4] = {&motor1, &motor2, &motor3, &motor4};
Mecanum robo(motors);

volatile uint8_t packet[5] = {'n', 128, 128, 128, 128}; //button, lx, ly, rx, ry 
volatile uint32_t receivedAt = 0;
volatile bool received = false;

float stick_normalize(uint8_t raw_val) {
    const float DEADZONE = 12.0f;
    float value = (static_cast<float>(raw_val) - 127.5f) / 127.5f;
    if (fabs(value) < DEADZONE / 127.5f) value = 0.0f;
    return value;
}

void getC(const uint8_t *data, const uint8_t len) {
    if (len != 5) return;
    for(int i = 0; i < 5; ++i) {
		packet[i] = data[i];
	}
    receivedAt = (uint32_t)clockTimer.read_us();
    received = true;
}
// Retain the original one-byte escaped framing protocol.

struct SendData{
	//int32_t Encoder1;
	//int32_t Encoder2;
	char button;
};

union SendData_union{
	SendData data_raw;
	uint8_t data_binary[sizeof(SendData)];

};

void sendmbed(uint8_t *buf){
	tombed.putc(0x7E);
	
	for(int i=0;i<sizeof(SendData);i++){
		if(buf[i]==0x7E || buf[i]==0x3E||buf[i]==0x1E){
			tombed.putc(0x3E);
			tombed.putc(~buf[i]);
			
		} else {
			tombed.putc(buf[i]);
		}
	}

	tombed.putc(0x1E);
}

SendData_union sendData_union;

int main() {
    robo.move(0, 0, 0);
    clockTimer.start();
    pc.baud(115200);
    tombed.baud(115200);
	wait_ms(100);
    mu3serial.baud(19200);
    mu3a.set(15, 110, 2, 1); //ch,groupID,myNum,sendNum 1から254までで適当に決める。相手と合わせる。 //サブ：5chにする
    mu3a.attach(getC);
    wait_ms(1000);
    mu3serial.printf("@rc\r\n");
    mu3a.setup();
	wait_ms(100);
    uint32_t lastSend = (uint32_t)clockTimer.read_us();
	
    while (true) {
        uint8_t receivedArray[5];
		static rob::regularC_ms printTime(100);
        uint32_t mask = __get_PRIMASK(); //現在の割り込み禁止状態を保存
        __disable_irq(); //割り込みを一時的に禁止し、データのコピーの際に割り込まれないようにする
        for (int i = 0; i < 5; ++i) {
			receivedArray[i] = packet[i];
		}
        uint32_t timestamp = receivedAt;
        bool isRadioRecieved = received;
        __set_PRIMASK(mask); //前の状態に戻す
		
        uint32_t now = (uint32_t)clockTimer.read_us();
        bool alive = isRadioRecieved && ((now - timestamp) < RADIO_TIMEOUT_US); //MU-3のタイムアウト判定
        if(alive) {
            robo.move(stick_normalize(receivedArray[1]),
                      -stick_normalize(receivedArray[2]), 
					  stick_normalize(receivedArray[3]));
		} else {
			robo.move(0, 0, 0);
		}
		
        if ((now - lastSend) >= SEND_INTERVAL_US) {
            // A stale radio command must never become a new fire command.
			sendData_union.data_raw.button = alive ? sendData_union.data_raw.button=packet[0] : '!';
            sendmbed(sendData_union.data_binary);
            lastSend = now;
        }
		
		if(printTime) {
			//pc.printf("button=%c, lx=%d, ly=%d, rx=%d\r\n", receivedArray[0], receivedArray[1], receivedArray[2], receivedArray[3]);
		}
    }
}
