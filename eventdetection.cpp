#include "eventdetection.h"
namespace text{
  EventDetc::EventDetc(){
	 root = NULL;
	 //root = new tree_node;
 }
  EventDetc::~EventDetc(){
	 if( root != NULL){
		 FreeTree();	
	 }
 }
 //初始化，20160207 - 20160213
  void EventDetc::Init(const char* dictpath, const char* path, const char* stoppath){
	 mseg.InitDict(dictpath);
	 for(int i =0; i< SAVEDATE; ++i){
		 string filepath = path + f.ConvertToStr(BEGINDATA + i);
		 FILE*fi = fopen(filepath.c_str(),"r");
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
			 if(r.size() != 8)continue;
			 str = r.at(4);
			 int k = atoi(r.at(5).c_str());
			 f.Split(" ",mseg.Segement(str.c_str()),r);
			 Erase(r);
			 //commom::DEBUG_INFO(mseg.Segement(str.c_str()));
			 for(int j =0; j< r.size(); j++){
				 if((worddict.find(r.at(j)) == worddict.end())&&(i)){
					for(int t = 0; t < i; t++){
						worddict[r.at(j)].pv[t] = 1;
					}
					worddict[r.at(j)].pv[i] += k;
				 }else{
					 worddict[r.at(j)].pv[i] += k;
				 }
			 }
		 }
		 fclose(fi);
	 }

	 for(daily_dict:: iterator it = worddict.begin(); it != worddict.end(); it++){
		 std::string wd = it->first;
		 //eve
		 it->second.eve = 0;
		 for(int i =0; i < SAVEDATE; i++){
			 it->second.eve += it->second.pv[i];
		 }
		 it->second.eve /= SAVEDATE;
		 //range
		 it->second.range = 0;
		 for(int i =0; i < SAVEDATE; i++){
			 it->second.range += abs(it->second.pv[i]-it->second.eve);
		 }
		 //inc
		 it->second.inc = it->second.pv[SAVEDATE-1] - it->second.pv[0];
	 }
	 //load stop words
	 FILE*fi = fopen(stoppath,"r");
	 if (fi == NULL) {
		 commom::LOG_INFO("open file error");
		 return ;
	 }
	 std::string str = "";
	 std::vector<std::string> r ;
	 char buffer[MAX_LENTH];	
	 while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		 str = f.GetLine(buffer); 
		 stopwd[str]++;
	 }

 }
  void EventDetc::Erase(std::vector<std::string>& r){
	 for(std::vector<std::string>::iterator it = r.begin(); it != r.end();){
		if(stopwd.find(*it) != stopwd.end()){
			it = r.erase(it);
		}else{
			it++;
		}
	 }
  }
  void EventDetc::WordCountDaily(const char* filepath, int k){
	 dailycount.clear();
	 root = new tree_node;
	 string filein = filepath + f.ConvertToStr(BEGINDATA + k);
	 FILE*fi = fopen(filein.c_str(),"r");
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
		 if(r.size() != 8)continue;
		 str = r.at(4);
		 k = atoi(r.at(5).c_str());
		 f.Split(" ",mseg.Segement(str.c_str()),r);
		 Erase(r);
		 for(int j =0; j< r.size(); j++){
			 dailycount[r.at(j)] += k;
		 }	
		 //insert tree
		 Insert(r,k);
	 }
	 fclose(fi);
 }

  void EventDetc::GetDailyHotWords(){
	 for(daily_dict::iterator it = worddict.begin(); it != worddict.end(); it++){
		 //analysis
		 std::string wd = it->first;
		 int dpv = dailycount[wd];
		 //TODO 前面为0
		 //计算抖动性
		 float alpha = (it->second.range - it->second.inc)/(SAVEDATE + 0.1);
		 //计算热度
		 float beta = (dpv- it->second.eve)/(1 + it->second.eve + alpha);
		 if(beta > 20){
			 beta = 20;
		 }
		 beta *= dpv;
		 dailyhot[wd] = beta;
		 
		 //updata
		 for(int i =0; i < SAVEDATE-1; i++){
			 it->second.pv[i] = it->second.pv[i+1];
		 }
		  it->second.pv[SAVEDATE-1] = dpv;
		 //commom::DEBUG_INFO(f.ConvertToStr(dpv));
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
	 }
 }	

  void EventDetc::ShowDailyHotWords(const char* filein, const char* outpath){
	 FILE*fo = fopen(outpath,"ab+");
	 for(int i = 7; i < 23; i++){
		 dailyhot.clear();
		 dailyhotbig.clear();
		 WordCountDaily(filein, i);//得到每日词频统计
		 GetDailyHotWords();
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
		 for(int j =0; j< HOTWORDSNUMBIG; j++){
			 dailyhotbig[temp.at(j).first] = temp.at(j).second; 
		 }
		 Event(root);
		 FreeTree();
		 commom::DEBUG_INFO("free ok");
		 //commom::DEBUG_INFO("size" + f.ConvertToStr(dailyhot.size()));
		 /*
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
		 */
		 f.WiteLine(str.c_str(), fo);
		 commom::DEBUG_INFO("wrrite ok");
	 }
  }
  bool Compare(const std::pair<int, float>& x, 
	 const std::pair<int, float>& y ){
		 return x.second > y.second;
 }
  void EventDetc::Event(tree_node* root){
	 //事件发现
	 std::vector< std::vector<std::pair<string, size_t> > > eventlist;
	 for(std::map<std::string, float>::iterator it = dailyhot.begin(); it != dailyhot.end(); it++){
		 //遍历开始词
		 tree_node* p = root;
		 bool flag = true;
		 std::vector<std::pair<string, size_t> > v;
		 if(p->childlist.find(it->first) != p->childlist.end()){
			 p = p->childlist[it->first];
			 v.push_back(std::pair<string, size_t>(it->first, p->freq));
			 //第一个词
			 while(flag){//循环寻找
				 flag = false;
				 size_t num = 0;
				 string str = "";
				 for(std::map<string, tree_node* >:: iterator bt = p->childlist.begin();
					 bt != p->childlist.end(); bt++){
					 if(dailyhotbig.find(bt->first) != dailyhotbig.end()){
						 flag = true;
						 if(bt->second->freq > num){
							 num = bt->second->freq;
							 str = bt->first;
						 }
					 }
				 }
				 if(flag){
					v.push_back(std::pair<string, size_t>(str, num));
					p = p->childlist[str];
				 }
			 }
			 eventlist.push_back(v);
		 }
	 }
	 for(int j = 0; j< eventlist.size(); j++){
		 float score = 1;
		 for(int k =0; k<eventlist.at(j).size(); k++){
			 std::cout<<eventlist.at(j).at(k).first<<" ";
			 score *= eventlist.at(j).at(k).second;
		     if(k == 2){
				 break;
			 }
		 }
		 std::cout<<score<<"  ";
		 std::cout<<std::endl;
	 }

	 /*
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
	 */
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

  bool EventDetc::Insert(std::vector<std::string>& r, int k){
	 for(int i =0; i< r.size(); i++){
	   tree_node* p =root; 
	   for(int j =i; j< r.size(); j++){
		 if(p->childlist.end() == p->childlist.find(r.at(j))){
			 tree_node * next = NULL;	
			 next = new tree_node;	
			 if(NULL == next){
			   return false; 
			 } 
			 p->childlist[r.at(j)] = next;	 
			 p = next;	 
			 p->freq += k;
		}else{ 
			 p = p->childlist[r.at(j)]; 
			 p->freq += k;
		}
	 }
	}
	return true;
  }
  /*删除*/
  bool EventDetc::DeleteNode(tree_node* node){ 
	  nodemap::iterator it; 
	  if (node->childlist.size()>0){	 
		  for ( it  = node->childlist.begin(); 
			  it  != node->childlist.end();  it++)	{ 
				  DeleteNode(it->second); 
		  }
		  if(root!=node){
			  node->childlist.clear(); 
			  delete node; 
		  } 
	  }
	  return true;
  }

  void EventDetc::FreeTree() { 		
	  if (!DeleteNode(root)){ 
		  commom::LOG_INFO("DeleteNode failed!");	 
	  }
	  root = NULL; 
  }	
}
 