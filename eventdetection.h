#ifndef _EVENTDETECTION_H
#define _EVENTDETECTION_H
#include <math.h>
#include <algorithm>
#include "../commom/func.h"
#include "../wordseg/seg.h"
#include "../textanalysis/keywords.h"
#define KEYWORDSNUM 4
typedef std::map<std::string, std::map<std::string,int> >daily_dict;
namespace event{
	//统计算法类
	class  EventDetc{
	public:
		commom::Func f;
		seg::Wordseg mseg;
		text::Keywords keywd;
		//std::map<std::string, float>idfdict;
	public:
		EventDetc();
		~EventDetc();
		void Init(const char* path);
		void WordCount(const char* filein, const char* fileout);
		void WordCountDaily(const char* filein, const char* fileout);
		void SentenceCount(const char* filein, const char* fileout);
	};
}
#endif