#include "mu3.h"
#include <stdlib.h>
#include <string> 
using namespace std;

Serial pc(USBTX,USBRX);
//Serial mu3serial(PC_10,PC_11);//uart2,tx,rx PA_2,PA_3
//Serial mu3serial(PB_6,PB_7);//uart2,tx,rx PA_2,PA_3
Serial mu3serial(PC_10,PC_11);//uart2,tx,rx PA_2,PA_3

mu3::mu3(){
	bufIndex=0;
	recvBufIndex=0;
	dataLength=0;
	lengthVisit=0;
	ifReceive=NULL;

	recvState = WAITING_HEADER;
	headerIndex = 0;
}

void mu3::set(int ch,int group,int mynumber,int sendnumber){
  //Serial1.begin(19200, SERIAL_8N1,rspin,txpin);//rx.tx　serial2のデフォルトピンなんか調子悪いため。
  //while(!Serial1);
  //pinMode(4,INPUT);//rts
  //pinMode(5,OUTPUT);//cts
  //digitalWrite(5,LOW);
  wait_ms(300);//これ入れると通信安定する。
  pc.printf("setup start\n");
  mu3serial.printf("@ch");
  sprintf(setnum,"%02x",ch);
  mu3serial.printf(setnum);
  mu3serial.printf("\r\n");
  wait_ms(100);
  mu3serial.printf("@gi");
  sprintf(setnum,"%02x",group);
  mu3serial.printf(setnum);
  mu3serial.printf("\r\n");
  wait_ms(100);
  mu3serial.printf("@ei");
  sprintf(setnum,"%02x",mynumber);
  mu3serial.printf(setnum);
  mu3serial.printf("\r\n");
  wait_ms(100);
  mu3serial.printf("@di");
  sprintf(setnum,"%02x",sendnumber);
  mu3serial.printf(setnum);
  mu3serial.printf("\r\n");
  pc.printf("setup finished\n");
}
/*
void mu3::check(){
  if (mu3serial.available() > 0) {
    bufIndex=mu3serial.available();
    string str0 = mu3serial.readStringUntil('\n');
    //Serial.println(str0);
    string str1 = str0.substring(6, bufIndex-2);
    num=str1.c_str();
    ifReceive(num);
  }
}*/

//ぶっちゃけこれここで使わんから定義適当
void mu3::send(char* data){
  sprintf(sendSize,"%02x",strlen(data));
  mu3serial.printf("@dt");
  mu3serial.printf(sendSize);
  mu3serial.printf(data);
  mu3serial.printf("\r\n");
  //Serial.print(strlen(data));
  pc.printf("@dt");
  pc.printf(sendSize);
  pc.printf(data);
  pc.printf("\r\n");
}
void mu3::setup(){
	mu3serial.attach(callback(this,&mu3::whenReceiveFromMU3),Serial::RxIrq);
}

void mu3::attach(void (*p)(const uint8_t*, const uint8_t)){
  ifReceive=p;
}

void mu3::resetReceiver()
{
    recvState = WAITING_HEADER;
    headerIndex = 0;
    recvBufIndex = 0;
    lengthVisit = 0;
    dataLength = 0;
}

void mu3::whenReceiveFromMU3(){
	// const uint8_t c=mu3serial.getc();
	// if(c=='\n'){
	// }else if(c=='\r'){
		// recvBuf[recvBufIndex]='\0';
		// ifReceive(recvBuf+7);
		// recvBufIndex=0;
		//pc.printf("%s\n",recvBuf);
		// return;
	// }
	// recvBuf[recvBufIndex]=(uint8_t)c;
	// recvBufIndex++;
	
	static const char HEADER[] = "*DR=";
	while (mu3serial.readable()) {
		const uint8_t c=static_cast<uint8_t>(mu3serial.getc());
		
		switch(recvState) {
			case WAITING_HEADER:
				if(c == static_cast<uint8_t>(HEADER[headerIndex])) {
					headerIndex++;
					if(headerIndex == sizeof(HEADER)-1) {
						dataLength=0;
						lengthVisit = 0;
						recvBufIndex = 0;
						recvState = CONVERTION;
						
					}
				} else {
					headerIndex = (c == '*') ? 1 : 0;
				}
				break;
			case CONVERTION:
				int value;
				if (c >= '0' && c <= '9') {
					value = c - '0';
				} else if (c >= 'A' && c <= 'F') {
					value = c - 'A' + 10;
				} else if (c >= 'a' && c <= 'f') {
					value = c - 'a' + 10;
				} else {
					resetReceiver();
					headerIndex = (c == '*') ? 1 : 0;
					break;
				}

				dataLength = dataLength * 16 + value;
				lengthVisit++;
				
				if(lengthVisit == 2) {
					if(dataLength <= sizeof(recvBuf)) {
						lengthVisit = 0;
						recvState = (dataLength == 0) ? CR : INSERT_DATA;
					} else {
						resetReceiver();
					}
				}
				break;
			case INSERT_DATA:
				recvBuf[recvBufIndex++] = c;
				//pc.printf("%0X,", recvBuf[recvBufIndex-1]); ////test
				if(recvBufIndex == dataLength) {
					recvState = CR;
				}
				break;
			case CR:
				if(c == '\r') {
					recvState = LF;
				} else {
					resetReceiver();
					headerIndex = (c == '*') ? 1 : 0;
				}
				break;
			case LF:
				if(c == '\n' && ifReceive != NULL) {
					ifReceive(recvBuf, dataLength);
					
				}
				resetReceiver();
				headerIndex = (c == '*') ? 1 : 0;
				break;
			default:
				resetReceiver();
				break;
		}
	}

				
	
}
