#ifndef mu3_h
#define mu3_h
#include "Arduino.h"
#include <stdlib.h>
#include <stdint.h>

class Mu3{
  public:
    Mu3();
    char setnum[3];
    char sendSize[3];
    const char *num;
    int bufIndex=0;
    void (*ifReceive)(const char*);
    void set(int,int,int,int);
    void check();
    void send(char*, size_t);
    void attach(void(*)(const char*));
  private:
  
};

#endif
