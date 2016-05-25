#include "eventdetection.h"
namespace text{
  EventDetc::EventDetc(){
	 root = NULL;
 }
  EventDetc::~EventDetc(){
	 if( root != NULL){
		 FreeTree();	
	 }
 }

  void EventDetc::Init(const char* dictpath, const char* path, const char* stoppath){
	 mseg.InitDict(dictpath);
	 for(int i =0; i< SAVEDATE; ++i){
		 string filepath = path + f.ConvertToStr(BEGINDATA + i);
		 FILE*fi = fopen(filepath.c_str(),"r");
		 if (fi == NULL) {
			 commom::LOG_INFO("open file error" + filepath);
			 return ;
		 }
		 char buffer[MAX_LENTH];		
		 std::string str = "";
		 std::map<std::string, double> v;
		 std::vector<std::string> r ;
		 std::vector<std::pair<std::string,double> > ret;
		 while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
			 str = f.GetLine(buffer); 
			 f.Split("\t", str, r);
			 if(r.size() != 8)continue;
			 str = r.at(4);
			 double k = atof(r.at(5).c_str())/10000.01;
			 if(k <= 0 )continue;
			 f.Brackets(str);
			 f.Split(" ",mseg.Segement(str.c_str()),r);
			 Erase(r);
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
		 commom::LOG_INFO("open file error" + string(stoppath));
		 return ;
	 }
	 std::string str = "";
	 std::vector<std::string> r ;
	 char buffer[MAX_LENTH];	
	 while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		 str = f.GetLine(buffer); 
		 stopwd[str]++;
	 }
	 commom::DEBUG_INFO("INIT_SECUSS");
 }
  void EventDetc::Erase(std::vector<std::string>& r){
	 for(std::vector<std::string>::iterator it = r.begin(); it != r.end();){
		if(stopwd.find(*it) != stopwd.end()){
			it = r.erase(it);
		}else if((*it).size() < 4){
			it = r.erase(it);
		}else{
			it++;
		}
	 }
  }
  bool comp(const articleinfo& x, const articleinfo& y){
	  return x.score > y.score;
  }

  void EventDetc::WordCountDaily(const char* filepath, int d){
	 //将每日统计数据写入本地文件
	 dailycount.clear();
	 dailyhotarticle.clear();
	 root = new tree_node;
	 string filein = filepath + f.ConvertToStr(BEGINDATA + d);
	 FILE*fi = fopen(filein.c_str(),"r");
	 string logpath = LOGPATH + f.ConvertToStr(BEGINDATA + d) + "word";
	 FILE*fo = fopen(logpath.c_str(),"ab+");
	 if (fi == NULL) {
		 commom::LOG_INFO("open file error" + logpath);
		 return ;
	 }
	 char buffer[MAX_LENTH];		
	 std::string str = "";
	 std::vector<std::string> r ;
	 articleinfo tmp;
	 while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL)	{
		 str = f.GetLine(buffer); 
		 f.Split("\t", str, r);
		 
		 if(r.size() != 8)continue;
		 tmp.url = r.at(6);
		 str = r.at(4);
		 double k = atof(r.at(5).c_str())/10000.01;
		 tmp.str = str;
		 tmp.score = k;
		 
		 if(k <= 0 )continue;
		 if(dailyhotarticle.size() < HOTARTICLE){
			 dailyhotarticle.push_back(tmp);
			 sort(dailyhotarticle.begin(), dailyhotarticle.end(), comp);
		 }else{
			 if(k > dailyhotarticle.at(0).score){
				dailyhotarticle.at(0).str = tmp.str;
				dailyhotarticle.at(0).score = tmp.score;
				dailyhotarticle.at(0).url = tmp.url;
				sort(dailyhotarticle.begin(), dailyhotarticle.end(), comp);
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
	 //统计每日词频
	 str = "";
	 for(std::map<std::string, double>::iterator it = dailycount.begin(); it != dailycount.end(); it++){
		 if(it->second < 1){
			 continue;
		 }
		 for(double x = 0; x < it->second; x++){
			 str += (it->first + " ");
		 }
		 str += "\n";
		 f.WiteLine(str.c_str(), fo);
		 str = "";
	 }
	 fclose(fo);
 }

  void EventDetc::GetDailyHotWords(int x){
	 for(daily_dict::iterator it = worddict.begin(); it != worddict.end(); it++){
		 //analysis
		 std::string wd = it->first;
		 double dpv = dailycount[wd];
		 //TODO 前面为0
		 //计算抖动性
		 double alpha = (it->second.range - it->second.inc)/(SAVEDATE + 0.1);
		 //计算热度
		 double beta = (dpv- it->second.eve)/(1 + it->second.eve + alpha);
		 if(beta > 50){
			 beta = 50;
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
	 //log 每日hot词 
	 string logpath = LOGPATH + f.ConvertToStr(BEGINDATA + x) + "hot";
	 FILE*fo = fopen(logpath.c_str(),"ab+");
	 string str = "";
	 for(std::map<std::string, double>::iterator it = dailyhot.begin(); it != dailyhot.end(); it++){
		 if(it->second < 1){
			 continue;
		 }
		 for(double x = 0; x < (it->second); x++){
			 str += (it->first + " ");
		 }
		 str += "\n";
		 f.WiteLine(str.c_str(), fo);
		 str = "";
	 }
	 fclose(fo);

 }	

  void EventDetc::ShowDailyHotWords(const char* filein, const char* outpath){
	 FILE*fo = fopen(outpath,"ab+");
	 for(int i = 7; i < 29; i++){
		 dailyhot.clear();
		 dailyhotbig.clear();
		 eventsres.clear();
		 WordCountDaily(filein, i);//得到每日词频统计
		 GetDailyHotWords(i);
		 //sort
		 std::vector<std::pair<std::string,float> > temp;
		 for(std::map<std::string, double>::iterator it =dailyhot.begin(); 
			 it != dailyhot.end(); it++){
			 temp.push_back(*it);
		 }
		 sort(temp.begin(),temp.end(),f.SortBySecondGreater);
		 std::string str = f.ConvertToStr(i + 20160201) + "\n";
		 str += "keywords: ";
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
		 str += "EVENT	:";
		 std::vector<std::pair<string,float> >tmp;
		 for(std::map<std::string, double>::iterator it = eventsres.begin(); it != eventsres.end(); it++){
			 //str += (it->first + " " + f.ConvertToStr(it->second) + "\t");
			 tmp.push_back(*it);
		 }
		 sort(tmp.begin(), tmp.end(),f.SortBySecondGreater);
		 for(int l = 0; l < tmp.size(); l++){
			 str += (tmp.at(l).first + "\001" + f.ConvertToStr(tmp.at(l).second) + "\t");
		 }
		 str += "\n";
		 std::vector<articleinfo>arts;
		 FindhotArticle(arts,hotstr);
		 sort(arts.begin(), arts.end(),comp);
		 str += "ARTICLE :";
		 int num = arts.size() < 10 ? arts.size():10;
		 for(int jj = 0; jj < num; jj++){
			 str += (arts.at(jj).str + " " + f.ConvertToStr(arts.at(jj).score) + " " + arts.at(jj).url + "\t");
		 }
		 //str += hotstr;
		 str += "\n";
		 FreeTree();
		 f.WiteLine(str.c_str(), fo);
	 }
  }

  //TODO 热度分级，
  void EventDetc::Event(tree_node* root, std::map<std::string, double>& eventsres){
	 //事件发现
	 std::vector< std::vector<std::pair<string, double> > > eventlist;
	 for(std::map<std::string, double>::iterator it = dailyhot.begin(); it != dailyhot.end(); it++){
		 //遍历开始词
		 tree_node* p = root;
		 bool flag = true;
		 std::vector<std::pair<string, double> > v;
		 if(p->childlist.find(it->first) != p->childlist.end()){
			 p = p->childlist[it->first];
			 //commom::DEBUG_INFO(it->first + f.ConvertToStr(p->freq));
			 v.push_back(std::pair<string, double>(it->first, p->freq));
			 //第一个词
			 while(flag){//循环寻找
				 flag = false;
				 double num = 0;
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
					v.push_back(std::pair<string, double>(str, num));
					p = p->childlist[str];
				 }
			 }
			 eventlist.push_back(v);
		   }
	 }
	 commom::DEBUG_INFO(f.ConvertToStr(eventlist.size()));
	 for(int j = 0; j< eventlist.size(); j++){
		 double score = 0;
		 string sstr = "";
		 for(double k =0; k<eventlist.at(j).size(); k++){
			 sstr += (eventlist.at(j).at(k).first + " ");
			 score += (eventlist.at(j).at(k).second);
		     if(k == 2){
				 break;
			 }
		 }
		 eventsres[sstr] = score;
		 sstr += f.ConvertToStr(score);
	 }	
  }
  void EventDetc::FindhotArticle(std::vector<articleinfo>& arts,string& str){
	  string article = "";
	  double score = 0.0;
	  std::map<string, int> r;
	  std::vector<string> v;
	  if(dailyhotarticle.size() == 0){
		  commom::DEBUG_INFO("no hot article");
	  }
	  articleinfo tp;
	  for(std::vector<articleinfo>::iterator it = dailyhotarticle.begin();
				  it != dailyhotarticle.end(); it++){
		  double _score = 0.0;
		  str = it->str;
		  f.Brackets(str);
		  f.Split(" ",mseg.Segement(str.c_str()),r);
		  std::map<string, double>dict;
		  for(std::map<std::string, double>::iterator bt =  eventsres.begin(); bt != eventsres.end(); bt++){
			  f.Split(" ",bt->first,v);
			  if(v.size() < 2) continue;
			  for(int i = 0; i < v.size(); i++){
				  if(r[v.at(i)] > 0){
					  dict[v.at(i)] += bt->second;		
				  }				  		
			  }
		  }
		  std::vector<std::pair<string, float> >temp;
		  for(std::map<string, double>::iterator tt = dict.begin(); tt != dict.end(); tt++){
			  temp.push_back(*tt);
		  }
		  sort(temp.begin(), temp.end(), f.SortBySecondGreater);
		  for(double q = 0; q < temp.size(); q++){
			  if(q == 2){
				  break;
			  }
			  _score += ((it->score)*temp.at(q).second)/10000;
		  }
		  tp.str = it->str;
		  tp.url = it->url;
		  tp.score = _score;
		  arts.push_back(tp);
		  if(_score > score){
			  article = it->str;
			  score = _score;
		  }	
	  }
	  str = article + "\t" + f.ConvertToStr(score);
  }

  bool EventDetc::Insert(std::vector<std::string>& r, double k){
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



  //事件跟踪
  bool EventDetc::EventTracking(const char* filein, const char* datadir, const char* outpath){
	  FILE*fi = fopen(filein,"r");
	  if (fi == NULL) {
		  commom::LOG_INFO("open file error" + string(filein));
		  return false;
	  }
	  char buffer[MAX_LENTH];		
	  std::string str = "";
	  std::vector<std::string> v ;
	  while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL){		  
		  str = f.GetLine(buffer); 
		  commom::DEBUG_INFO(str);
		  EventCount(str, datadir, outpath);
	  }
	  fclose(fi);
	  return true;
  }

  bool EventDetc::EventCount(string& keywd, const char* datadir, const char* outpath){
	  dailypv.clear();
	  std::vector<std::string> r ;
	  f.Split(" ", keywd, r);
	  std::pair<double, double>pv;
	  commom::DEBUG_INFO(f.ConvertToStr(r.size()));
	  for(int i = 0; i < 29; i++){		  
		  string filein = datadir + f.ConvertToStr(BEGINDATA + i);
		  FILE*fi = fopen(filein.c_str(),"r");
		  if (fi == NULL) {
			  commom::LOG_INFO("open file error" + string(filein));
			  return false;
		  }
		  char buffer[MAX_LENTH];		
		  std::string str = "";
		  std::vector<std::string> v ;		  
		  while ( f.ReadLine(buffer,MAX_LENTH,fi)!=NULL){
			  str = f.GetLine(buffer); 
			  f.Split("\t", str, v);
			  if(v.size() != 8)continue;
			  str = v.at(4);
			  double k = atof(v.at(5).c_str())/10000.01;
			  if(k <= 0 )continue;
			  f.Split(" ",mseg.Segement(str.c_str()),v);
			  //判断是否为相关文章
			  int flag = 0;
			  for(int j = 0; j < r.size(); j++){
				  for(int t = 0; t < v.size(); t++){
					  if(v.at(t) == r.at(j)){
						  flag++;
					  }
				  }
			  }
			  if(flag > 2){
				pv.first += 1;
				pv.second += k;
			  }
		  }
		  fclose(fi);
		  dailypv.push_back(pv); 
	  }
	  FILE*fo = fopen(outpath,"ab+");
	  string str = keywd;  
	  for(std::vector<std::pair<double, double> >::iterator it= dailypv.begin(); it != dailypv.end(); it++){
		  str +=(" " + f.ConvertToStr(it->first));
	  }
	  str += "\n";
	  str += keywd;
	  for(std::vector<std::pair<double, double> >::iterator it= dailypv.begin(); it != dailypv.end(); it++){
		  str +=(" " + f.ConvertToStr(it->second));
	  }
	  str += "\n";
	  f.WiteLine(str.c_str(), fo);	 
	  fclose(fo);
	  return true;
  }
  
}
 