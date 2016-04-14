#ifndef _EVENTDETECTION_H
#define _EVENTDETECTION_H
#include <math.h>
#include <algorithm>
#include <queue>
#include "../commom/func.h"
#include "../wordseg/seg.h"
#include "../textanalysis/keywords.h"
#define KEYWORDSNUM 4
#define SAVEDATE 7
typedef std::map<std::string, std::queue<int> >daily_dict;
namespace event{
	//ͳ���㷨��
	class  EventDetc{
	public:
		commom::Func f;
		seg::Wordseg mseg;
		text::Keywords keywd;
		daily_dict worddict;
		//std::map<std::string, float>idfdict;
	public:
		EventDetc();
		~EventDetc();
		void Init(const char* path);
		//void WordCount(const char* filein, const char* fileout);
		void WordCountDaily(const char* filein, const char* fileout);
		//void SentenceCount(const char* filein, const char* fileout);
	};
}
#endif