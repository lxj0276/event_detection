#include<iostream>
#include "../commom/func.h"
#include "../wordseg/seg.h"
int main(int argc, char *argv[]) {
	commom::Func f;
	std::map<std::string, int>bizuindict;
	FILE*fi = fopen(argv[1],"r");
	if (fi == NULL) {
		commom::LOG_INFO("open file error");
		return false;
	}
	char buffer[MAX_LENTH];		
	std::string str = "";
	std::vector<std::string> v;
	while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL){
		str = f.GetLine(buffer); 
		f.Split("\t",str,v);
		if(v.size() != 2) continue;
		bizuindict[v.at(0)] = atoi(v.at(1).c_str());
	}
	fclose(fi);	
	fi = fopen(argv[2],"r");
	FILE*fo = fopen(argv[3],"ab+");
	if ((fi == NULL)||(fo == NULL)) {
		commom::LOG_INFO("open file error");
		return false;
	}	
	while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		str = f.GetLine(buffer); 
		f.Split("\t",str,v);
		std::map<std::string, int>::iterator it = bizuindict.find(v.at(0)); 
		if(it == bizuindict.end()){
			continue;
		}
		f.WiteLine(std::string(v.at(3) + "\t" + f.ConvertToStr(it->second/100) + "\n").c_str(), fo);
	}
	fclose(fi);	
	fclose(fo);
	return 0;
}