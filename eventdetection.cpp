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
			 worddict[str].pv[t] = 10;
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
	 while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		 str = f.GetLine(buffer); 
		 f.Split("\t", str, r);
		 if(r.size() != 31)continue;
		 str = r.at(0);
		 dailycount[str] = atof(r.at(k).c_str());
	 }
	 fclose(fi);
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
		 for(int i =0; i < SAVEDATE-1; i++){
			 it->second.pv[i] = it->second.pv[i+1];
		 }
		  it->second.pv[SAVEDATE-1] = dpv;
		 commom::DEBUG_INFO(f.ConvertToStr(dpv));
		 it->second.eve = 0;
		 for(int i =0; i < SAVEDATE; i++){
			 it->second.eve += it->second.pv[i];
		 }
		 it->second.eve /= SAVEDATE;
		 it->second.inc = it->second.pv[SAVEDATE-1] - it->second.pv[0];
		 it->second.range = 0;
		 for(int i =0; i < SAVEDATE; i++){
			 it->second.range += abs(it->second.pv[i]-it->second.eve);
		 }
		 std::cout<<"test*********************"<<std::endl;
		 std::cout<<it->first<<std::endl;
		 for(int i =0; i < SAVEDATE; i++){
			 std::cout<<it->second.pv[i]<<"  ";
		 }
		 std::cout<<std::endl;
	 }
 }	

 void EventDetc::ShowDailyHotWords(const char* filein, const char* outpath){
	 FILE*fo = fopen(outpath,"ab+");
	 for(int i = 1; i < 31; i++){
		 dailyhot.clear();
		 WordCountDaily(filein, i);//得到每日词频统计
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
		 for(int j =0; j< HOTWORDSNUM; j++){
			 str += ("\t" + temp.at(j).first);
		 }
		 str += "\n";
		 dailyhot.clear();
		 for(int j =0; j< HOTWORDSNUM; j++){
			 dailyhot[temp.at(j).first] = temp.at(j).second; 
		 }
		 commom::DEBUG_INFO("size" + f.ConvertToStr(dailyhot.size()));
		 Event();
		 for(std::vector<std::vector<std::string> >::iterator bt = hotevent.begin();
			 bt != hotevent.end(); bt++){
			if((*bt).size() >= 2){
				str += "event:\n";
				for(std::vector<std::string>::iterator tt = (*bt).begin(); tt != (*bt).end(); tt++){
					str += (*tt + " ");
				}
				str += "\n";
			}
		 }
		 f.WiteLine(str.c_str(), fo);
	 }
  }
 bool Compare(const std::pair<int, float>& x, 
	 const std::pair<int, float>& y ){
		 return x.second > y.second;
 }
 void EventDetc::Event(){
	 hotevent.clear();
	 //count
	 std::map<std::string, int> strint;
	 std::map<int, std::string> intstr;
	 int i = 0;
	 for(std::map<std::string, float>::iterator it = dailyhot.begin(); it != dailyhot.end(); it++){
		 strint[it->first] = i++;
		 intstr[i-1] = it->first;
	 }
	 distance = new float*[i];
	 for(int j =0; j < i; j++){
		 distance[j] = new float[i];
	 }
	 //init 
	 for(int pi = 0; pi < i; ++pi){
		 for(int pj = pi; pj < i; ++pj){
			 distance[pi][pj] = CountRo(intstr[pi], intstr[pj]);
			 distance[pj][pi] =  distance[pi][pj];
		 }
	 }
	 for(int pi = 0; pi < i; ++pi){
		 for(int pj = 0; pj < i; ++pj){
			 std::cout<<distance[pi][pj]<<"  ";
		 }
		 std::cout<<std::endl;
	 }
	 std::vector<std::string >  temp;
	 temp.push_back(intstr[0]);
	 hotevent.push_back(temp);
	 for(int j = 1; j < i; j++){
		 temp.clear();
		 //寻找距离最近的社区
		 std::vector<std::pair<int, float> >dissort;
		 int index = 0;
		 for(std::vector<std::vector<std::string> >::iterator itt = hotevent.begin(); 
			 itt != hotevent.end(); itt++){
			//count distance
			float dis = 0.0;
			int num = 0;
			for(std::vector<std::string>::iterator ib = (*itt).begin(); ib != (*itt).end(); ib++){
				dis += distance[strint[*ib]][j];
				num++;
			}
			dis /= num;
			dissort.push_back(std::pair<int, float>(index++, dis));
			//commom::DEBUG_INFO(f.ConvertToStr(dis));
		 }
		 //sort
		 sort(dissort.begin(), dissort.end(), Compare);
		 if(dissort.at(0).second < ALPHA){
			 temp.push_back(intstr[j]);
			 hotevent.push_back(temp);
		 }else{
			 hotevent.at(dissort.at(0).first).push_back(intstr[j]);
		 }
	 }
	 commom::DEBUG_INFO(f.ConvertToStr(hotevent.size()));
 }

 float EventDetc::CountRo(std::string& stra, std::string& strb){
	 float cgmxy = 0.0;
	 float cgmx = 0.0;
	 float cgmy = 0.0;
	 float cgmx2 = 0.0;
	 float cgmy2 = 0.0;
	 std::string as = "";
	 std::string bs = "";
	 for(int i =0; i < SAVEDATE; i++){
		 int tempa = worddict[stra].pv[i];
		 int tempb = worddict[strb].pv[i];
		 as +=(f.ConvertToStr(tempa) + " ");
		 bs +=(f.ConvertToStr(tempb) + " ");
		 cgmx += tempa;
		 cgmy += tempb;
		 cgmxy += tempa*tempb;
		 cgmx2 += tempa*tempa;
		 cgmy2 += tempb*tempb;
	 }
	 float ro = SAVEDATE*cgmxy - cgmx*cgmy;
	 ro /= sqrt((SAVEDATE*cgmx2 - pow(cgmx,2))*(SAVEDATE*cgmy2 - pow(cgmy,2)));

	 commom::DEBUG_INFO(stra + " " + strb + ":" + f.ConvertToStr(ro));
	 commom::DEBUG_INFO(as);
	 commom::DEBUG_INFO(bs);
	 return ro;
  }
}