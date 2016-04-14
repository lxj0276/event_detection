all:
	g++ -o biz bizuin.cpp ../commom/func.cpp 
	g++ -o gettitle gettitle.cpp ../commom/func.cpp 
	g++ -o keywdcount getkeywords.cpp count.cpp ../wordseg/seg.cpp ../wordseg/hmm.cpp ../wordseg/tree.cpp ../wordseg/symbol.cpp ../textanalysis/keywords.cpp ../commom/func.cpp
	g++ -o titleclassfy titleclassfy.cpp count.cpp ../categorization/textcategorization.cpp ../wordseg/seg.cpp ../wordseg/hmm.cpp ../textanalysis/texthash.cpp ../wordseg/tree.cpp ../wordseg/symbol.cpp ../textanalysis/keywords.cpp ../commom/func.cpp ../textanalysis/textvec.cpp 
clean:
	$(RM) -rf gettitle keywdcount biz titleclassfy
