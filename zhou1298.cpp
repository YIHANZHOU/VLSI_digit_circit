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
#include <queue>
#include <set>
#include <map>
#include <stack>


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
struct gateinfor{
	string name;
	vector<double>cell_delay[7];
	vector<double>output_slew[7];
	vector<double>delay_index[2];
	
	//vector<double>slew_index[2];
	double capacity;
    gateinfor(string name_,double capacity_) :name(name_),capacity(capacity_){};
};
struct graphNode{
	vector<int>next;   //which are output node of this gate
	map<int,double>pre;   //which are input node of this gate and corelated delay time for those gate
	double output_slew;   //output slew of this gate
	double output_time;   // output time of this gate
	double required_time; //requirte time
	int prenode;  // the input nodes with mamimun latency(critical previous node)
	int degree;   // how many inputs its have
	int nextindex;  //how many outputs its have
	string gate;    //gate name
	double cap;     // capcity 
	double delay;
	double slack;   //requirte time-output-time
	graphNode():gate("None"),degree(0),output_slew(0),output_time(0),cap(0),slack(0),required_time(100000000),nextindex(0),prenode(0),delay(0){};

	graphNode(string g):gate(g),degree(0),output_slew(0),output_time(0),cap(0),slack(0),required_time(100000000),nextindex(0),prenode(0),delay(0){};
};

//map<int,graphNode*>graph;
queue<int>que;

// returns the next character in file without actually reading it (i.e., going past it)
int fpeek(FILE *stream);
int parseFileCppFormat(char *fName);
int parsecircit(char *stream);
set<int>finaloutput;
set<int>finaldif;

map<int,double>virtualmap;
map<int,double>virtualdelay;

vector<int>inputs(1000,0);
map<int,double>inputsrequiredtime;

vector<int>outputs(1000,0);
bool diff=0;
// vector<vector<int>>pre(1000);
// vector<vector<int>>next(1000);

vector<graphNode*>graph(1000);
gateinfor* gates[100];
map<string,gateinfor*>gatename;
pair<double,double> f(double slew,double capacity,gateinfor* gate){
	
	
	vector<double>C=gate->delay_index[1];
	vector<double>S=gate->delay_index[0];
	//double C1=0;double C2=0;double s1=0; double s[2]=0; double v11=0;double v22=0; double v12=0;double v21=0;
	int i; int j;
	for(i=0;i<6;i++){
		if(slew>=S[i]&& slew<S[i+1]){
			break;
		}
	}
	if(i==6){
		i--;
	}

	for(j=0;j<6;j++){
		if(capacity>=C[j]&& capacity<C[j+1]){
			break;
		}
	}
	if(j==6){
		j--;
	}
//	cout<<"sss"<<slew<<" "<<i<<j<<" ";
//	cout<<i<<j<<gate->cell_delay[1].size()<<endl;
//	cout<<slew<<":"<<capacity<<":"<<i<<j<<endl;
	pair<double,double>res;

	if(C.size()<7||S.size()<7||gate->output_slew[0].size()<7||gate->cell_delay[0].size()<7){
		cout<<gate->name<<"invalide gate"<<endl;
		return res;
	}

	double product=(C[j+1]-C[j])*(S[i+1]-S[i]);
	//vector<double>cell[7]=gate->cell_delay;
	double v11=gate->output_slew[i][j];double v22=gate->output_slew[i+1][j+1]; double v12=gate->output_slew[i][j+1];double v21=gate->output_slew[i+1][j];
    
	double sum=v11*(C[j+1]-capacity)*(S[i+1]-slew) + v12*(capacity-C[j])*(S[i+1]-slew) + (slew-S[i])*(v21*(C[j+1]-capacity) + v22*(capacity-C[j]));
    res.second=sum/product;

	
	v11=gate->cell_delay[i][j];v22=gate->cell_delay[i+1][j+1]; v12=gate->cell_delay[i][j+1];v21=gate->cell_delay[i+1][j];
	sum=v11*(C[j+1]-capacity)*(S[i+1]-slew) + v12*(capacity-C[j])*(S[i+1]-slew) + (slew-S[i])*(v21*(C[j+1]-capacity) + v22*(capacity-C[j]));
    res.first=sum/product; //delay
//	cout<<i<<j<<"product"<<product<<"dealay"<<res.first;

	return res;
}
vector<int>isvisited;

void sta(){
	isvisited.resize(graph.size());
    int count=0;

	while(!que.empty()){
		int index=que.front();
		count++;
		isvisited[index]=1;
		que.pop();
		double maxi=0;
		string name_gate=graph[index]->gate;
		if(!graph[index]){
			cout<<"error"<<endl;
		}
        
		if(gatename.find(name_gate)==gatename.end()){
		 cout<<"error"<<endl;
	     cout<<graph[index]->gate<<"not in the gatelist";
       }
		gateinfor*gate=gatename[name_gate];
		int n=graph[index]->pre.size();if(n<2){n=2;}

		double capacity=0;
		for(int a:graph[index]->next){// cout<<a<<" "<<gatename[graph[a]->gate]->capacity;
		capacity+=gatename[graph[a]->gate]->capacity;}//cout<<endl;
		double virtualcap=4.0*gatename["INV"]->capacity;
		if(index<outputs.size()&&outputs[index]==1){capacity=4.0*gatename["INV"]->capacity;}
	    //cout<<index;
		//double output_delay=0;
		double output_arrive=0;
		double output_slew=0;

	//	cout<<index<<" "<<graph[index]->gate<<graph[index]->pre.size()<<" "<<capacity<<" ";
//cout<<index<<" "<<capacity<<endl;
		for(auto& temp:graph[index]->pre){
			//cout<<a<<" ";
			int a=temp.first;
			double arrivetime=0;
			double slew=0.002;
		    if(graph[a]){ 
			//cout<<"not input"<<" "; 
			arrivetime=graph[a]->output_time;
			slew=graph[a]->output_slew;
		    }
	//		pair<double,double>outputs={0,1};//DFF situation
			if(finaldif.find(index)!=finaldif.end()){
				pair<double,double>outputs2=f(slew,virtualcap,gate);
				if(outputs2.first*(double)n/2+arrivetime>virtualmap[index]){
				virtualmap[index]=outputs2.first*double(n)/2+arrivetime;
				virtualdelay[index]=outputs2.first*double(n)/2;
			}
			}

			pair<double,double>outputs=f(slew,capacity,gate); //delay slew
			temp.second=outputs.first*n/2;
			cout<<"delay"<<temp.first<<" "<<temp.second<<endl;

		//	cout<<outputs.first<<endl;
	   	//	cout<<outputs.second<<" "<<arrivetime;
			if(outputs.first*n/2+arrivetime>output_arrive){
				output_arrive=outputs.first*n/2+arrivetime;
				output_slew=outputs.second*n/2;
				graph[index]->prenode=temp.first;

			}
		}

//		cout<<endl;
//		cout<<"output_arrive"<<output_arrive<<endl;
		graph[index]->output_slew=output_slew;
		graph[index]->output_time=output_arrive;

		cout<<index<<" "<<output_arrive<<endl;
		if(index<outputs.size()&&outputs[index]==1){
			continue;
		}

		for(int a:graph[index]->next){
			graph[a]->degree--;
			if(graph[a]->degree==0){
				if(isvisited[a]==0){
				que.push(a);
			//	isvisited[a]=1;
			}
			}
		}
	}
	cout<<"traverse through "<<count<<" nodes in total"<<endl;

}
double delay=0;

int findpath(){
	int last=0;
	queue<int>backtrack;
	vector<int>isvisited(graph.size(),0);

	if(diff==1){
	for(int a:finaldif){
	  if(graph[a]){
	
	 // cout<<a<<" "<<graph[a]->output_time<<endl;
	  if(graph[a]->nextindex==0){backtrack.push(a);}
	  delay=max(delay,virtualmap[a]);
	  last=a;}
	//	cout<<f(graph[a]->output_slew,1.700230,gatename["INV"]).second+graph[a]->output_time<<endl;	
	}
	}
	else{
	for(int a:finaloutput){
			//cout<<graph[a]->output_time<<endl;
	  if(graph[a]){
		//  cout<<graph[a]->nextindex;
	  if(graph[a]->nextindex==0){backtrack.push(a);}
	  delay=max(delay,graph[a]->output_time);
	  last=a;}
	//	cout<<f(graph[a]->output_slew,1.700230,gatename["INV"]).second+graph[a]->output_time<<endl;	
	}
	}
	cout<<"delay-time"<<delay<<endl;
	double totalarrive=1.1*delay;
	while(!backtrack.empty()){
		int index=backtrack.front();
		backtrack.pop();
		isvisited[index]=1; 


		if((index<outputs.size()&&outputs[index]==1)||(diff==1&&finaldif.find(index)!=finaldif.end())){
		//	cout<<index<<" "<<endl;
			graph[index]->required_time=totalarrive;
		}else{
			for(int a:graph[index]->next){
		    	double time_to_node=graph[a]->pre[index];
				if(finaldif.find(a)!=finaldif.end()){
					graph[index]->required_time=min(totalarrive-virtualdelay[a],graph[index]->required_time);
				}else{
			//	cout<<index<<" "<<a<<time_to_node<<endl;
			//	graph[index]->required_time=min(graph[index]->required_time,graph[a]->required_time-time_to_node);
				graph[index]->required_time=min(graph[index]->required_time,graph[a]->required_time-graph[a]->pre[graph[a]->prenode]);
				}

			}
		}
	//	cout<<index<<" "<<graph[index]->required_time<<endl;
		for(auto a:graph[index]->pre){
			int i=a.first;
			if(graph[i]){
			graph[i]->nextindex--;
			if(graph[i]->nextindex==0&&isvisited[i]==0){
				backtrack.push(i);
			}
			}else{
			//	inputsrequiredtime[i]=min(inputsrequiredtime[i],graph[index]->required_time-graph[index]->pre[i]);
		     	inputsrequiredtime[i]=min(inputsrequiredtime[i],graph[index]->required_time-graph[index]->pre[graph[index]->prenode]);

			}
		}
		
		
		
	}
	return last;



}

void writefile(int a){
  ofstream myfile ("ckt_traversal.txt");
   myfile<<"circutdelay : "<<delay*1000<<" pS\n";

	myfile<<"Gate Slack:"<<endl;

   	for(int i=0;i<inputs.size();i++){
		if(inputs[i]){
	//	cout<<"Node"<<i<<":"<<" "<<graph[i]->required_time<<" "<<graph[i]->output_time<<graph[i]->required_time-graph[i]->output_time<<" ps\n";
		myfile<<"INPUT      -NODE:  "<<i<<"      Slack"<<":"<< 1000*inputsrequiredtime[i]<<" ps\n";
		}
	}

	for(int i=0;i<graph.size();i++){
		if(graph[i]&&isvisited[i]){
	//	cout<<"Node"<<i<<":"<<" "<<graph[i]->required_time<<" "<<graph[i]->output_time<<graph[i]->required_time-graph[i]->output_time<<" ps\n";
		myfile<<graph[i]->gate<<"         NODE"<<i<<":"<<"      Slack"<<1000*(graph[i]->required_time-graph[i]->output_time)<<" ps\n";

		}
	}
	myfile<<"Critical Path:"<<endl;
	stack<int>criticalpath;
	while(graph[a]){
		criticalpath.push(a);
		a=graph[a]->prenode;
	}
	myfile<<"INPUT Node"<<a<<endl;
	while(!criticalpath.empty()){
		int temp=criticalpath.top();
		myfile<<graph[temp]->gate<<" Node "<<temp<<endl;
		criticalpath.pop();
	}
	//cout<<"Node"<<a<<" ";
	//cout<<endl;

	
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		cout << "I need one parameter, which is the file name." << endl;
		return -1;
	}
	 parseFileCppFormat(argv[1]);

	if(parsecircit(argv[2])==-1){
		cout<<"fail to open file"<<argv[2]<<endl;
		return 0;
	}
//	cout<<graph[224]->gate;
//cout<<graph[4101]->next.size();
	sta();

	//cout<<graph[7496]->output_time<<endl;
//	cout<<graph[4101]->next.size();

    int a=findpath();
	writefile(a);

    
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
		//cout<<a;
		//cout<<sizeof(inputs)<<endl;
		if(temp[0]=='I'){
			if( a >=inputs.size()){
				int newsize=(1+a/1000)*1000;
				inputs.resize(newsize); 
			}
			inputs[a]=1;
			inputsrequiredtime[a]=1000000;
		}else{
			if( a >= outputs.size()){
				int newsize=(1+a/1000)*1000;
				outputs.resize(newsize); 
			}
		//	finaldif.insert(a);
			outputs[a]=1;
			finaloutput.insert(a);
			
		}
		//cout<<temp<<a<<endl;

		}
		else if(state==1&&iss>>a>>temp){
		if(a<inputs.size()&&inputs[a]==1){
			cout<<a<<endl;
		}
	  //  cout<<a;
		if( a >= graph.size()){
				int newsize=(1+a/1000)*1000;
				graph.resize(newsize); 
				//pre.resize(newsize);
				//next.resize(newsize);
		}
		transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
		
		if(!graph[a])graph[a]=new graphNode;
		graph[a]->gate=temp;
		int number;
		if(temp=="DFF"){
			
			diff=1;
			graph[a]=NULL;
			if( a >=inputs.size()){
				int newsize=(1+a/1000)*1000;
				inputs.resize(newsize); 
			}
			inputs[a]=1;
			inputsrequiredtime[a]=1000000;
            iss>>number;
			
			if( number >=outputs.size()){
				int newsize=(1+a/1000)*1000;
				outputs.resize(newsize); 
			}
			
		   // outputs[number]=1;

			finaldif.insert(number);
		//	virtualmap[number]=a;
			continue;
		}
	//	int iso=0;

		//cout<<a<<" ";
		//if(outputs[a]==1)graph[a]->cap=4.0*gatename["INV"]->capacity;
		//cout<<gatename[temp]->capacity;
       //double capacity=gatename[temp]->capacity;	
		while(iss>>number){
			//pre[a].push_back(number);
			// if(virtualmap.find(number)!=virtualmap.end()){
			// 	iso=1;
			// 	break;
			// }
			graph[a]->pre[number]=0;

			if(number<inputs.size()&&inputs[number]==1){continue;}
			if( number >= graph.size()){
				int newsize=(1+number/1000)*1000;
				graph.resize(newsize); 
			//	pre.resize(newsize);
			//	next.resize(newsize);
		     }
			if(!graph[number]){graph[number]=new graphNode;}
			graph[number]->next.push_back(a);
			graph[number]->nextindex++;
			//next[number].push_back(a);
			graph[a]->degree++;	

		}

		if(graph[a]->degree==0){
		//	cout<<a<<endl;
		que.push(a);
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
		//	gateinfor*temp=new gateinfor(cellName);
			gates[gatenum]=new gateinfor(cellName,0);
			// cell_delay[gatenum].first=cellName;
			// output_slew[gatenum].first=cellName;
			gatename[cellName]=gates[gatenum];
		//	cout << cellName << endl;
			gatenum++;
			read_cell=-1;
		}
		if(firstWord.find("capacitance")!=string::npos && read_cell==-1){
			char temp;
			double a;
			iss>>temp>>a;
			gates[gatenum-1]->capacity=a;
	
		}
		if(firstWord.find("index_1")!=string::npos && read_cell==-1){
			char temp;
			char temp2; double a;
			iss>>temp>>temp2;
           while(iss>>a>>temp){
				gates[gatenum-1]->delay_index[0].push_back(a);
			}		
		}
		if(firstWord.find("index_2")!=string::npos && read_cell==-1){
			char temp;
			char temp2; double a;
			iss>>temp>>temp2;
           while(iss>>a>>temp){
				gates[gatenum-1]->delay_index[1].push_back(a);
			}		
		}
		// if(firstWord.find("index_1")!=string::npos && read_cell==7){
		// 	char temp;
		// 	char temp2; double a;
		// 	iss>>temp>>temp2;
        //    while(iss>>a>>temp){
		// 		gates[gatenum-1]->slew_index[0].push_back(a);
		// 	}		
		// }
		// if(firstWord.find("index_2")!=string::npos && read_cell==7){
		// 	char temp;
		// 	char temp2; double a;
		// 	iss>>temp>>temp2;
        //    while(iss>>a>>temp){
		// 		gates[gatenum-1]->slew_index[1].push_back(a);
		// 	}		
		// }
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
			//		cout<<read_cell<<stod(temp);

					gates[gatenum-1]->cell_delay[read_cell].push_back(stod(temp));

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
					//cout<<read_cell<<" ";
				    

					gates[gatenum-1]->output_slew[read_cell-8].push_back(stod(temp));
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
		
	




 //   myfile<<"Number of cell types"<<gatenum<<"\n";
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
