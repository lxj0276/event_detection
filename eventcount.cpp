#include<iostream>
#include "../commom/func.h"
#include "eventdetection.h"
int main(int argc, char *argv[]) {
	text::EventDetc m_event;
	m_event.Init(argv[1],argv[2],argv[3]);
	m_event.EventTracking(argv[4],argv[2],argv[5]);
	//m_count.SentenceCount(argv[2],argv[4]);
	return 0;
}