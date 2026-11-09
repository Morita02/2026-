#include "useful.hpp"

namespace rob{


//Serial pc(PC_10,PC_11,115200);
//Serial deb(USBTX,USBRX,115200);
//Serial pc(USBTX,USBRX,115200);
DigitalOut led(LED1);

namespace __flt_internal__{
	//myFlt ver.0.1 by mss7z 201908250133
	const int bufMax=10;
	char buf[bufMax][20]={""};
	char *c=NULL;
	int cnt=0;
	char *newBuf(){
		if(!(cnt<bufMax)){
			cnt=0;
		}
		c=buf[cnt];
		cnt++;
		return c;
	}
	void add(const char x){
		*c=x;
		c++;
	}
	void pltMsg(const char s[]){
		const char *p=s;
		while(*p!='\0'){
			add(*p);
			p++;
		}
		add('\0');
	}
	uint32_t intPow10(const int jo){
		uint32_t ans=1;
		for(int i=0;i<jo;i++){
			ans*=10;
		}
		return ans;
	}
	int addUint(uint32_t val,int maxk){
		int printed=maxk+1;
		for(;maxk>=0;maxk--){
			uint32_t h=intPow10(maxk);
			int k=(val/h);
			add(k+'0');
			val-=(k*h);
		}
		return printed;
	}
	int addUintNonTopZero(uint32_t val){
		//pc.printf("addUint %d\n",val);
		int maxk=1;
		while(true){
			if(maxk>=9){
				return -1;
			}
			if(val<intPow10(maxk)){
				maxk--;
				break;
			}
			maxk++;
		}
		return addUint(val,maxk);
	}
	char *flt(float val,const int decimal){
		char *s=newBuf();
		if(decimal>=9){
			pltMsg("INVALID_DECIMAL_ERR");
			return s;
		}
		if(val<0.0){
			add('-');
			val*=-1.0;
		}
		uint32_t beforeDecimal=(uint32_t)val;
		if(addUintNonTopZero(beforeDecimal)==-1){
			s=newBuf();
			pltMsg("TOO_LARGE_VAL_ERR");
			return s;
		}
		add('.');
		int printed;
		if( (printed=addUint(((val-(float)beforeDecimal)*intPow10(decimal)),decimal-1))==-1 ){
			s=newBuf();
			pltMsg("INT_ERR_PLS_DEBUG");
			return s;
		}
		for(int i=printed;i<decimal;i++){
			add('0');
		}
		add('\0');
		return s;
	}
	
}

}