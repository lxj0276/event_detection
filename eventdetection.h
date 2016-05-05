#ifndef _EVENTDETECTION_H
#define _EVENTDETECTION_H
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <queue>
#include "../commom/func.h"
#include "../wordseg/seg.h"
#define BEGINDATA 20160207
#define HOTWORDSNUM 20
#define HOTWORDSNUMBIG 100
#define KEYWORDSNUM 4
#define SAVEDATE 7
#define ALPHA 0.9
typedef struct{
	int pv[SAVEDATE];
	float eve;
	float range;
	float inc;
} datanode; 
typedef std::map<std::string, datanode>daily_dict;

//tree
struct tree_node	{
	std::map <string,  tree_node* > childlist;
	size_t freq;
	tree_node(){
		freq = 0;
	}
};
typedef std::map<string,  tree_node* > nodemap;
//tree

namespace text{
	//统计算法类
	class  EventDetc{
	public:
		commom::Func f;
		daily_dict worddict;
		std::map<std::string, float> dailycount;
		std::map<std::string, float> dailyhot;	
		std::map<std::string, float> dailyhotbig;	
		std::vector<std::vector<std::string> > hotevent;
		float** distance;
		seg::Wordseg mseg;
		tree_node* root;	
		std::map<string, int> stopwd;
	public:
		EventDetc();
		~EventDetc();
		void Init(const char* dictpath, const char* path, const char* stoppath);
		void WordCountDaily(const char* filepath, int k);
		void GetDailyHotWords();	
		void ShowDailyHotWords(const char* filein, const char* outpath);
		void Event(tree_node* root);
		float CountRo(std::string& stra, std::string& strb);
		bool Insert(std::vector<std::string>& r, int k);
		bool DeleteNode(tree_node* node);
		void Erase(std::vector<std::string>& r);
	private:
		void FreeTree();
	};
}
#endif