#ifndef _EVENTDETECTION_H
#define _EVENTDETECTION_H
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <queue>
#include "../commom/func.h"
#define KEYWORDSNUM 4
#define SAVEDATE 7
typedef struct{
	std::queue<int> pv;
	float eve;
	float range;
	float inc;
} node; 
typedef std::map<std::string, node>daily_dict;
namespace text{
	//统计算法类
	class  EventDetc{
	public:
		commom::Func f;
		daily_dict worddict;
		std::map<std::string, float> dailycount;
		std::map<std::string, float> dailyhot;		
		std::vector<std::vector<std::string> > hotevent;
		float** distance;
	public:
		EventDetc();
		~EventDetc();
		void Init(const char* path);
		void WordCountDaily(const char* filein, int k);
		void GetDailyHotWords();	
		void ShowDailyHotWords(const char* filein, const char* outpath);
		void Event();
		float CountRo(std::string& stra, std::string& strb);
	};
}
#endif