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
			 f.Brackets(str);
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
	 dailyhotarticle.clear();
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
		 if(dailyhotarticle.size() < HOTARTICLE){
			 //test
			 dailyhotarticle.push_back(std::pair<string, float>(str, k));
			 sort(dailyhotarticle.begin(), dailyhotarticle.end(), f.SortBySecondLess);
		 }else{
			 if(k > dailyhotarticle.at(0).second){
				dailyhotarticle.at(0).first = str;
				dailyhotarticle.at(0).second = k;
				sort(dailyhotarticle.begin(), dailyhotarticle.end(), f.SortBySecondLess);
			 }
		 }
		 f.Brackets(str);
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
		 eventsres.clear();
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
		 Event(root,eventsres);
		 string hotstr = "";

		 FindhotArticle(hotstr);
		 commom::DEBUG_INFO(hotstr);
		 FreeTree();
		 commom::DEBUG_INFO("free ok");
		 f.WiteLine(str.c_str(), fo);
		 commom::DEBUG_INFO("wrrite ok");
	 }
  }
  bool Compare(const std::pair<int, float>& x, 
	 const std::pair<int, float>& y ){
		 return x.second > y.second;
  }
  void EventDetc::Event(tree_node* root, std::map<std::string, float>& eventsres){
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
	 float _MIN = 100000000;
	 for(int j = 0; j< eventlist.size(); j++){
		 float score = 1;
		 string sstr = "";
		 for(int k =0; k<eventlist.at(j).size(); k++){
			 sstr += (eventlist.at(j).at(k).first + " ");
			 score *= eventlist.at(j).at(k).second;
		     if(k == 2){
				 break;
			 }
		 }
		 score /= _MIN;
		 score /= _MIN;
		 if(score >= 1){
			 eventsres[sstr] = score;
			 sstr += f.ConvertToStr(score);
			 std::cout<<sstr<<std::endl;
		 }
	 }	
  }
  void EventDetc::FindhotArticle(string& str){
	  string article = "";
	  float score = 0.0;
	  std::map<string, int> r;
	  std::vector<string> v;
	  if(dailyhotarticle.size() == 0){
		  commom::DEBUG_INFO("no hot article");
	  }
	  for(std::vector<std::pair<string, float> >::iterator it = dailyhotarticle.begin();
		  it != dailyhotarticle.end(); it++){
		  //commom::DEBUG_INFO(it->first);
		  float _score = 0.0;
		  str = it->first;
		  f.Brackets(str);
		  f.Split(" ",mseg.Segement(str.c_str()),r);
		  std::map<string, float>dict;
		  for(std::map<std::string, float>::iterator bt =  eventsres.begin(); bt != eventsres.end(); bt++){
			  f.Split(" ",bt->first,v);
			  for(int i = 0; i < v.size(); i++){
				  //取最热两个词
				  if(r[v.at(i)] > 0){
					  dict[v.at(i)] += bt->second;		
				  }				  		
			  }
		  }
		  //取dict 里值最大的两
		  std::vector<std::pair<string, float> >temp;
		  for(std::map<string, float>::iterator tt = dict.begin(); tt != dict.end(); tt++){
			  temp.push_back(*tt);
		  }
		  sort(temp.begin(), temp.end(), f.SortBySecondGreater);
		  for(int q = 0; q < temp.size(); q++){
			  if(q == 2){
				  break;
			  }
			  _score += (it->second)*temp.at(q).second;
		  }
		  //commom::DEBUG_INFO(f.ConvertToStr(_score));
		  if(_score > score){
			  article = it->first;
			  score = _score;
		  }		 
	  }
	  str = article;
  }

  /*
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
  */
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
 