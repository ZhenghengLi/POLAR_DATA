#ifndef Z_POLAR_DECODEPATTERN
#define Z_POLAR_DECODEPATTERN
#include <iostream>
#include <vector>
#include <string.h>
#include <string>
#define BYTE  8
using namespace std;

class DescConfig{
	public:
		DescConfig();
		~DescConfig();
        friend class DecodePattern;
	private:
		int id;
		string title;
		int type;
		int firstBegin;
		int firstEnd;
		int secondBegin;
		int secondEnd;
		int repeat;   // repeat for array and variable-size 
		int reserve;     // for reserve
		string flag;

};

class DecodePattern{
	public:
		DecodePattern();
		~DecodePattern();

		int readConfigureFile(const string& configFile);
		void setData( char* const data);
		int getID(const string& title);

		template<typename T>
			int getData(const int id,T* realData,const int number=0);

	private:
		char* repData;
		uint64_t decodeBit(const int beginBit, const int endBit);
		uint64_t decodeByte(const int beginByte, const int endByte);
		uint64_t decodeBit(const int beginBit,const int endBit, const int beginBit2, const int endBit2);

		std::vector<DescConfig*> VDescConfig;
};

template<typename T>
int DecodePattern::getData(const int id,T* realData,const int number){
	if(VDescConfig[id]->type==0){
		if(VDescConfig[id]->repeat<=1){
			if(VDescConfig[id]->flag=="bit"){
				*realData=this->decodeBit(VDescConfig[id]->firstBegin,VDescConfig[id]->firstEnd);
				return 0;
			}
			if(VDescConfig[id]->flag=="byte"){
				*realData=this->decodeByte(VDescConfig[id]->firstBegin,VDescConfig[id]->firstEnd);
				return 0;
			}
		}    

		if(VDescConfig[id]->repeat>1){
			
			if(VDescConfig[id]->repeat!=number){
			    std::cout<<"DecodePattern: getData(), Warning, array number ("<< number <<") != repeat ("<<VDescConfig[id]->repeat<<")!"<<std::endl;
			}
            
			if(number>VDescConfig[id]->repeat){
			    std::cout<<"DecodePattern: getData(), number ("<<number<<") larger than repeat ("<<VDescConfig[id]->repeat<<")!"<<std::endl;
				abort();
			}

			int firstBit=VDescConfig[id]->firstBegin;
			int endBit=VDescConfig[id]->firstEnd;
            
			if(VDescConfig[id]->flag=="byte"){
				firstBit=firstBit*8;
				endBit=endBit*8+7;
			}
            
			int step=(endBit-firstBit+1)/VDescConfig[id]->repeat;
             
			for(int i=0;i<VDescConfig[id]->repeat;i++){
			    realData[i]=this->decodeBit(firstBit+step*i,firstBit+step*(i+1)-1);
			}
			
            return 0;
		}
	}
	else if(VDescConfig[id]->type==1){
		int firstBit=VDescConfig[id]->firstBegin;
		int firstEnd=VDescConfig[id]->firstEnd;
		int secondBit=VDescConfig[id]->secondBegin;
		int secondEnd=VDescConfig[id]->secondEnd;
		if(VDescConfig[id]->flag=="byte"){
		   firstBit=firstBit*8;
		   firstEnd=firstEnd*8+7;
		   secondBit=secondBit*8;
		   secondEnd=secondEnd*8+7;
		}
		*realData=this->decodeBit(firstBit,firstEnd,secondBit,secondEnd);
		return 0;
	}
	else if(VDescConfig[id]->type==2){
		   int firstBit=VDescConfig[id]->firstBegin;
		   int endBit=VDescConfig[id]->firstEnd;
		   if(VDescConfig[id]->flag=="byte"){
		       firstBit=firstBit*8;
			   endBit=endBit*8+7;
		   }
           int moveInt=endBit-firstBit;

		   for(int i=0;i<number;i++){
		        realData[i]=this->decodeBit(firstBit,endBit);
				firstBit=endBit+VDescConfig[id]->repeat+1;//repeat->step
				endBit=firstBit+moveInt;
		   } 
           
		   return 0;
	}
    return 0;
}

#endif
