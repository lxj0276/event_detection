#include<iostream>
#include "../commom/func.h"
#include "../wordseg/seg.h"
#include "../textanalysis/keywords.h"
#include "count.h"
int main(int argc, char *argv[]) {
	text::Count m_count;
	m_count.Init(argv[1]);
	m_count.WordCountDaily(argv[2],argv[3]);
	//m_count.SentenceCount(argv[2],argv[4]);
	return 0;
}