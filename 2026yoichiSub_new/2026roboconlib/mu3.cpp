#include "mu3.h"
#include <stdlib.h>
#include <string> 
using namespace std;

Serial pc(USBTX,USBRX);
//Serial mu3serial(PC_10,PC_11);//uart2,tx,rx PA_2,PA_3
//Serial mu3serial(PB_6,PB_7);//uart2,tx,rx PA_2,PA_3
Serial mu3serial(PA_0,PA_1);//uart2,tx,rx PA_2,PA_3

mu3::mu3(){
	bufIndex=0;
  recvBufIndex=0;
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

void mu3::attach(void (*p)(const uint8_t*)){
  ifReceive=p;
}


void mu3::whenReceiveFromMU3(){
	const uint8_t c=mu3serial.getc();
	if(c=='\n'){
	}else if(c=='\r'){
		recvBuf[recvBufIndex]='\0';
		ifReceive(recvBuf+7);
		recvBufIndex=0;
		// pc.printf("%s\n",recvBuf);
		return;
	}
	recvBuf[recvBufIndex]=(uint8_t)c;
	recvBufIndex++;
	
	
}
