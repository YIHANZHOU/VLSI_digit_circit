// FileIO.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>  // basic C++ input/output (e.g., cin, cout)
#include <fstream>	// needed to open files in C++
#include <sstream>   // needed if you are using sstream (C++)
#include <stdio.h>	// needed to open files in C
#include <string.h> // needed for C string processing (e.g., strstr)
#include<vector>
#include <algorithm>
#include <string>
#include <map>
using namespace std;
struct ParenCommaEq_is_space : std::ctype<char> {
       ParenCommaEq_is_space() : std::ctype<char>(get_table()) {}
       static mask const* get_table()
       {

static mask rc[table_size];
rc['('] = std::ctype_base::space;
rc[')'] = std::ctype_base::space;
rc[','] = std::ctype_base::space;
rc['='] = std::ctype_base::space;
rc[' '] = std::ctype_base::space;
rc['\t'] = std::ctype_base::space;
return &rc[0];
} };
// returns the next character in file without actually reading it (i.e., going past it)
int fpeek(FILE *stream);
int parseFileCppFormat(char *fName);
int parsecircit(char *stream);

vector<int>inputs(1000,0);
vector<int>outputs(1000,0);
vector<int>outputs2(1000,0);
vector<vector<int> >outputwithinput(1000,vector<int>());


// class outputgate{
// 	vector<int>a;
// 	string 
// }
vector<string>outputwithgate(1000," ");
map<string,int>gatename;

pair<string,vector<string>[7]>cell_delay[100]; //stores gatename 2‐dimensional array delayTable
	
pair<string,vector<string>[7]>output_slew[100]; //stores gatename 2‐dimensional array delayTable

//map<int,string>mp1;
int main(int argc, char *argv[])
{
	if (argc < 3) {
		cout << "I need one parameter, which is the file name." << endl;
		return -1;
	}
	parseFileCppFormat(argv[1]);
	
	parsecircit(argv[2]);
///cout<<endl;
    
	for(int i=3;i<argc;i++){
//		cout<<argv[i]<<endl;

		string temp(argv[i]);
		
		istringstream ir(temp);
		int a;
		ir>>a;
//		cout<<a<<endl<<inputs[a];
		if(a<inputs.size()&&inputs[a]==1){
			cout<<a<<" input"<<endl;
		}else if(a<outputwithinput.size()&&!outputwithinput[a].empty()){
			cout<<a<<" "<<outputwithgate[a]<<" ";

			int n=outputwithinput[a].size()-1;
			cout<<outputwithinput[a][0]<<" "<<outputwithinput[a][n]<<" ";
			
			if(gatename.find(outputwithgate[a])!=gatename.end()){
			int gate_id=gatename[outputwithgate[a]];
			
			//cout<<output_slew[gate_id+1].second[2][1];
			}else{
				cout<<"0";
			}
			cout<<endl;
		}
		else if(a<outputs.size()&&outputs[a]==1){
			cout<<a<<" output"<<endl;
		}   else{
			cout<<" cannot find"<<temp<<endl;
		}
	}
	return 0;
}


int searchinput(int a,vector<int>b){
	int mind=0;
	int maxd=b.size()-1;
	while(mind<maxd){
		int center=(mind+maxd)/2;
		if(b[center]==a){
			return center;
		}
		if(b[center]>a){
			maxd=center;
			break;
		}else{
			mind=center;
		}
	}
	return -1;
	
}

// Kia got this function from stackoverflow https://stackoverflow.com/questions/2082743/c-equivalent-to-fstreams-peek
int fpeek(FILE *stream)
{
	int c;

	c = fgetc(stream);
	ungetc(c, stream);

	return c;
}


int parsecircit(char *fName){

	char lineBuf[1024];
//	cout << "Parsing input file " << fName << " using C++ syntax." << endl;
	ifstream ifs(fName);
	if (ifs.is_open() == 0){ // or we could say if (!ifs)
		cout << "Error opening file " << fName << endl;
		return -1;
	}
 	int state=0;
//	cout<<ifs.good();
	while (ifs.good()) {
		
		ifs.getline(lineBuf, 1023);	// read one line
		string lineStr(lineBuf); // convert to C++ string

		if (lineStr[0]=='#')	// is empty line?
		//cout<<"is$";
			continue;
		if (lineStr.find('=') != string::npos){
			state=1;
		}
		   
		// if (lineStr.empty())	// is empty line?
		// 	continue;
		
		//cout<<lineStr;

		istringstream iss(lineStr); 
		iss.imbue(locale(cin.getloc(),new ParenCommaEq_is_space));
		//cout<<iss.str()<<endl;
		string temp; int a;

		if(state==0){
		iss>>temp>>a;
		//cout<<sizeof(inputs)<<endl;
		if(temp[0]=='I'){
			if( a >=inputs.size()){
				int newsize=(1+a/1000)*1000;
				inputs.resize(newsize); 
			}
			
			inputs[a]=1;
		}else{
			if( a >= outputs.size()){
				int newsize=(1+a/1000)*1000;
				outputs.resize(newsize); 
			}
			outputs[a]=1;
		}
		//cout<<temp<<a<<endl;

		}
		if(state==1){
			
		iss>>a>>temp;
		
		if( a >= outputwithinput.size()){
				int newsize=(1+a/1000)*1000;
				outputwithinput.resize(newsize); 
				outputwithgate.resize(newsize);
			}
		transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
		outputwithgate[a]=temp;
		int number;
		//cout<<a<<temp<<endl;
		while(iss>>number){
			outputwithinput[a].push_back(number);
		}
			
		}

		// 

		// if(state==0){
		// 
		// iss>>temp>>a;
		

		// }
		
		

		
	}

	




	return 0;
	
	
}

int parseFileCppFormat(char *fName)
{
	char lineBuf[1024];
	int gatenum=0;
	int read_cell=-2;
	//vecto
	
	//cout << "Parsing input file " << fName << " using C++ syntax." << endl;
	ifstream ifs(fName);

	ofstream myfile ("zhou1298.txt");
	if (ifs.is_open() == 0){ // or we could say if (!ifs)
		cout << "Error opening file " << fName << endl;
	    myfile << "Error opening file " << fName << endl;

		return -1;
	}

	while (ifs.good()) {
		ifs.getline(lineBuf, 1023);	// read one line
		string lineStr(lineBuf); // convert to C++ string
		if (lineStr.empty())	// is empty line?
			continue;

		istringstream iss(lineStr); 

		string firstWord,secondWord;
		iss >> firstWord;
		if (firstWord.find("cell") != string::npos) { // found the word cell
			char c;
			iss >> c; // read '('
			if (c != '(') {
				continue; // this is not an error. Happens for example when we have cell_delay(
			}
	//		cout << "Found cell ";

			string cellName;
			iss >> cellName;
			cellName=cellName.substr(0,cellName.size()-1);
			cell_delay[gatenum].first=cellName;
			output_slew[gatenum].first=cellName;
			gatename[cellName]=gatenum;
		//	cout << cellName << endl;
			gatenum++;
			read_cell=-1;
		}
		if(firstWord.find("values")!=string::npos && read_cell!=-2){
			//cout<<firstWord;
			read_cell=read_cell==-1?0:8;
		}



		if(read_cell>-1 && read_cell<7){
			//iss>>test>>a;
			//cout<<test;

			
			string test=iss.str();

			int start=read_cell==0?4:4;
            //test.erase(std::remove_if(test.begin(), test.end(), isspace), test.end());
            test.erase(remove(test.begin(), test.end(), '('), test.end());
            test.erase(remove(test.begin(), test.end(), 'v'), test.end());
			test.erase(remove(test.begin(), test.end(), 'a'), test.end());
			test.erase(remove(test.begin(), test.end(), 'l'), test.end());
			test.erase(remove(test.begin(), test.end(), 'u'), test.end());
			test.erase(remove(test.begin(), test.end(), 'e'), test.end());
			test.erase(remove(test.begin(), test.end(), 's'), test.end());
			test.erase(remove(test.begin(), test.end(), ' '), test.end());
			test.erase(remove(test.begin(), test.end(), '"'), test.end());

			for(int j=1;j<test.size();j++){
			
				if(test[j]==','|| test[j]==')'){
					string temp=test.substr(start,j-start);
					start=j+1;

					cell_delay[gatenum-1].second[read_cell].push_back(temp);

					//gatename[gatenum-1][read_cell].push_back(temp);
	
					}
					//cout<<temp<<"|";
				}
			

			//cout<<"\n";
			read_cell++;
			//read_cell+=1;
			}
	

	if(read_cell>7){
			//iss>>test>>a;
			//cout<<test;

			
			string test=iss.str();

			int start=read_cell==0?4:4;
            //test.erase(std::remove_if(test.begin(), test.end(), isspace), test.end());
            test.erase(remove(test.begin(), test.end(), '('), test.end());
            test.erase(remove(test.begin(), test.end(), 'v'), test.end());
			test.erase(remove(test.begin(), test.end(), 'a'), test.end());
			test.erase(remove(test.begin(), test.end(), 'l'), test.end());
			test.erase(remove(test.begin(), test.end(), 'u'), test.end());
			test.erase(remove(test.begin(), test.end(), 'e'), test.end());
			test.erase(remove(test.begin(), test.end(), 's'), test.end());
			test.erase(remove(test.begin(), test.end(), ' '), test.end());
			test.erase(remove(test.begin(), test.end(), '"'), test.end());

			for(int j=1;j<test.size();j++){
			
				if(test[j]==','|| test[j]==')'){
				//	cout<<output_slew[gatenum-1].first;

					string temp=test.substr(start,j-start);
					start=j+1;
				//	cout<<gatenum<<" "<<temp<<" ";
				    

					output_slew[gatenum-1].second[read_cell].push_back(temp);
					//string name=output_slew[gatenum-1].first;
				//	cout<<name<<endl;


					}
					//cout<<temp<<"|";
				}
			

			//cout<<"\n";
			read_cell++;
			if(read_cell==15){
				read_cell=-2;}
			//read_cell+=1;
			}
	}
	
		
			// want to get rid of the ')' at the end of the cell name?
			// You can get rid of the last character, but a cooler and probably much more useful technique is to
			// define your delimiter to be space and ')'.
			// This would be useful when later we want to get rid of " and , as well to get the 7x7 delay table.
			// See this post and the solution https://stackoverflow.com/questions/7302996/changing-the-delimiter-for-cin-c
		
	




    myfile<<"Number of cell types"<<gatenum<<"\n";
// 	for(int i=0;i<gatenum+1;i++){
// 		pair<string,vector<string>[7]> content=cell_delay[i];
// 		pair<string,vector<string>[7]> content2=output_slew[i+1];
		
// //		gatename.insert({content2.first,content2.second});

// 		myfile<<i<<content.first<<"\n";
// 		myfile<<"cell_delay"<<"\n";

// 		for(vector<string>a:content.second){
// 			for(string b:a){
// 				myfile<<b<<",";
// 			}
// 			myfile<<"\n";
// 		}

// 		myfile<<"output_slew"<<"\n";


// 		for(vector<string>a:content2.second){
// 			for(string b:a){
// 				myfile<<b<<",";
// 			}
// 			myfile<<"\n";
// 		}
		

// 	}


	return 0;
}
