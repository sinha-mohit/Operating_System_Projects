/* While process i moved from Q2 to Q3 and another process j is already present in the Q3 then, while selecting process from Q3 process j should get precedence but process i is getting precedence, Sometimes.*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <limits.h>

#define ageing 1

using namespace std;


typedef struct _node{
	int index;
	int burst_time;
	int arrival_time;
	int res_arr_time;
	int rem_time;
	int wait_time;
	int trn_arnd_time;
	int prt;
	int myq;
} PROC;




//temp->ate<typename T>
//class my_pq : public std::priority_queue<T, std::vector<T>>{
template <typename T, class Container=std::vector<T>, class Compare=std::less<typename Container::value_type>> 
class my_pq : public std::priority_queue<T, Container, Compare>{

	public:

		bool rem(const int val) {
			//auto iter = std::find(this->c.begin(), this->c.end(), val);
			auto iter = std::find_if(this->c.begin(), this->c.end(),  [val] (const PROC* d){return d->index == val;});
			if (iter != this->c.end()) {
				this->c.erase(iter);
				std::make_heap(this->c.begin(), this->c.end(), this->comp);
				return true;
			} else {
				return false;
			}
		}
};



struct sort_p {
	bool operator()(const PROC* p ,const PROC* q){
		if (p->prt < q->prt){
			return false;
		} else if (p->prt > q->prt){
			return true;
		} else {
			return p->index < q->index;
		}
	}
};


struct sort_a {
        bool operator()(const PROC* p ,const PROC* q){
                if (p->res_arr_time < q->res_arr_time){
                        return false;
                } else if (p->res_arr_time > q->res_arr_time){
                        return true;
                } else {
                        return p->prt > q->prt;
                }
        }
};

/*bool operator == (const my_pq<PROC,vector<PROC>, sort_p> &p1,const my_pq<PROC,vector<PROC>, sort_p> &p2){
        if (p1.index == p2.index){
                return true;
        } else {
                return false;
        }
}*/


//void do_swap(int index,priority_queue <PROC> &fromq, priority_queue <PROC> &toq,map <int,int> &fromm,map <int,int> &tom,int tom_q){
	
/*void do_swap(int index,priority_queue <PROC,vector<PROC>, sort_p> &fromq, priority_queue <PROC,vector<PROC>, sort_p> &toq,map <int,int> &fromm,map <int,int> &tom,int tom_q){

	vector <PROC> temp->
	PROC tp;
	int flag = 0;
	
	while(!fromq.empty()){
		tp = fromq.top();
		fromq.pop();
		if (tp.index == index){
			toq.push(tp);
			tom.insert({tp.index,tom_q});
			fromm.erase(tp.index);
			flag = 1;
			break;
		} else {
			temp->push_back(tp);
		}
	}

	if (flag){
		while (!temp->empty()){
			tp = temp->pop_back();
			toq.push(tp);
		}
	} else {
		printf("something went wrong\n");
	}
}*/
			


PROC* generate_proc_data(PROC *s,int n){

	srand(time(0));
	for(int i=0;i<n;i++){
		(s+i)->index = i;
		(s+i)->burst_time = ((rand())%30) + 1;
		(s+i)->arrival_time = rand()%10;
		(s+i)->wait_time = -1;
		(s+i)->trn_arnd_time = -1;
		(s+i)->res_arr_time = (s+i)->arrival_time;
		(s+i)->rem_time = (s+i)->burst_time;
		(s+i)->prt = rand()%40;
		(s+i)->myq = -1;
	}

	return s;
}

void print_details(PROC *s,int n){

	printf("+*******+*********+***********+***********+*******+*********+***********+**********+*******+\n");
	printf("| INDEX | Burst T | Arrival T | Res Arr T | Rem T | Waiting T | TArnd T | Priority | Queue |\n");
	printf("+*******+*********+***********+***********+*******+*********+***********+**********+*******+\n");

	for(int i=0;i<n;i++){
		printf("\t%d\t",(s+i)->index);
		printf("%d\t",(s+i)->burst_time);
		printf("%d\t",(s+i)->arrival_time);
		printf("\t%d\t",(s+i)->res_arr_time);
		printf("%d\t",(s+i)->rem_time);
		printf("%d\t",(s+i)->wait_time);
		printf("%d\t",(s+i)->trn_arnd_time);
		printf("\t%d\t",(s+i)->prt);
		printf("%d\n",(s+i)->myq);
	}
}

void sort_arrival(PROC *s,int n){

	PROC temp;
	for (int i=0;i<n;i++){
		for(int j=0;j<n-1;j++){
			if((s+j)->arrival_time > (s+j+1)->arrival_time){
				temp = *(s+j);
				*(s+j) = *(s+j+1);
				*(s+j+1) = temp;
			}
		}
	}
	//print_details(s,n);
}	


void prnt_chart(vector<vector<string> > x){
	for (unsigned int i = 0; i < x.size(); i++) {
		for (unsigned int j = 0; j < x[i].size(); j++)
			cout << x[i][j];
		cout << endl;
	}
}

void mlfq_sched(PROC *p,int n){

	vector<vector<string> > chart(3);
	my_pq <PROC*,vector<PROC*>, sort_p> q1;
	my_pq <PROC*,vector<PROC*>, sort_a> q2;
	my_pq <PROC*,vector<PROC*>, sort_a> q3;
	PROC *exis,*temp;
	/*map <int,int> mq1;
	map <int,int> mq2;
	map <int,int> mq3;*/
	int num_proc = 0;
	int ts = 0;
	int q1_lp = 0;
	int q2_lp = 10;
	int q3_lp = 20;
	int q1_slice = 2;
	int q2_slice = 5;
	int q3_slice = 8;
	int c_s = 0;
	int proc_q1 = 0;
	int proc_q2 = 0;
	int proc_q3 = 0;
	int t_prt = -1;
	int slice = 0;
	int sum_w = 0;
	int sum_t = 0;
	string t1 = "|";
	string t2,t3;

	int prt = INT_MAX;
	int index = -1;

	//auto it;


	sort_arrival(p,n);
	print_details(p,n);

	cin.get();

	while(num_proc != n){
		
		for(int i=0;i<n;i++){
			if ((p+i)->arrival_time == ts){
				if ((p+i)->prt >=q1_lp && (p+i)->prt <q2_lp){
					q1.push((p+i));
					(p+i)->myq = 1;
					//mq1.insert({(p+i)->index,1});
				} else if ((p+i)->prt >=q2_lp && (p+i)->prt <q3_lp){
					q2.push((p+i));
					(p+i)->myq = 2;
					//mq2.insert({(p+i)->index,2});
				} else if ((p+i)->prt >= q3_lp){
					q3.push((p+i));
					(p+i)->myq = 3;
					//mq3.insert({(p+i)->index,3});
				}
			}

			if ((p+i)->prt >=q1_lp && (p+i)->prt <q2_lp){

				if(q2.rem((p+i)->index)){
					//q1.push((p+i));
					(p+i)->res_arr_time = ts;
					(p+i)->myq = 1;
					q1.push((p+i));

				} else if (q3.rem((p+i)->index)){
					//q1.push((p+i));
                                        (p+i)->res_arr_time = ts;
					(p+i)->myq = 1;
					q1.push((p+i));

				}
			} else if ((p+i)->prt >=q2_lp && (p+i)->prt <q3_lp){

				if(q1.rem((p+i)->index)){
					//q2.push((p+i));
                                        (p+i)->res_arr_time = ts;
					(p+i)->myq = 2;
					q2.push((p+i));

                                } else if (q3.rem((p+i)->index)){
					//q2.push((p+i));
                                        (p+i)->res_arr_time = ts;
					(p+i)->myq = 2;
					q2.push((p+i));
                                }

			} else if ((p+i)->prt >= q3_lp){

				if(q1.rem((p+i)->index)){
                                	//q3.push((p+i));
                                        (p+i)->res_arr_time = ts;
					(p+i)->myq = 3;
                                	q3.push((p+i));
                                } else if (q2.rem((p+i)->index)){
                                	//q3.push((p+i));
                                        (p+i)->res_arr_time = ts;
					(p+i)->myq = 3;
                                	q3.push((p+i));
                                }

			}

		}

		print_details(p,n);
                prnt_chart(chart);

		if(c_s == 0){
			
			if(!q1.empty()){
				exis = q1.top();
				q1.pop();
				c_s++;
				proc_q1++;
				slice = q1_slice;
				prt = exis->prt;
				index = exis->index;
			} else if (!q2.empty()){
				exis = q2.top();
                                q2.pop();
                                c_s++;
                                proc_q2++;
                                slice = q2_slice;
				prt = exis->prt;
                                index = exis->index;
			} else if (!q3.empty()){
				exis = q3.top();
                                q3.pop();
                                c_s++;
                                proc_q3++;
                                slice = q3_slice;
				prt = exis->prt;
                                index = exis->index;
			}

		} else if(c_s == 1){

			if (proc_q1 == 1 && (!q1.empty())){
				
				temp = q1.top();
				if (temp->prt < prt){
					q1.pop();
					q1.push(exis);
					exis = temp;
					slice = q1_slice;
					prt = exis->prt;
                                	index = exis->index;
				}
			} else if (proc_q2 == 1 && (!q1.empty())){

				q2.push(exis);
				exis = q1.top();
				q1.pop();
				proc_q2 = 0;
				proc_q1 = 1;
				slice = q1_slice;
				prt = exis->prt;
                                index = exis->index;
			} else if (proc_q3 == 1 && (!q1.empty() || !q2.empty())){
				if (!q1.empty()){
					q3.push(exis);
					exis = q1.top();
					q1.pop();
					proc_q3 = 0;
					proc_q1 = 1;
					slice = q1_slice;
					prt = exis->prt;
                                	index = exis->index;
				} else if (!q2.empty()){
					q3.push(exis);
                                        exis = q2.top();
                                        q2.pop();
                                        proc_q3 = 0;
                                        proc_q2 = 1;
                                        slice = q2_slice;
					prt = exis->prt;
                                	index = exis->index;
				}
			}
		}


		if (index != -1){
		
			(exis->rem_time)--;
			slice--;

			t2 = t1 + to_string(ts) + "-" + to_string(exis->index) + "-" + to_string(ts+1);
			t3 = t1 + to_string(ts) + "---" + to_string(ts+1);
			if(proc_q1 == 1){
				chart[0].push_back(t2);
				chart[1].push_back(t3);
				chart[2].push_back(t3);
			} else if (proc_q2 == 1){
				chart[0].push_back(t3);
                                chart[1].push_back(t2);
                                chart[2].push_back(t3);
			} else if (proc_q3){
				chart[0].push_back(t3);
                                chart[1].push_back(t3);
                                chart[2].push_back(t2);
			}

			if (exis->rem_time == 0){
				c_s = 0;
				num_proc++;
				slice = 0;
				proc_q1 = 0;
				proc_q2 = 0;
				proc_q3 = 0;
				prt = INT_MAX;
				index = -1;
				exis->wait_time = ts + 1 - exis->arrival_time - exis->burst_time;
				exis->trn_arnd_time = ts + 1;
				sum_w += exis->wait_time;
				sum_t += exis->trn_arnd_time;
			} else if (slice == 0){
				if (proc_q1 == 1){
					t_prt = exis->prt;
					t_prt = (q2_lp - (t_prt%q2_lp) -1) + t_prt + q2_lp;
					exis->prt = t_prt;
					q1.push(exis);
				} else if (proc_q2 == 1){
					t_prt = exis->prt;
                                        t_prt = (q3_lp - (t_prt%q3_lp) -1) + t_prt + q3_lp;
                                        exis->prt = t_prt;
					q2.push(exis);
				} else if (proc_q3 == 1){
					q3.push(exis);
					//do nothing
				}

				prt = INT_MAX;
                                index = -1;
				proc_q1 = 0;
				proc_q2 = 0;
				proc_q3 = 0;
				c_s = 0;
			}
		} else {
			t2 = t1 + to_string(ts) + "---" + to_string(ts+1);
			chart[0].push_back(t2);
			chart[1].push_back(t2);
			chart[2].push_back(t2);

		}

		if (ageing){
			for (int i=0;i<n;i++){
				if((p+i)->prt != 0){
					if((p+i)->arrival_time < ts && (p+i)->prt > q2_lp){
						if ((p+i)->index != index && (p+i)->wait_time == -1){
							(p+i)->prt -= 1;
						}
					}
				}
			}
		}

		cout << "Queue 1 - Priority Based Premtive - Time quanta:" << q1_slice << " Priorities: " << q1_lp << "-" << q2_lp - 1 << endl;
		cout << "Queue 2 - Round Robin - Time quanta:" << q2_slice << " Priorities: " << q2_lp << "-" << q3_lp - 1 << endl;
		cout << "Queue 3 - FCFS - Time quanta:" << q3_slice << " Priorities: " << q3_lp << "-" << 39 << endl;
		cout << "Completed Processes are: "<< num_proc << endl;
		/*print_details(p,n);
		prnt_chart(chart);*/
		cin.get();
		ts++;

	}

	print_details(p,n);
	prnt_chart(chart);	
	float avg_w = float(sum_w)/n;
	float avg_t = float(sum_t)/n;
	printf("Average Waiting time: %f\n",avg_w);
	printf("Average Turn Around time: %f\n", avg_t);

	/*for (int i=0;i<n;i++){
		q2.push(p[i]);
	}

	q2.rem(2);

	while (!q2.empty()){
		PROC temp->= q2.top();
		printf("%d\n",temp->index);
		q2.pop();
	}*/
}

int main(int argc,char **argv){

	int n = 0;
	PROC *p = NULL;

	printf("Enter number of Processes, their attributes will be choosen in random manner\n");
	scanf("%d",&n);
	
	p = (PROC *)malloc(sizeof(PROC) *n);

	p = generate_proc_data(p,n);
	print_details(p,n);
	
	
	/*printf("****************************************************************\n");
	fifo_sched(p,n);
	printf("****************************************************************\n");
	sjf_sched(p,n);
	printf("****************************************************************\n");
	rr_sched(p,n);
	printf("****************************************************************\n");
	prt_sched(p,n);
	printf("****************************************************************\n");
	mlq_sched(p,n);*/
	printf("****************************************************************\n");
	mlfq_sched(p,n);
	printf("****************************************************************\n");
	//linux_sched(p,n);
	printf("****************************************************************\n");

	return 0;
}
