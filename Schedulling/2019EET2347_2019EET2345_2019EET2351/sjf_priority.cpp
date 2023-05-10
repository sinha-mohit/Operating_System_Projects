#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

typedef struct _node{
	int index;
	int burst_time;
	int arrival_time;
	int wait_time;
	int trn_arnd_time;
	int prt;
} PROC;


PROC* generate_proc_data(PROC *s,int n){

	srand(time(0));
	for(int i=0;i<n;i++){
		(s+i)->index = i;
		(s+i)->burst_time = 1 + (rand())%30;
		(s+i)->arrival_time = rand()%10;
		(s+i)->wait_time = -1;
		(s+i)->trn_arnd_time = -1;
		(s+i)->prt = rand()%40;
	}

	return s;
}

void print_details(PROC *s,int n){

	printf("+********+************+**************+**************+*****************+************+\n");
	printf("|  INDEX | Burst Time | Arrival Time | Waiting Time | Turnaround Time |  Priority  |\n");
	printf("+********+************+**************+**************+*****************+************+\n");

	for(int i=0;i<n;i++){
		printf(" P[%d]\t\t",(s+i)->index);
		printf("%d\t\t",(s+i)->burst_time);
		printf("%d\t\t",(s+i)->arrival_time);
		printf("%d\t\t",(s+i)->wait_time);
		printf("%d\t\t",(s+i)->trn_arnd_time);
		printf("%d\n",(s+i)->prt);
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


void sjf_sched(PROC *p,int n){

	vector<vector<int>> v(n, vector<int> (7, 0));

    for(int i = 0; i < n; i++){
        v[i][0] = (p+i)->index;
        v[i][1] = (p+i)->arrival_time;
        v[i][2] = (p+i)->burst_time;
		v[i][6] = (p+i)->prt;
    }


    int t;
    for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n - 1; j++)
		{
			if (v[j][1] > v[j + 1][1])
			{
				for (int k = 0; k < 7; k++)
				{
                    t = v[j][k];
                    v[j][k] = v[j + 1][k];
                    v[j + 1][k] = t;
				}
			}
		}
	}

    int temp, temp2, t1, t2, t3, t4, t5, t6;

	t1 = v[0][1];
	t2 = v[0][2];
	v[0][3] = t1 + t2;
	t3 = t1 + t2;
	v[0][5] = t3 - v[0][1];
	t5 = t3 - t1;
	v[0][4] = v[0][5] - v[0][2];
	t4 = t5 - t2;
	t4++;
	
	int x;
	for (int i = 1; i < n; i++)
	{
		temp = v[i - 1][3];
		t6 = temp;
		int low = v[i][2];
		for (int j = i; j < n; j++)
		{
			if (t6 >= v[j][1] && low >= v[j][2])
			{
				low = v[j][2];
				x = j;
			}
		}
		v[x][3] = t6 + v[x][2];
		v[x][5] = v[x][3] - v[x][1];
		v[x][4] = v[x][5] - v[x][2];


		for (int k = 0; k < 7; k++)
		{
            temp2 = v[x][k];
            v[x][k] = v[i][k];
            v[i][k] = temp2;
		}
	}

	sort_arrival(p,n);
    int sum_wait = 0,sum_trn_arnd = 0;
	float avg_wait,avg_trn_arnd;

	PROC *z = NULL; 
	z = (PROC *)malloc(sizeof(PROC) *n);

    for(int i = 0; i < n; i++){
        (z+i)->index = v[i][0];
        (z+i)->arrival_time = v[i][1];
        (z+i)->burst_time = v[i][2];
        (z+i)->wait_time = v[i][4];
        sum_wait += v[i][4];
        sum_trn_arnd += v[i][5];
        (z+i)->trn_arnd_time = v[i][5];
		(z+i)->prt = v[i][6];
    }

    avg_wait = sum_wait/(float)n;
	avg_trn_arnd = sum_trn_arnd/(float)n;

	sort_arrival(z,n);
	print_details(z,n);

	int disp = 0;
	/*printf("\n\n GANTT CHART\n ");
	for(int i = 0;i < n;i++){
		printf("     %d     ",(z+i)->index);
	}

	printf("\n ");

	printf("0\t");
	for(int i = 1;i <= n;i++){
		disp += (z+i-1)->burst_time;
		printf("%d      ",disp);
	}*/

	printf("\n\n Average waiting time = %0.2f\n Average turn-around = %0.2f.",avg_wait,avg_trn_arnd);

	free(z);
}

////////////////////////////////////////////////////////

void solver(PROC* z, int n, int wt[]) 
{ 
	
	int* remain = new int[n];

	for (int i = 0; i < n; i++) {
		remain[i] = (z+i)->burst_time; 
	}

	int complete = 0, t = 0, minm = INT_MAX; 
	int shortest = 0, finish_time; 
	int flag = 0; 

	
	while (complete != n) { 

		
		for (int j = 0; j < n; j++) { 
			if (((z+j)->arrival_time <= t) && (remain[j] < minm) && remain[j] > 0) { 
				minm = remain[j]; 
				shortest = j; 
				flag = 1; 
			} 
		} 

		if (flag == 0) { 
			t++; 
			continue; 
		} 

		remain[shortest]--; 

		minm = remain[shortest]; 
		if (minm == 0) 
			minm = INT_MAX; 

		if (remain[shortest] == 0) { 

			complete++; 
			flag = 0; 

			finish_time = t + 1; 

			wt[shortest] = finish_time - (z+shortest)->burst_time - (z+shortest)->arrival_time; 

			if (wt[shortest] < 0) 
				wt[shortest] = 0; 
		} 
		t++; 
	} 

	delete[] remain;
} 






void sjf_pre_sched(PROC *p,int n){
	
	PROC *z = NULL;
	z = (PROC *)malloc(sizeof(PROC) *n);

	for(int i = 0; i < n; i++){
		*(z+i) = *(p+i);
	}


	int wt[n], tat[n], total_wt = 0, 
	total_tat = 0; 

	solver(z, n, wt); 

	for (int i = 0; i < n; i++) {
		tat[i] = z[i].burst_time + wt[i]; 
	}

	
	for (int i = 0; i < n; i++) { 
		total_wt = total_wt + wt[i]; 
		total_tat = total_tat + tat[i]; 
		(z+i)->wait_time = wt[i];
		(z+i)->trn_arnd_time = tat[i];
	} 

	print_details(z,n);

	int disp = 0;
	/*printf("\n\n GANTT CHART\n ");
	for(int i = 0;i < n;i++){
		printf("     %d     ",(z+i)->index);
	}

	printf("\n ");

	printf("0\t");
	for(int i = 1;i <= n;i++){
		disp += (z+i-1)->burst_time;
		printf("%d      ",disp);
	}*/

	printf("\n\n Average waiting time = %0.2f\n Average turn-around = %0.2f.",(float)total_wt / (float)n,(float)total_tat / (float)n);

	free(z);
	
}

void fifo_sched(PROC *p,int n){

	int sum_wait,sum_trn_arnd,disp;
	float avg_wait,avg_trn_arnd;
	sort_arrival(p,n);

	sum_wait = p->wait_time = 0;
	sum_trn_arnd = p->trn_arnd_time = p->burst_time - p->arrival_time;

	for(int i = 1;i < n;i++){
		(p+i)->wait_time = ((p+i-1)->trn_arnd_time + (p+i-1)->arrival_time) - (p+i)->arrival_time;
		(p+i)->trn_arnd_time = ((p+i)->wait_time + (p+i)->burst_time);
		sum_wait += (p+i)->wait_time;
		sum_trn_arnd += (p+i)->trn_arnd_time;
	}

	avg_wait = sum_wait/n;
	avg_trn_arnd = sum_trn_arnd/n;

	print_details(p,n);

	/*printf("\n\n GANTT CHART\n ");
	for(int i = 0;i < n;i++){
		printf("     %d     ",(p+i)->index);
	}

	printf("\n ");

	printf("0\t");
	for(int i = 1;i <= n;i++){
		disp += (p+i-1)->burst_time;
		printf("%d      ",disp);
	}*/

	printf("\n\n Average waiting time = %0.2f\n Average turn-around = %0.2f.\n\n",avg_wait,avg_trn_arnd);

}


void prt_main_func(_node proc[], int n, int wt[])
{
	int* rt = new int[n];
	int* pri = new int[n];

	for (int i = 0; i < n; i++) {
		rt[i] = proc[i].burst_time;
		pri[i] = proc[i].prt;
		if (rt[i] == 0) {
			rt[i] = 1;
		}
	}

	int complete = 0, t = 0, max_pri = INT_MIN;
	int largest = 0, finish_time;
	bool check = false;

	while (complete != n) {

		for (int j = 0; j < n; j++) {
			if ((proc[j].arrival_time <= t) && (pri[j] > max_pri) && rt[j] > 0) {
				max_pri = pri[j];
				largest = j;
				check = true;
			}
		}

		if (check == false) {
			t++;
			continue;
		}

		rt[largest]--;

	
		if (rt[largest] == 0) {

			complete++;
			check = false;
			max_pri = INT_MIN;

			finish_time = t + 1;

			wt[largest] = finish_time - proc[largest].burst_time - proc[largest].arrival_time;

			if (wt[largest] < 0)
				wt[largest] = 0;
		}
		t++;
	}

	delete[] rt;
	delete[] pri;
}

void prt_sched_static(PROC* p, int n){
	
	PROC *z = NULL;
	z = (PROC *)malloc(sizeof(PROC) *n);

	for(int i = 0; i < n; i++){
		*(z+i) = *(p+i);
	}



	int wt[n], tat[n], total_wt = 0, 
	total_tat = 0; 

	prt_main_func(z, n, wt); 

	for (int i = 0; i < n; i++) {
		tat[i] = z[i].burst_time + wt[i]; 
	}

	for (int i = 0; i < n; i++) { 
		total_wt = total_wt + wt[i]; 
		total_tat = total_tat + tat[i]; 
		(z+i)->wait_time = wt[i];
		(z+i)->trn_arnd_time = tat[i];
	} 

	print_details(z,n);

	int disp = 0;
	/*printf("\n\n GANTT CHART\n ");
	for(int i = 0;i < n;i++){
		printf("     %d     ",(z+i)->index);
	}

	printf("\n ");

	printf("0\t");
	for(int i = 1;i <= n;i++){
		disp += (z+i-1)->burst_time;
		printf("%d      ",disp);
	}*/

	printf("\n\n Average waiting time = %0.2f\n Average turn-around = %0.2f.",(float)total_wt / (float)n,(float)total_tat / (float)n);

	free(z);

}

///////////////////////////////////////////////////////


void prt_main_func2(_node proc[], int n, int wt[])
{
	int* rt = new int[n];
	int* pri = new int[n];

	for (int i = 0; i < n; i++) {
		rt[i] = proc[i].burst_time;
		pri[i] = proc[i].prt;
		if (rt[i] == 0) {
			rt[i] = 1;
		}
	}

	int complete = 0, t = 0, max_pri = INT_MIN;
	int largest = 0, finish_time;
	bool check = false;

	while (complete != n) {

		for (int j = 0; j < n; j++) {
			if ((proc[j].arrival_time <= t) && (pri[j] > max_pri) && rt[j] > 0) {
				max_pri = pri[j];
				largest = j;
				check = true;
			}
		}

		if (check == false) {
			t++;
			continue;
		}

		rt[largest]--;

		if (rt[largest] == 0) {

			complete++;
			check = false;
			max_pri = INT_MIN;

			finish_time = t + 1;

			wt[largest] = finish_time - proc[largest].burst_time - proc[largest].arrival_time;

			if (wt[largest] < 0)
				wt[largest] = 0;
		}

		int mini_prt = INT_MAX, index_min = -1;
		for(int i = 0; i < n; i++){
			if(proc[i].arrival_time <= t && rt[i] > 0){
				if(mini_prt > pri[i]){
					mini_prt = pri[i];
					index_min = i;
				}
			}
		}

		if(index_min != -1){
			pri[index_min] = pri[index_min] + 1;
		}
		t++;
	}


	for (int i = 0; i < n; i++) {
		proc[i].prt = pri[i];
	}

	delete[] rt;
	delete[] pri;
}

void prt_sched_dynamic(PROC* p, int n){
	
	PROC *z = NULL;
	z = (PROC *)malloc(sizeof(PROC) *n);

	for(int i = 0; i < n; i++){
		*(z+i) = *(p+i);
	}



	int wt[n], tat[n], total_wt = 0, 
	total_tat = 0; 

	prt_main_func2(z, n, wt); 

	for (int i = 0; i < n; i++) {
		tat[i] = z[i].burst_time + wt[i]; 
	}

	for (int i = 0; i < n; i++) { 
		total_wt = total_wt + wt[i]; 
		total_tat = total_tat + tat[i]; 
		(z+i)->wait_time = wt[i];
		(z+i)->trn_arnd_time = tat[i];
	} 

	print_details(z,n);

	int disp = 0;
	/*printf("\n\n GANTT CHART\n ");
	for(int i = 0;i < n;i++){
		printf("     %d     ",(z+i)->index);
	}

	printf("\n ");

	printf("0\t");
	for(int i = 1;i <= n;i++){
		disp += (z+i-1)->burst_time;
		printf("%d      ",disp);
	}*/

	printf("\n\n Average waiting time = %0.2f\n Average turn-around = %0.2f.",(float)total_wt / (float)n,(float)total_tat / (float)n);

	free(z);

}







int main(int argc,char **argv){

	int n = 0;
	PROC *p = NULL;

	printf("Enter number of Processes, their attributes will be choosen in random manner\n");
	scanf("%d",&n);
	
	p = (PROC *)malloc(sizeof(PROC) *n);

	p = generate_proc_data(p,n);
	print_details(p,n);
	
	
	// printf("\n\n************************************fifo_sched*************************************************\n");
	// fifo_sched(p,n);
	// printf("\n**************************************************************************************************\n");


	printf("\n\n****************************sjf_sched(Non- preemptive)*****************************************\n");
	sjf_sched(p,n);
	printf("\n*************************************************************************************************\n");


	printf("\n\n****************************sjf_sched(preemptive)*****************************************\n");
	sjf_pre_sched(p,n);
	printf("\n********************************************************************************************\n");

	// rr_sched(p,n);
	// printf("****************************************************************\n");

	printf("\n\n************************Priority_Scheduling (static)*************************************\n");
	prt_sched_static(p,n);
	printf("\n*******************************************************************************************\n");


	printf("\n\n**************************Priority_Scheduling (Dynamic)*************************************\n");
	prt_sched_dynamic(p,n);
	printf("\n**********************************************************************************************\n");


	// mlq_sched(p,n);
	// printf("****************************************************************\n");
	// mlfq_sched(p,n);
	// printf("****************************************************************\n");
	// linux_sched(p,n);
	// printf("****************************************************************\n");

	free(p);

	return 0;
}
