#pragma once
#include "mbed.h"
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


