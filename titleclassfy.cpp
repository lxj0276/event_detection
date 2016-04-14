#include<iostream>
#include "../commom/func.h"
#include "../wordseg/seg.h"
#define TOPICNUM 25
#define CENRENUM 4
#include "../categorization/textcategorization.h"
int main(int argc, char *argv[]) {
	classfy::DictClassfy dicttopic;
	//classfy::DictClassfy dictgenre;
	//load dict
	dicttopic._LoadDict(TOPICNUM,argv[1]);
	commom::Func f;
	std::map<std::string, int>bizuindict;
	FILE*fi =  fopen(argv[2],"r");
	FILE*fo = fopen(argv[3],"ab+");
	if ((fi == NULL)||(fo == NULL)) {
		commom::LOG_INFO("open file error");
		return false;
	}	
	char buffer[MAX_LENTH];		
	std::string str = "";
	std::vector<std::string> v;
	while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		str = f.GetLine(buffer); 
		f.Split("\t",str,v);
		if(v.size() != 2) continue;
		int k = dicttopic.TitleClassfy(v.at(0));
		if(k == 6){
			f.WiteLine(std::string(str + "\n").c_str(), fo);
		}		
	}
	fclose(fi);	
	fclose(fo);
	return 0;
}