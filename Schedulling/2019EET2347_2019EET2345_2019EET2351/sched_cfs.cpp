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

#define MGD 4
#define TLD 20

using namespace std;


typedef struct _node{
	int index;
	int burst_time;
	int arrival_time;
	int rem_time;
	double wait_time;
	double trn_arnd_time;
	int nyc;
	double vrt;
	int qued;
	double tsl;
	double art;
} PROC;

static const int nyc_to_w[40] = {
 /*- 20 */     88761,     71755,     56483,     46273,     36291,
 /*-15 */     29154,     23254,     18705,     14949,     11916,
 /*- 10 */      9548,      7620,      6100,      4904,      3906,
 /* -5 */      3121,      2501,      1991,      1586,      1277,
 /*   0 */      1024,       820,       655,       526,       423,
 /*   5 */       335,       272,       215,       172,       137,
 /* 10 */       110,        87,        70,        56,        45,
 /*  15 */        36,        29,        23,        18,        15,
};


PROC* generate_proc_data(PROC *s,int n){

	srand(time(0));
	for(int i=0;i<n;i++){
		(s+i)->index = i;
		(s+i)->burst_time = ((rand())%30) + 1;
		(s+i)->arrival_time = rand()%10;
		(s+i)->wait_time = -1;
		(s+i)->trn_arnd_time = -1;
		(s+i)->rem_time = (s+i)->burst_time;
		(s+i)->nyc = rand()%40;
		(s+i)->vrt = 0;
		(s+i)->qued = 0;
		(s+i)->tsl = 0;
		(s+i)->art = 0;

	}

	return s;
}

void print_details(PROC *s,int n){

	printf("+*******+*********+***********+*******+**********+****************+********+**********+***********+**************+************+\n");
	printf("| INDEX | Burst T | Arrival T | Rem T | Waiting T |    TArnd T    |   NI   |  Vrun T  |    Que    |    TSlice    |    ART     |\n");
	printf("+*******+*********+***********+*******+**********+****************+********+**********+***********+**************+********:****+\n");

	for(int i=0;i<n;i++){
		printf("\t%d\t",(s+i)->index);
		printf("%d\t",(s+i)->burst_time);
		printf("%d\t",(s+i)->arrival_time);
		printf("%d\t",(s+i)->rem_time);
		printf("%0.6f\t",(s+i)->wait_time);
		printf("%0.6f\t",(s+i)->trn_arnd_time);
		printf("%d\t",(s+i)->nyc);
		printf("%0.6f\t",(s+i)->vrt);
		printf("%d\t",(s+i)->qued);
		printf("%0.6f\t",(s+i)->tsl);
		printf("%0.6f\n",(s+i)->art);
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

void prnt_chart(vector<string>  x){
	for (unsigned int i = 0; i < x.size(); i++) {
		cout << x[i];
	}
	cout << endl;
}

void prnt_rbt(multimap<double,PROC*> x){

	cout << "RB Tree : ";
	for (auto itr = x.begin(); itr != x.end(); ++itr) {
                cout << itr->first
                        << ',' << itr->second->index << "\t";
        }

	cout << endl;

}

double calc_tsl(multimap<double,PROC*> x,int nic,int n){

	double tl = (double)max(TLD,n*MGD);
	double wt_sum = 0;
	double tsl;
	PROC *temp = NULL;

	for (auto it = x.begin();it != x.end(); ++it){
		temp = it->second;
		wt_sum += (double)nyc_to_w[temp->nyc];
	}

	tsl = double(tl) * ((double)nyc_to_w[nic]/(double)wt_sum);
	return tsl;
}

double min_vrtm(multimap <double,PROC*> x){

	if (x.size() != 0){
		auto it = x.begin();
		return it->second->vrt;
	}

	return 0;
}


void linux_sched(PROC *p,int n){

	double sum_w,sum_t;
	vector<string> chart;
	multimap<double,PROC*> rbt;
	PROC *curr,*temp;
	int c_s = 0;
	double min_vrt = 0;
	int num_proc = 0;
	int ts = 0;
	int index = -1;
	string t1 = "|";
        string t2,t3;
	//int flag = 0;

	sort_arrival(p,n);
	cin.get();

	while(num_proc != n){

		print_details(p,n);
                prnt_rbt(rbt);
                prnt_chart(chart);

		for (int i=0;i<n;i++){
			
			if ((p+i)->arrival_time == ts){
				if(min_vrtm(rbt) >= MGD/2){
					(p+i)->vrt = (double)min_vrtm(rbt) -(double)(MGD/2);
				}

				/*if (flag == 0){
					(p+i)->vrt = (double)(MGD/2);
					flag = 1;
				}*/
				(p+i)->qued = 1;
				rbt.insert({(p+i)->vrt,(p+i)});
			}
		}


		/*print_details(p,n);
		prnt_rbt(rbt);
                prnt_chart(chart);*/


		if (c_s == 0){

			if(rbt.size() > 0){
				auto it = rbt.begin();
			        curr = it->second;
				min_vrt = curr->vrt;
				curr->tsl = calc_tsl(rbt,curr->nyc,rbt.size());
				rbt.erase(it);
				c_s = 1;
				curr->art = 0;
				index = curr->index;
			}

		} else if (c_s == 1){

			if(rbt.size() > 0){
				
				if (curr->art >= MGD){
					
					auto itr = rbt.begin();
					temp = itr->second;
					
					if (temp->vrt <= min_vrt){
						
						curr->vrt += (double)curr->art * (double)(nyc_to_w[0]/nyc_to_w[curr->nyc]);
						curr->art = curr->tsl = 0;
						rbt.insert({curr->vrt,curr});
						curr = temp;
						temp = NULL;
						curr->tsl = calc_tsl(rbt,curr->nyc,rbt.size());
						rbt.erase(itr);
						curr->art = 0;
						min_vrt = curr->vrt;
						index = curr->index;
					}
				}
			}
		}

		if (index != -1){

			(curr->rem_time)--;
			(curr->tsl)--;
			(curr->art)++;

			t2 = t1 + to_string(ts) + "-" + to_string(curr->index) + "-" + to_string(ts+1);
			chart.push_back(t2);

			if(curr->rem_time <= 0){
				
				num_proc++;
				min_vrt = 0;
				index = -1;
				c_s = 0;
				curr->wait_time = double(ts) + 1 - double(curr->arrival_time) - double(curr->burst_time);
				curr->trn_arnd_time = double(ts) + 1;
				sum_w += curr->wait_time;
				sum_t += curr->trn_arnd_time;
				curr->art = 0;
				curr->tsl = 0;
			} else if (curr->tsl <= 0){
				
				min_vrt = 0;
                                index = -1;
                                c_s = 0;
				curr->vrt += (double)curr->art * (double)(nyc_to_w[0]/nyc_to_w[curr->nyc]);
                                curr->art = curr->tsl = 0;
                                rbt.insert({curr->vrt,curr});
			}
		} else {
			//chart
			t2 = t1 + to_string(ts) + "---" + to_string(ts+1);
                        chart.push_back(t2);
		}

		cout << "Minimum Granularity: " << MGD << " Targeted Latency Default: " << TLD << endl;
		cout << "Nice Values : 0-39" << endl;
		cin.get();
		ts++;
	}


	print_details(p,n);
	prnt_rbt(rbt);
        prnt_chart(chart);
        double avg_w = float(sum_w)/n;
        double avg_t = float(sum_t)/n;
        printf("Average Waiting time: %f\n",avg_w);
        printf("Average Turn Around time: %f\n", avg_t);
	

	/*for (int i=0;i<n;i++){
		rbt.insert({(p+i)->arrival_time,(p+i)});
	}

	for (auto itr = rbt.begin(); itr != rbt.end(); ++itr) { 
		cout << itr->first 
			<< '\t' << itr->second << '\n'; 
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
	mlq_sched(p,n);
	printf("****************************************************************\n");
	mlfq_sched(p,n);*/
	printf("****************************************************************\n");
	linux_sched(p,n);
	printf("****************************************************************\n");

	return 0;
}
