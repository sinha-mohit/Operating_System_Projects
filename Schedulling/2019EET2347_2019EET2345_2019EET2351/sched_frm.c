#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

char array[5][20] = {
    "System",
    "Interactive",
};

typedef struct _node{
    char *process_type;
	int index;
	int burst_time;
	int arrival_time;
	int wait_time;
	int trn_arnd_time;
	int prt;
	int flag;
} PROC;


PROC* generate_proc_data(PROC *s,int n){

	srand(time(0));
	for(int i=0;i<n;i++){
        (s+i)->process_type = array[rand()%2];
		(s+i)->index = i;
		(s+i)->burst_time = rand()%100;
		(s+i)->arrival_time = rand()%100;
		(s+i)->wait_time = -1;
		(s+i)->trn_arnd_time = -1;
		(s+i)->prt = rand()%100;
		(s+i)->flag = 0;
	}

	return s;
}


void print_details(PROC *s,int n){

	printf("+*******+************+**************+**************+*****************+**********+\n");
	printf("| INDEX | Burst Time | Arrival Time | Waiting Time | Turnaround Time | Priority |\n");
	printf("+*******+************+**************+**************+*****************+**********+\n");

	for(int i=0;i<n;i++){
		printf("%d\t\t",(s+i)->index);
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


void fifo_sched(PROC *p,int n){

	int sum_wait, sum_trn_arnd, disp, i;
	float avg_wait, avg_trn_arnd;
    PROC *temp_p = (PROC *)malloc(sizeof(PROC) *n);

    for(i=0;i<n;i++)
		temp_p[i]=p[i];

	sort_arrival(temp_p,n);

	sum_wait = temp_p->wait_time = 0;
	sum_trn_arnd = temp_p->trn_arnd_time = temp_p->burst_time;

	for(i = 1;i < n;i++){
		if(((temp_p+i)->wait_time = (temp_p+i-1)->trn_arnd_time + (temp_p+i-1)->arrival_time - (temp_p+i)->arrival_time) < 0)
            (temp_p+i)->wait_time = 0;
		(temp_p+i)->trn_arnd_time = ((temp_p+i)->wait_time + (temp_p+i)->burst_time);
		sum_wait += (temp_p+i)->wait_time;
		sum_trn_arnd += (temp_p+i)->trn_arnd_time;
	}

	avg_wait = (float)sum_wait/n;
	avg_trn_arnd = (float)sum_trn_arnd/n;

	print_details(temp_p,n);

	printf("\n\n GANTT CHART\n     ");
	for(int i = 0;i < n;i++){
		printf("%-8d", (temp_p+i)->index);
	}

	printf("\n ");

	printf("%d\t", temp_p->arrival_time);
	disp = temp_p->arrival_time;
	for(int i = 1;i <= n;i++){
		disp += (temp_p+i-1)->burst_time;
		printf("%-8d", disp);
	}

	printf("\n\n Average waiting time = %0.2f\n Average turn-around = %0.2f.", avg_wait, avg_trn_arnd);
    free(temp_p);
}


void rr_sched(PROC *p, int n)
{
	int process_count=0, t, curr_time=0, k = 0, i, *temp_bt;
	int qt = 10; // quantum time
	int sum_wait=0, sum_trn_arnd=0;
	float avg_wait, avg_trn_arnd;
	PROC *temp_p;

	temp_p = (PROC *)malloc(sizeof(PROC)*n);

	for(i=0; i<n; i++)
		*(temp_p+i) = *(p+i);

	sort_arrival(temp_p, n);

	temp_bt = (int *)malloc(sizeof(int)*n);

	for(i=0; i<n; i++)
		*(temp_bt+i) = (temp_p+i)->burst_time;

    printf(" GANTT CHART \n\n");
    printf(" Current time \t Process index\n");
    curr_time = temp_p->arrival_time;
    do{
        if(k > n-1)
			k = 0;

        if(curr_time < (temp_p+k)->arrival_time) // mtlb agar uss process ka arrival time aaye hi nhi to aage wali process pe jao
                k = 0;

		if((temp_p+k)->burst_time > 0)
			printf("    %d \n\t\t      %d \n", curr_time, (temp_p+k)->index);

		t=0;

		while(t<qt && (temp_p+k)->burst_time > 0){
			t++;
			curr_time++;
			(temp_p+k)->burst_time--;
		}

		if((temp_p+k)->burst_time <= 0 && (temp_p+k)->flag != 1){
			(temp_p+k)->wait_time = curr_time - temp_bt[k] - (temp_p+k)->arrival_time;
			(temp_p+k)->trn_arnd_time = curr_time - (temp_p+k)->arrival_time;
			process_count++; // count for number of processes finished
			(temp_p+k)->flag = 1; // flag denoting the completion of process
			sum_wait += (temp_p+k)->wait_time;
			sum_trn_arnd += (temp_p+k)->trn_arnd_time;
		}

		if(k<n-1 && (curr_time < (temp_p+k+1)->arrival_time)){
            i = k;
            while((temp_p+i)->flag == 1 && i >= 0) // ye loop se pata chalega ki isse pichli saare processes khatam ho gayi h ya nhi
                i--;
            if(i < 0){// agar isse pehle ki saari processes khatam ho gayi h to wait for next process
                while(curr_time < (temp_p+k+1)->arrival_time) // example, at1-0 bt1-8, at2-22 bt2-anything
                    curr_time++;
            }
		}

		k++;
	}while(process_count != n);

	printf("    %d", curr_time);
	avg_wait = (float)sum_wait/n;
	avg_trn_arnd = (float)sum_trn_arnd/n;
	printf("\n\n Average waiting time = %0.2f\n Average turn-around = %0.2f.", avg_wait, avg_trn_arnd);
}


void mlq_sched(PROC *p, int n) // jab bhi program extend kro 5 process_type k liye upar  jake generate_proc_data me %5 kr dena
{
    int curr_time, i, process_count = 0, j = 0, sum_wait = 0, sum_trn_arnd = 0, t, k = 0;
    int qt = 10;
    float avg_wait, avg_trn_arnd;
    PROC *temp_p;

	temp_p = (PROC *)malloc(sizeof(PROC)*n);
	for(i=0; i<n; i++)
		*(temp_p+i) = *(p+i);

    sort_arrival(temp_p, n);

    int *temp_bt = (int *)malloc(sizeof(int)*n);
    for(i=0; i<n; i++)
        temp_bt[i] = temp_p[i].burst_time;

    curr_time = temp_p->arrival_time;
    printf("\n\n GANTT CHART \n\n");
    printf(" Current time \t Process index\n");

    do{

        if(!strcmp((temp_p+k)->process_type, "System") && (temp_p+k)->flag != 1){

            temp_p[k].wait_time = curr_time - temp_p[k].arrival_time;
            temp_p[k].trn_arnd_time = temp_p[k].wait_time + temp_p[k].burst_time;
            sum_wait += temp_p[k].wait_time;
            sum_trn_arnd += temp_p[k].trn_arnd_time;
            printf("    %d \n\t\t      %d \n", curr_time, (temp_p+k)->index);
            curr_time += temp_p[k].burst_time;
            (temp_p+k)->flag = 1; // flag denoting the completion of process
            process_count++;
        }

        if(!strcmp((temp_p+k)->process_type, "Interactive") && (temp_p+k)->flag != 1){

            if(curr_time < (temp_p+k)->arrival_time) // mtlb agar uss process ka arrival time aaye hi nhi aur pichli kisi process ka burst time baaki h
            {                                           //to aage wali process pe jao
                k = 0;
                continue;
            }

            if((temp_p+k)->burst_time > 0)
                printf("    %d \n\t\t      %d \n", curr_time, (temp_p+k)->index);

            t=0;

            while(t<qt && (temp_p+k)->burst_time > 0){
                t++;
                curr_time++;
                (temp_p+k)->burst_time--;
            }

            if((temp_p+k)->burst_time <= 0){
                (temp_p+k)->wait_time = curr_time - temp_bt[k] - (temp_p+k)->arrival_time;
                (temp_p+k)->trn_arnd_time = curr_time - (temp_p+k)->arrival_time;
                process_count++; // count for number of processes finished
                (temp_p+k)->flag = 1;
                sum_wait += (temp_p+k)->wait_time;
                sum_trn_arnd += (temp_p+k)->trn_arnd_time;
            }
        }

        if(k<n-1 && (curr_time < (temp_p+k+1)->arrival_time)){
            i = k;
            while((temp_p+i)->flag == 1 && i >= 0) // ye loop se pata chalega ki isse pichli saare processes khatam ho gayi h ya nhi
                i--;
            if(i < 0){// agar isse pehle ki saari processes khatam ho gayi h to wait for next process
                while(curr_time < (temp_p+k+1)->arrival_time) // example, at1-0 bt1-8, at2-22 bt2-anything
                    curr_time++;
            }
		}

		k++;
        if(k > n-1)
            k = 0;

    }while(process_count != n);

    printf("    %d", curr_time);

    avg_wait = (float)sum_wait/n;
    avg_trn_arnd = (float)sum_trn_arnd/n;

    printf("\n\n Average waiting time = %0.2f\n Average turn-around = %0.2f.", avg_wait, avg_trn_arnd);
}



int main(int argc,char **argv){

	int n = 0;
	PROC *p = NULL;

	printf("Enter number of Processes, their attributes will be chosen in random manner\n");
	scanf("%d",&n);

	p = (PROC *)malloc(sizeof(PROC) *n);

	p = generate_proc_data(p,n);
	print_details(p,n);


	printf("****************************************************************\n");
	fifo_sched(p,n);
	printf("\n\n****************************************************************\n\n");
	/*sjf_sched(p,n);
	printf("****************************************************************\n");
	*/
	rr_sched(p,n);
	printf("\n\n****************************************************************\n\n");

	for(int i=0 ; i<n ; i++){
        printf(" %s ", (p+i)->process_type);
	}
	/*
	prt_sched(p,n);
	printf("****************************************************************\n");
	*/
	mlq_sched(p,n);
	printf("\n\n****************************************************************\n");
	/*
	mlfq_sched(p,n);
	printf("****************************************************************\n");
	linux_sched(p,n);
	printf("****************************************************************\n");*/

	return 0;
}
