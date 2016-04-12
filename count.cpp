#include "count.h"
namespace text{
 Count::Count(){}
 Count::~Count(){}
 void Count::Init(const char* path){
	 std::string dictpath = path;
	 dictpath += "seg/";
	 mseg.InitDict(dictpath.c_str());
	 keywd.Init(path);
 }
 void Count::WordCount(const char* filein, const char* fileout){
   //关键词抽取
	std::map<std::string, int> dict;
	commom::Func f;
	FILE*fi = fopen(filein,"r");
	FILE*fo = fopen(fileout,"ab+");
	if ((fi == NULL)||(fo == NULL)) {
		commom::LOG_INFO("open file error");
		return ;
	}
   //关键词词频统计
	char buffer[MAX_LENTH];		
	std::string str = "";
	std::map<std::string, int> v;
	std::vector<std::pair<std::string,double> > ret;
	while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		str = f.GetLine(buffer); 
		f.Split(" ",mseg.Segement(str.c_str()),v);
		keywd.IdfKeyWords(ret,v, KEYWORDSNUM);
		for(std::vector<std::pair<std::string,double> >::iterator it = ret.begin(); 
			it != ret.end(); it++){
			dict[it->first]++;
		}		
	 }
	 fclose(fi);
	 commom::DEBUG_INFO("count ok");
	 std::vector<std::pair<std::string,int> > temp;
	 for(std::map<std::string, int>::iterator bt = dict.begin(); bt!= dict.end(); bt++){
		 temp.push_back(*bt);
	 }
	 sort(temp.begin(), temp.end(),f.SortBySecondGreater);
	 for(std::vector<std::pair<std::string,int> >::iterator bt = temp.begin(); bt!= temp.end(); bt++){
		 f.WiteLine((bt->first + f.ConvertToStr(bt->second)).c_str(), fo);
	 }
	 fclose(fo);
 }
}