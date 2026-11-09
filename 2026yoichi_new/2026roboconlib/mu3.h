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
	int lengthVisit;
    void (*ifReceive)(const uint8_t*, uint8_t);
    void set(int,int,int,int);
    //void check();
    void send(char*);
	void resetReceiver();
    void attach(void(*)(const uint8_t*, const uint8_t));
	void setup();
  private:
	enum RecvState {
		WAITING_HEADER,
		CONVERTION,
		INSERT_DATA,
		CR,
		LF
	};
	
	RecvState recvState;
	unsigned int headerIndex;
	
    uint8_t recvBuf[255];
	int recvBufIndex;
	uint8_t dataLength;
	void whenReceiveFromMU3();
  
};

#endif
