#include "../commom/func.h"
typedef FILE* _FILE;
int main(int argc, char *argv[]){
	commom::Func f;
	std::map<std::string, int> dict;
	std::map<std::string, int> index;
	FILE* fi = fopen(argv[1],"r");
	if (fi == NULL) {
		commom::LOG_INFO("open file error");
		return 0;
	}
	//关键词词频统计
	char buffer[MAX_LENTH];		
	std::string str = "";
	std::vector<std::string> r ;
	while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		str = f.GetLine(buffer); 
		f.Split("\t", str, r);
		if(r.size() != 8)continue;
		str = r.at(7);
		dict[str]++;
	}
	fclose(fi);
	_FILE* fo = new _FILE[dict.size()];
	int i =0;
	for(std::map<std::string, int>::iterator it = dict.begin(); it != dict.end(); it++){
		std::string path = argv[2] + it->first;
		fo[i++] = fopen(path.c_str(),"ab+");
		index[it->first] = i -1;
	}

	//read again
	fi = fopen(argv[1],"r");
	if(fi == NULL) {
		commom::LOG_INFO("open file error");
		return 0;
	}
	//关键词词频统计
	while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		str = f.GetLine(buffer); 
		f.Split("\t", str, r);
		if(r.size() != 8)continue;
		f.WiteLine((str + "\n").c_str(), fo[index[r.at(7)]]);
	}
	fclose(fi);
	for(int j =0; j < dict.size(); j++){
		fclose(fo[j]);
	}
	return 0;
}