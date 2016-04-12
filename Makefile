all:
	g++ -o gettitle gettitle.cpp ../commom/func.cpp 
clean:
	$(RM) -rf gettitle
