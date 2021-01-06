README

All contens are in zhou.cpp

My original assumption is that the delay for different input-output paths with same output pad is different, so I create a map to store the delay of input nodes with different delay time.
but before the dealine I found my answer is inconsistent with TA so I make a instant change to assume the delay is the same for all different pad and assume the delay of an gate is the one with maximun
delay+arrival time. I don't know why this would affect my result.
but you can uncomment line 	273		//	graph[index]->required_time=min(graph[index]->required_time,graph[a]->required_time-time_to_node);  line 			288 //	inputsrequiredtime[i]=min(inputsrequiredtime[i],graph[index]->required_time-graph[index]->pre[i]);

and commment on 	274			graph[index]->required_time=min(graph[index]->required_time,graph[a]->required_time-graph[a]->pre[graph[a]->prenode]);     289 		     	inputsrequiredtime[i]=min(inputsrequiredtime[i],graph[index]->required_time-graph[index]->pre[graph[index]->prenode]);
to get the result on.


1. Main class;
for inputs remain same, no class only a vector and map<int,double>inputsrequiredtime; stores the its minimun required time from each of its outputs.
graph node store each outputs delay for different input-output paths with same output pad is different. 

vector<graphNode*>graph(1000);
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
	double slack;   //requirte time-output-time
}

store information in each gate and use map gatename to find each gateinfo by

gateinfor* gates[100];
struct gateinfor{
	string name;
	vector<double>cell_delay[7];
	vector<double>output_slew[7];
	vector<double>delay_index[2];
	
	//vector<double>slew_index[2];
	double capacity;
};

2. Sta() is forward traverse to find outputtime and slew for each gate

3. findpath() is the backward traverse to find required_time for each gate. return the first output for the critical path.

4.writefile write content in ckt_traversal. and travere back to find critical path.

