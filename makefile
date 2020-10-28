
run: zhou1298.cpp 
	g++ zhou1298.cpp -std=c++11 -o parser 
	./parser NLDM_lib_max2Inp c1908_.isc 8 12


.PHONY:clean 
clean: 
	rm run zhou1298.txt 
