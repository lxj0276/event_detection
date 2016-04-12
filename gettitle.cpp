#include<iostream>
#include "../commom/func.h"
#include "../wordseg/seg.h"
int main(int argc, char *argv[]) {
	commom::Func f;
	FILE*fi = fopen(argv[1],"r");
	FILE*fo = fopen(argv[2],"ab+");
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
		//commom::DEBUG_INFO(f.ConvertToStr(v.size()));	
		//commom::DEBUG_INFO(v.at(3).c_str());
		f.WiteLine(std::string(v.at(3) + "\n").c_str(), fo);
	}
	fclose(fi);	
	fclose(fo);
	return 0;
}