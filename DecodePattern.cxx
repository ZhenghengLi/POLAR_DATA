#include "../include/DecodePattern.h"
#include <stdlib.h>
#include <fstream>
#define COMMENT_CHAR "#"
DescConfig::DescConfig():id(-1),title(""),type(-1),firstBegin(-1),firstEnd(-1),secondBegin(-1),secondEnd(-1),repeat(-1),reserve(-1),flag(""){

}
DescConfig::~DescConfig(){

}
/*****************************************/
DecodePattern::DecodePattern():repData(NULL){
    VDescConfig.clear();
}

DecodePattern::~DecodePattern(){
    for(int i=0;i<VDescConfig.size();i++)delete VDescConfig[i];
	VDescConfig.clear();
}

int DecodePattern::readConfigureFile(const string& configFile){
     ifstream _file(configFile);
	 if(!_file){
		 std::cout<<"DecodePattern: readConfigureFile(), Can't get configure file '"<<configFile<<"' !"<<std::endl;
		 abort();
	 }
	 string rawLine;

	 while(getline(_file,rawLine)){
		 if(rawLine.empty())continue;
		 int pos=rawLine.find(COMMENT_CHAR);
		 if(pos==0)continue;
         
		 char line[350];
		 strcpy(line,rawLine.c_str());

		 DescConfig* descConfig=new DescConfig;

		 char* delim=" ,";
		 descConfig->id=atoi(strtok(line,delim));
		 descConfig->title=(string)strtok(NULL,delim);
		 descConfig->type=atoi(strtok(NULL,delim));
		 descConfig->firstBegin=atoi(strtok(NULL,delim));
		 descConfig->firstEnd=atoi(strtok(NULL,delim));
		 descConfig->secondBegin=atoi(strtok(NULL,delim));
		 descConfig->secondEnd=atoi(strtok(NULL,delim));
		 descConfig->repeat=atoi(strtok(NULL,delim));
		 descConfig->reserve=atoi(strtok(NULL,delim));
		 descConfig->flag=(string)(strtok(NULL,delim));
         
		 if(descConfig->id<0||descConfig->type<0||descConfig->repeat<0||
				 descConfig->firstBegin<0||descConfig->firstEnd<0){
		     std::cout<<"DecodePattern: readConfigureFile(), Can't read parameters correctly from configure file '"<<configFile<<"' !"<<std::endl;
			 abort();
		 }

		 VDescConfig.push_back(descConfig);
	 }
	 _file.close();
	 return 0;

}

void DecodePattern::setData(char* const data){
	if(data==NULL){
	   std::cout<<"DecodePattern: setData(), data is NULL!"<<std::endl;
       abort();
	}
	repData=data;
}
int DecodePattern::getID(const string& title){
    /*
    std::vector<DescConfig*>::iterator iter;
	for(iter=VDescConfig.begin();iter!=VDescConfig.end();++iter){
	    if((*iter)->title==title){
		   
		}
	}
	 */	
	for(int i=0;i<VDescConfig.size();i++){
		if(VDescConfig[i]->title==title)return i;
	}

	std::cout<<"DecodePattern: getID(), Can't find '"<<title<<"' !"<<std::endl;
	return -1;
}

uint64_t DecodePattern::decodeBit(const int beginBit, const int endBit){
	uint64_t sum=0;//64bits

	int nByte=endBit/BYTE;
	int resBit=endBit%BYTE;

	int nByte2=beginBit/BYTE;
	int resBit2=beginBit%BYTE;

	if(nByte==nByte2){
		switch(resBit2){
			case 0:
				sum=repData[nByte]&0x00ff;
				break;
			case 1:
				sum=repData[nByte]&0x007f;
				break;
			case 2:
				sum=repData[nByte]&0x003f;
				break;
			case 3:
				sum=repData[nByte]&0x001f;
				break;
			case 4:
				sum=repData[nByte]&0x000f;
				break;
			case 5:
				sum=repData[nByte]&0x0007;
				break;
			case 6:
				sum=repData[nByte]&0x0003;
				break;
			case 7:
				sum=repData[nByte]&0x0001;
				break;
		}

		sum>>=(BYTE-resBit-1);
		return sum;
	}

	sum=repData[nByte]&0x00ff;
	sum>>=(BYTE-resBit-1);


	int iByte=1;
	while((nByte-iByte)>nByte2){
		uint64_t midByte=0;
		midByte=repData[nByte-iByte]&0x00ff;
		midByte<<=(resBit+1+(iByte-1)*BYTE);
		sum+=midByte;
		iByte++;
	}

	uint64_t firstByte=0;
	switch(resBit2){
		case 0:
			firstByte=repData[nByte2]&0x00ff;
			break;
		case 1:
			firstByte=repData[nByte2]&0x007f;
			break;
		case 2:
			firstByte=repData[nByte2]&0x003f;
			break;
		case 3:
			firstByte=repData[nByte2]&0x001f;
			break;
		case 4:
			firstByte=repData[nByte2]&0x000f;
			break;
		case 5:
			firstByte=repData[nByte2]&0x0007;
			break;
		case 6:
			firstByte=repData[nByte2]&0x0003;
			break;
		case 7:
			firstByte=repData[nByte2]&0x0001;
			break;
	}
	firstByte<<=(resBit+1+(iByte-1)*BYTE);

	sum+=firstByte;

	return sum;
}

uint64_t DecodePattern::decodeByte(const int beginByte, const int endByte){
	return this->decodeBit(beginByte*8,endByte*8+7);
}

uint64_t DecodePattern::decodeBit(const int beginBit,const int endBit, const int beginBit2, const int endBit2){
    uint64_t sum=0;
	sum=this->decodeBit(beginBit,endBit);

	int moveInt=endBit2-beginBit2+1;
	sum<<=moveInt;
	sum+=this->decodeBit(beginBit2,endBit2);

	return sum;
}

