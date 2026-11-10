/*
#include <ps5.h>
#include <ps5Controller.h>
#include <ps5_int.h>
*/
/*
 Example sketch for the PS5 USB library - developed by Kristian Sloth Lauszus
 For more information visit the Github repository: github.com/felis/USB_Host_Shield_2.0 or
 send me an e-mail: lauszus@gmail.com
 */

//#include <XBOXUSB.h>
#include <XBOXUSB.h>
#include <stdio.h>
#include <stdlib.h>
#include "mu3.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

Mu3 mu3;
USB Usb;
//ps3USB ps3(&Usb);
XBOXUSB Xbox(&Usb);

bool printAngle = false, printTouch = false;
uint16_t lastMessageCounter = -1;
uint8_t player_led_mask = 0;
bool microphone_led = false;

class Origin{
private:
  Origin *next_node;
  static Origin *start_node;
  static Origin **previous_node_s_next_node;
public:
  Origin();
  //~Origin(){};
  virtual void setup() = 0;
  virtual void loop() = 0;
  static void setup_all(){
    Origin *current;
    for(current = start_node; current != nullptr; current = current->next_node){
      current->setup();
    }
    Serial.write("setup complete\n");
  }
  static void loop_all(){
    Origin *current;
    for(current = start_node; current != nullptr; current = current->next_node){
      current->loop();
    }
  }
  static void print_all(){
    Origin *current;
    for(current = start_node; current != nullptr; current = current->next_node){
      Serial.print((unsigned int)current);
      Serial.print("\n");
    }
  }
};
Origin *Origin::start_node = nullptr;
Origin **Origin::previous_node_s_next_node = nullptr;
Origin::Origin(){
  next_node = nullptr;
  if(start_node == nullptr){
    start_node = this;
  }else{
    *previous_node_s_next_node = this;
  }
  previous_node_s_next_node = &next_node;
}
/*
Origin::~Origin(){
  Serial.write("delete start\n");
  Origin *current;
  Origin *previous_node = nullptr;
  if(start_node == this){
    Serial.write("a");
    current = start_node;
    start_node = current->next_node;
    Serial.write("delete okk\n");
  }else{
    Serial.write("b");
    for(current = start_node; current != this ; current = current->next_node){
      previous_node = current;
      Serial.write("c");
    }
    previous_node->next_node = current->next_node;
    Serial.write("delete ok\n");
  }
}
*/
class Limitswitch:Origin{
private:
  byte limitswitchpin;
  bool ispositive;
public:
  Limitswitch(byte pin, bool ispositive_arg = true);
  void setup()override;
  void loop()override{}
  bool isp(){
    if(ispositive){
      if(digitalRead(limitswitchpin) == HIGH){
        return true;
      }else{
        return false;
      }
    }else{
      if(digitalRead(limitswitchpin) == LOW){
        return true;
      }else{
        return false;
      }
    }
  }
  operator bool(){return isp();}
};
Limitswitch::Limitswitch(byte pin, bool ispositive_arg){
  limitswitchpin = pin;
  ispositive = ispositive_arg;
}
void Limitswitch::setup(){
  pinMode(limitswitchpin, INPUT_PULLUP);
  Serial.write("switch setup ok\n");
}

class regularC{
private:
  unsigned long interval;
  unsigned long nextTime;
public:
  regularC(unsigned long intervalArg,unsigned long start=0):
    interval(intervalArg)
  {
    nextTime=start;
  }
  bool ist(){
    if(nextTime<millis()){
      nextTime=interval+nextTime;
      return true;
    }else{
      return false;
    }
  }
  operator bool(){return ist();}
  void set(unsigned long val){interval=val;}
  unsigned long read(){return interval;}
};

// Limitswitch first_row(32, false);
// Limitswitch second_row(33, false);
// Limitswitch third_row(25, false);
// Limitswitch fourth_row(26, false);

char buf[5];

void getMu3(const char* num){
  //Serial.println(num);
}

void format(){
  
  uint8_t lx, ly, rx, ry;
  int index = 0;
  
  if(Xbox.getButtonPress(R3)!=0)buf[index++]='c';
  else if(Xbox.getButtonPress(L3)!=0)buf[index++]='v';
  else if(Xbox.getButtonPress(Y)!=0)buf[index++]='h'; //サンカク
  else if(Xbox.getButtonPress(B)!=0)buf[index++]='i'; //ps3だとマル
  else if(Xbox.getButtonPress(X)!=0)buf[index++]='o'; //シカク
  else if(Xbox.getButtonPress(A)!=0)buf[index++]='p'; //バツ
  else if(Xbox.getButtonPress(UP)!=0)buf[index++]='w';
  else if(Xbox.getButtonPress(DOWN)!=0)buf[index++]='s';
  else if(Xbox.getButtonPress(LEFT)!=0)buf[index++]='a';
  else if(Xbox.getButtonPress(RIGHT)!=0)buf[index++]='d';
  else if(Xbox.getButtonPress(L1)!=0)buf[index++]='l';
  else if(Xbox.getButtonPress(R1)!=0)buf[index++]='r';
  else if(Xbox.getButtonPress(L2)!=0)buf[index++]='k';
  else if(Xbox.getButtonPress(R2)!=0)buf[index++]='t';
  else if(Xbox.getButtonClick(START)!=0)buf[index++]='g';
  else buf[index++]='n';

  // Convert Xbox signed 16-bit axes to PS3-compatible bytes; invert Y axes.
  buf[index++] = static_cast<uint8_t>((static_cast<int32_t>(Xbox.getAnalogHat(LeftHatX)) + 32768) / 256);
  buf[index++] = static_cast<uint8_t>((32767 - static_cast<int32_t>(Xbox.getAnalogHat(LeftHatY))) / 256);
  buf[index++] = static_cast<uint8_t>((static_cast<int32_t>(Xbox.getAnalogHat(RightHatX)) + 32768) / 256);
  buf[index++] = static_cast<uint8_t>((32767 - static_cast<int32_t>(Xbox.getAnalogHat(RightHatY))) / 256);

  // lx = Xbox.getAnalogHat(LeftHatX);
  // ly = Xbox.getAnalogHat(LeftHatY);
  // rx = Xbox.getAnalogHat(RightHatX);
  // ry = Xbox.getAnalogHat(RightHatY);

  // buf[index++] = lx & 0xFF;
  // //buf[index++] = (lx >> 8) & 0xFF;
  // buf[index++] = ly & 0xFF;
  // //buf[index++] = (ly >> 8) & 0xFF;
  // buf[index++] = rx & 0xFF;
  // //buf[index++] = (rx >> 8) & 0xFF;
  // buf[index++] = ry & 0xFF;
  // //buf[index++] = (ry >> 8) & 0xFF;
  
}



// char surinukecheck(){
//   char val;
//   if(first_row)val='1';
//   else if(second_row)val='2';
//   else if(third_row)val='3';
//   else if(fourth_row)val='4';
//   else val='0';
//   return val;
// }

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nXbox 360 USB Library Started"));
  Serial1.begin(19200, SERIAL_8N1,21,22);//rx,tx　serial2のデフォルトピンなんか調子悪いため。
  delay(1000);
  mu3.set(15,110,1,2);//ch,groupID,myNum,sendNum 1から254までで適当に決める。相手と合わせる。
  mu3.attach(getMu3);
  Serial1.flush();
  Serial1.print("@rc");
  Serial1.print("\r\n");
  Origin::setup_all();
}

void loop() {
  size_t len;
  static regularC checkps3Time(50); //50
  static regularC time(100);
  mu3.check();
  Usb.Task();
  
  if(checkps3Time){
    if (Xbox.Xbox360Connected){//&& lastMessageCounter != Xbox.getMessageCounter())
    //if(ps3.ps3360Connected){
      //lastMessageCounter = Xbox.getMessageCounter();
      
      format();
      //val = surinukecheck();
      //sprintf(sendmessage,"%c",key);
      len = sizeof(buf);
      mu3.send(buf, len);
      //Serial.print(buf[0]);
      
      /*
      for(size_t i = 0; i < len; i++) {
        Serial.print((uint8_t)buf[i], HEX);
        Serial.print(" ");
      }
      
      Serial.println();
      */
    }
  }
  if(time) {
    for(int i = 1; i<5; i++) {
      Serial.println(buf[i], HEX);
    }
  }
  /*static regularC checkRSSILevelTime(1000);
  if(checkRSSILevelTime){
    
  }*/
} 
