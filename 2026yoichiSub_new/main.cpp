#include "mbed.h"
#include "2026roboconlib_setting.hpp"
#include "launch_controller.hpp"

Serial pc(USBTX, USBRX);
Serial tombed(PA_0, PA_1);
Serial LEDs(PC_6, PC_7);

rob::KKMD motor1(MD1_PINS, false); // アーム
rob::KKMD motor2(MD2_PINS, false); // パンジャンドラム
Timer clockTimer;
volatile uint32_t receivedAt = 0;
volatile bool received = false;

volatile int bufEnd = 0;
uint8_t buf[255];
volatile int bufLenForRead = 0;
volatile bool isReceive = false;
volatile bool isNextEscaped = false;

struct Sample {
    uint32_t us;
    float armAngle, armSpeed, drumAngle, drumSpeed;
    float armRawSpeed, drumRawSpeed;
    uint32_t state;
};


// Keep the latest 20.48 seconds at 10ms intervals; no SD card required.
//過去20.48秒分のログをsamples配列に書き込む。LOG_CAPACITYの長さを超えたら古い順から削除する(リングバッファ)
//これ必要なんか
// const unsigned LOG_CAPACITY = 2048;
// Sample samples[LOG_CAPACITY];
// unsigned logNext = 0, logCount = 0, dumpIndex = 0, dumpRemaining = 0;
// uint32_t shotStarted = 0, lastLog = 0;
// bool recording = false;

// void recordSample(uint32_t now, unsigned state) {
    // Sample &s = samples[logNext];
    // s.us = now - shotStarted;
    // s.armAngle = rob::Encoder1.angleDeg();
    // s.armSpeeds = rob::Encoder1.speedDegPerSec();
    // s.drumAngle = rob::Encoder2.angleDeg();
    // s.drumSpeed = rob::Encoder2.speedDegPerSec();
    // s.armRawSpeed = rob::Encoder1.rawSpeedDegPerSec();
    // s.drumRawSpeed = rob::Encoder2.rawSpeedDegPerSec();
    // s.state = state;
    // logNext = (logNext + 1) % LOG_CAPACITY;
    // if (logCount < LOG_CAPACITY) ++logCount;
    // lastLog = now;
// }

struct SendData{
	//int32_t Encoder1;
	//int32_t Encoder2;
	volatile char button;
};

union Data_union{
	SendData data_raw;
	uint8_t data_binary[sizeof(SendData)];
};

Data_union data = {{'n'}};

void getCtombed(){
	uint8_t c = tombed.getc();
	if(c == 0x7E) {
		isReceive = true;				//スタート数を受信
		isNextEscaped = false;
		bufEnd = 0;
	} else if(isReceive) {
		if(c == 0x3E) {
			isNextEscaped = true;		//エスケープ数を受信
		} else if(c == 0x1E) {
			if(bufEnd == sizeof(SendData)) {		//エンド数を受信
				memcpy(data.data_binary,buf,bufEnd);
			}
			receivedAt = (uint32_t)clockTimer.read_us();
			received = true;
			isReceive = false;
			isNextEscaped = false;
			bufLenForRead = bufEnd;
		} else {									//通常データ受信
			buf[bufEnd] = isNextEscaped ? ~c:c;
			isNextEscaped = false;
			if(bufEnd < sizeof(buf)-2){			//存在しない領域に書き込まないようにする
				bufEnd++;
			}
		}
	}
}


int main() {
    motor1 = 0; 
	motor2 = 0;
    pc.baud(9600);
	
    clockTimer.start();
    rob::Encoder1.setup();
    rob::Encoder2.setup();

    tombed.baud(115200);
    tombed.attach(getCtombed, Serial::RxIrq);
	
	LEDs.baud(115200);
    LaunchController controller;
    uint32_t lastSample = (uint32_t)clockTimer.read_us();
    LaunchController::State lastState = controller.state;
    pc.printf("get ready\r\n");
	
	LEDs.printf("%d\n", controller.state);
	
	SendData & d = data.data_raw;

    while (true) {
		static rob::regularC_ms printTime(100);
        uint32_t now = (uint32_t)clockTimer.read_us();
        uint32_t elapsed = now - lastSample;
        if (elapsed < 1000) { wait_us(100); continue; }
        lastSample = now;
        rob::Encoder1.update(elapsed * 0.000001f);
        rob::Encoder2.update(elapsed * 0.000001f);
		
        uint32_t mask = __get_PRIMASK(); //割り込み禁止&コピー
		__disable_irq();
        uint32_t timestamp = receivedAt;
        bool isMbedRecieved = received;
        __set_PRIMASK(mask);
		
        // Read time after the IRQ snapshot, avoiding a just-arrived future timestamp.
        now = (uint32_t)clockTimer.read_us();
        bool alive = isMbedRecieved && ((now - timestamp) < cfg::LINK_TIMEOUT_US);
		
        controller.tick(now, alive, d.button,
            -(rob::Encoder1.angleDeg()), rob::Encoder2.angleDeg(),
            -(rob::Encoder1.speedDegPerSec()), 
			rob::Encoder2.speedDegPerSec());

        if (controller.resetArm) {
            rob::Encoder1.reset();
            // logNext = logCount = dumpRemaining = 0;
            // shotStarted = now; lastLog = now; recording = true;
            // recordSample(now, controller.state);
        }
		
        motor1 = controller.armPower;
        motor2 = controller.drumPower;
		
        // if (recording && ((uint32_t)(now - lastLog) >= 10000 || controller.state != lastState))
            // recordSample(now, controller.state);
        // if (controller.state == LaunchController::WAITING || controller.state == LaunchController::FAULT)
            // recording = false;
		
        if (controller.state != lastState) {
			LEDs.printf("%d\n", controller.state);
        }
		
        // Serial terminal: send d while stopped to export the last shot.
        // Export at most one row per loop; UART reception stays enabled.
        // bool stopped = controller.state == LaunchController::WAITING ||
                       // controller.state == LaunchController::FAULT;
		
		if(printTime) {
			// pc.printf("arm=%.4f,armSpeed=%.4f,armP=%.4f,drum=%.4f,drumSpeed=%.4f,drumP=%.4f,state=%d,button=%c\r\n", 
					  // -(rob::Encoder1.angleDeg()), -(rob::Encoder1.speedDegPerSec()),controller.armPower,
                      // rob::Encoder2.angleDeg(),rob::Encoder2.speedDegPerSec(), 
					  // controller.drumPower,controller.state,d.button);
			//pc.printf("%c, %d\r\n", d.button, controller.state);
			if(controller.SW) {
				pc.printf("HHHHHHHHHHHHHHHHHh\r\n");
			} else {
				pc.printf("NNNNNNN\r\n");
			}
		}
		
    }
}
