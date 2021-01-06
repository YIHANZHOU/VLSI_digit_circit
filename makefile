
run: zhou1298.cpp 
	g++ zhou1298.cpp -std=c++11 -o sta 
	./sta NLDM_lib_max2Inp b15_1.isc 

.PHONY:clean 
clean: 
	rm run zhou1298.txt 
