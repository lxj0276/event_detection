#include<iostream>
#include "../commom/func.h"
#include "../wordseg/seg.h"
int main(int argc, char *argv[]) {
	commom::Func f;
	FILE*fi = fopen(argv[1],"r");
	FILE*fo = fopen(argv[2],"ab+");
	FILE*foo = fopen(argv[3],"ab+");
	if ((fi == NULL)||(fo == NULL)) {
		commom::LOG_INFO("open file error");
		return false;
	}
	char buffer[MAX_LENTH];		
	std::string str = "";
	std::vector<std::string> v;
	int num = std::atoi(argv[4]);
	while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL){
		str = f.GetLine(buffer); 
		f.Split("\t",str,v);
		if(v.size() < 2 )continue;
		if(std::atoi(v.at(1).c_str()) > num){
			f.WiteLine((str + "\n").c_str(), fo);
		}else{
			f.WiteLine((str + "\n").c_str(), foo);
		}		
	}
	fclose(fi);	
	fclose(fo);
	fclose(foo);
	return 0;
}