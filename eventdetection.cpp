#include "eventdetection.h"
namespace text{
 EventDetc::EventDetc(){}
 EventDetc::~EventDetc(){}
 void EventDetc::Init(const char* path){
	 FILE*fi = fopen(path,"r");
	 if (fi == NULL) {
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
		 if(r.size() != 31)continue;
		 str = r.at(0);
		 int k = 0;
		 for(int i =1; i< r.size(); i++){
			k += atoi(r.at(i).c_str());
		 }
		 k /= 30;
		 worddict[str].eve = k;
		 worddict[str].inc = 0;
		 worddict[str].range = 0;
		 for(int t =0; t< SAVEDATE; t++){
			 worddict[str].pv.push(k);
		 }
	 }
	 commom::DEBUG_INFO("init ok");
	 fclose(fi);
 }
 void EventDetc::WordCountDaily(const char* filein, int k){
	 dailycount.clear();
	 FILE*fi = fopen(filein,"r");
	 if (fi == NULL) {
		 commom::LOG_INFO("open file error");
		 return ;
	 }
	 char buffer[MAX_LENTH];		
	 std::string str = "";
	 std::vector<std::string> r ;
	 // float totalwordsnum = 0.0;
	 // std::vector<std::string> wds;
	 while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		 str = f.GetLine(buffer); 
		 f.Split("\t", str, r);
		 if(r.size() != 31)continue;
		 str = r.at(0);
		 dailycount[str] = atof(r.at(k).c_str());
		 //wds.push_back(str);
		 //totalwordsnum += atof(r.at(k).c_str());
	 }
	 fclose(fi);
	 /*
	 for(std::vector<std::string>::iterator it = wds.begin(); it != wds.end(); it++){
		 dailycount[*it] /= totalwordsnum;
		 commom::DEBUG_INFO(*it);
		 commom::DEBUG_INFO(f.ConvertToStr(dailycount[*it]));
	 }
	 */

 }

 void EventDetc::GetDailyHotWords(){
	 for(daily_dict::iterator it = worddict.begin(); it != worddict.end(); it++){
		 //analysis
		 std::string wd = it->first;
		 int dpv = dailycount[wd];
		 //计算抖动性
		 float alpha = (it->second.range - it->second.inc)/(SAVEDATE + 0.1);
		 //计算热度
		 float beta = (dpv- it->second.eve)/(it->second.eve + alpha);
		 dailyhot[wd] = beta;
		 
		 //updata
		 it->second.pv.pop();
		 it->second.pv.push(dpv);
		 it->second.eve = 0;
		 for(int i =0; i < SAVEDATE; i++){
			 int temp = it->second.pv.front();
			 it->second.eve += temp;
			 it->second.pv.pop();
			 it->second.pv.push(temp);
		 }
		 it->second.eve /= SAVEDATE;
		 it->second.inc = it->second.pv.back() - it->second.pv.front();
		 it->second.range = 0;
		 for(int i =0; i < SAVEDATE; i++){
			 int temp = it->second.pv.front();
			 it->second.range += abs(temp-it->second.eve);
			 it->second.pv.pop();
			 it->second.pv.push(temp);
		 }
	 }
 }	

 void EventDetc::ShowDailyHotWords(const char* filein, const char* outpath){
	 FILE*fo = fopen(outpath,"ab+");
	 for(int i = 1; i < 31; i++){
		 dailyhot.clear();
		 WordCountDaily(filein, i);
		 //commom::DEBUG_INFO("WordCountDaily ok");
		 GetDailyHotWords();
		 //commom::DEBUG_INFO("GetDailyHotWords ok");
		 //sort
		 std::vector<std::pair<std::string,float> > temp;
		 for(std::map<std::string, float>::iterator it =dailyhot.begin(); 
			 it != dailyhot.end(); it++){
			 temp.push_back(*it);
		 }
		 sort(temp.begin(),temp.end(),f.SortBySecondGreater);
		 std::string str = f.ConvertToStr(i) + "day : \n";
		 for(int j =0; j< 20; j++){
			 str += ("\t" + temp.at(j).first);
		 }
		 str += "\n";
		 f.WiteLine(str.c_str(), fo);
	 }
  }
 void EventDetc::Event(){
	 //count
	 std::map<std::string, int> strint;
	 std::map<int, std::string> intstr;
	 int i = 0;
	 for(std::map<std::string, float>::iterator it = dailycount.begin(); it != dailycount.end(); it++){
		 strint[it->first] = i++;
		 intstr[i-1] = it->first;
	 }
	 distance = new float*[i];
	 for(int j =0; j < i; j++){
		 distance[j] = new float[i];
	 }
	 //
	 std::vector<std::string >  temp;
	 temp.push_back(intstr[0]);
	 hotevent.push_back(temp);
	 for(int j = 1; j < i; j++){


	 }



 }

 float EventDetc::CountRo(std::string& stra, std::string& strb){
	 for(int i =0; i < SAVEDATE; i++){
		 int temp = it->second.pv.front();
		 it->second.range += abs(temp-it->second.eve);
		 it->second.pv.pop();
		 it->second.pv.push(temp);
	 }




	 float cxy



 }
  


}