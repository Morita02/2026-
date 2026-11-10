#ifndef mu3_h
#define mu3_h
#include <stdlib.h>
#include "mbed.h"

extern Serial pc;
extern Serial mu3serial;

class mu3{
  public:
    mu3();
    char setnum[3];
    char sendSize[3];
    const char *num;
    int bufIndex;
    void (*ifReceive)(const uint8_t*);
    void set(int,int,int,int);
    //void check();
    void send(char*);
    void attach(void(*)(const uint8_t*));
	void setup();
  private:
    uint8_t recvBuf[255];
	int recvBufIndex;
	void whenReceiveFromMU3();
  
};

#endif
