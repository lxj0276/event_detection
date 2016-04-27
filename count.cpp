#include "count.h"
namespace text{
 Count::Count(){}
 Count::~Count(){}
 void Count::Init(const char* path){
	 std::string dictpath = path;
	 dictpath += "seg/";
	 mseg.InitDict(dictpath.c_str());
	 std::string idfpath = path;
	 idfpath += "vector/idf";
	 keywd.Init(idfpath.c_str());
	 commom::DEBUG_INFO("init ok");
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
	std::vector<std::string> r ;
	std::vector<std::pair<std::string,double> > ret;
	while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		str = f.GetLine(buffer); 
		f.Split("\t", str, r);
		if(r.size() != 8)continue;
		str = r.at(0);
		int k = atoi(r.at(5).c_str());
		f.Split(" ",mseg.Segement(str.c_str()),v);
		keywd.IdfKeyWords(ret,v, KEYWORDSNUM);
		for(std::vector<std::pair<std::string,double> >::iterator it = ret.begin(); 
			it != ret.end(); it++){
			dict[it->first] += k;
			//dict[it->first] += 1;
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
		 f.WiteLine((bt->first + "\t" + f.ConvertToStr(bt->second) + "\n").c_str(), fo);
	 }
	 fclose(fo);
 }
 void Count::WordCountDaily(const char* filein, const char* fileout){
	 //定义数据结构
	 daily_dict m_dict;
	 std::map<std::string, int>dailylist;
	 std::map<std::string, int> dict;
	 commom::Func f;
	 FILE*fi = fopen(filein,"r");
	 FILE*fo = fopen(fileout,"ab+");
	 if ((fi == NULL)||(fo == NULL)) {
		 commom::LOG_INFO("open file error");
		 return ;
	 }
	 char buffer[MAX_LENTH];		
	 std::string str = "";
	 std::map<std::string, int> v;
	 std::vector<std::string> r ;
	 std::vector<std::pair<std::string,double> > ret;
	 while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		 str = f.GetLine(buffer); 
		 f.Split("\t", str, r);
		 if(r.size() != 8)continue;
		 str = r.at(4);
		 std::string date = r.at(7);
		 dict[date]++;
		 int k = atoi(r.at(5).c_str())/10000;
		 //commom::DEBUG_INFO(mseg.Segement(str.c_str()));
		 f.Split(" ",mseg.Segement(str.c_str()),v);
		 //commom::DEBUG_INFO(f.ConvertToStr(v.size()));
		 for(std::map<std::string, int>::iterator it = v.begin(); it!= v.end(); it++){
			 m_dict[it->first][date] += k;
		 }
		  /*
		 keywd.IdfKeyWords(ret,v, KEYWORDSNUM);
		 for(std::vector<std::pair<std::string,double> >::iterator it = ret.begin(); 
			 it != ret.end(); it++){
				 //dict[it->first] += k;
				 dict[it->first] += 1;
		 }		
		 */
	 }
	 fclose(fi);
	 commom::DEBUG_INFO("count ok");
	 commom::DEBUG_INFO(f.ConvertToStr(m_dict.size()));
	 /*
	 std::vector<std::pair<std::string,int> > temp;
	 for(std::map<std::string, int>::iterator bt = dict.begin(); bt!= dict.end(); bt++){
		 temp.push_back(*bt);
	 }
	 sort(temp.begin(), temp.end(),f.SortBySecondGreater);
	
	 for(std::vector<std::pair<std::string,int> >::iterator bt = temp.begin(); bt!= temp.end(); bt++){
		 f.WiteLine((bt->first + "\t" + f.ConvertToStr(bt->second) + "\n").c_str(), fo);
	 }
	 */
	 for(daily_dict::iterator it =m_dict.begin(); it != m_dict.end(); it++){
		 std::string linestr = it->first;
		 int totalnum = 0;
		 for(std::map<std::string, int>::iterator bt = dict.begin(); bt != dict.end(); bt++){
			linestr += ("\t" + f.ConvertToStr(m_dict[it->first][bt->first]));
			totalnum += m_dict[it->first][bt->first];
		 }
		 linestr += "\n";
		 if(totalnum > 100){
			f.WiteLine(linestr.c_str(), fo);
		 }		
	 }
	 fclose(fo);
 }
 void Count::SentenceCount(const char* filein, const char* fileout){
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
	 std::vector<std::string> r ;
	 std::vector<std::pair<std::string,double> > ret;
	 while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		 str = f.GetLine(buffer); 
		 f.Split("\t", str, r);
		 if(r.size() != 2)continue;
		 str = r.at(0);
		 int k = atoi(r.at(1).c_str())/100;
		 f.Split(" ",mseg.Segement(str.c_str()),v);
		 keywd.IdfKeyWords(ret,v, KEYWORDSNUM-1);
		 str = "";
		 for(std::vector<std::pair<std::string,double> >::iterator it = ret.begin(); 
			 it != ret.end(); it++){
			 str +=  it->first;
		 }		
		 //dict[str] += k;
		 dict[str] += 1;
	 }
	 fclose(fi);
	 commom::DEBUG_INFO("count ok");
	 std::vector<std::pair<std::string,int> > temp;
	 for(std::map<std::string, int>::iterator bt = dict.begin(); bt!= dict.end(); bt++){
		 temp.push_back(*bt);
	 }
	 sort(temp.begin(), temp.end(),f.SortBySecondGreater);
	 for(std::vector<std::pair<std::string,int> >::iterator bt = temp.begin(); bt!= temp.end(); bt++){
		 f.WiteLine((bt->first + "\t" + f.ConvertToStr(bt->second) + "\n").c_str(), fo);
	 }
	 fclose(fo);
 }
}