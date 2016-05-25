#ifndef _EVENTDETECTION_H
#define _EVENTDETECTION_H
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <algorithm>
#include <queue>
#include "../commom/func.h"
#include "../wordseg/seg.h"
#define BEGINDATA 20160201
#define HOTWORDSNUM 50
#define HOTWORDSNUMBIG 100
#define HOTARTICLE 50
#define KEYWORDSNUM 4
#define SAVEDATE 7
#define LOGPATH "/data/seanxywang/data/daily_count/"
#define ALPHA 0.9
typedef struct{
	double pv[SAVEDATE];
	double eve;
	double range;
	double inc;
} datanode; 
typedef std::map<std::string, datanode>daily_dict;

//tree
struct tree_node	{
	std::map <string,  tree_node* > childlist;
	double freq;
	tree_node(){
		freq = 0;
	}
};
typedef std::map<string,  tree_node* > nodemap;
//tree
typedef  struct{
	string str;
	string url;
	double score;
}articleinfo;

namespace text{
	//统计算法类
	class  EventDetc{
	public:
		commom::Func f;
		//事件发现
		daily_dict worddict;
		std::map<std::string, double> dailycount;
		std::map<std::string, double> dailyhot;	
		std::map<std::string, double> eventsres;
		std::vector<articleinfo > dailyhotarticle;
		std::map<std::string, double> dailyhotbig;	
		std::vector<std::vector<std::string> > hotevent;
		double** distance;
		seg::Wordseg mseg;
		tree_node* root;	
		std::map<string, double> stopwd;

		//事件跟踪
		std::vector<std::pair<double, double> >dailypv;
		bool EventTracking(const char* filein , const char* datadir, const char* outpath);
		bool EventCount(string& keywd, const char* datadir, const char* outpath);


	public:
		EventDetc();
		~EventDetc();
		void Init(const char* dictpath, const char* path, const char* stoppath);
		void WordCountDaily(const char* filepath, int d);
		void GetDailyHotWords(int x);	
		void ShowDailyHotWords(const char* filein, const char* outpath);
		void Event(tree_node* root, std::map<std::string, double>& eventsres);
		//double CountRo(std::string& stra, std::string& strb);
		bool Insert(std::vector<std::string>& r, double k);
		bool DeleteNode(tree_node* node);
		void Erase(std::vector<std::string>& r);
		void FindhotArticle(std::vector<articleinfo>& arts,string& str);
	private:
		void FreeTree();
	};
}
#endif