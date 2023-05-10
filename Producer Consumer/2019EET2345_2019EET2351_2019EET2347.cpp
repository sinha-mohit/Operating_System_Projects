#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <deque>
#include <condition_variable>
using namespace std;

#define random 4
#define lower 1
#define upper 6

#define RED_COLOR "\x1B[31m"
#define GREEN_COLOR "\x1B[32m"
#define BLUE_COLOR "\x1B[34m"
#define YELLOW_COLOR "\x1B[33m"
#define COLOR_RESET "\x1B[0m"

std::mutex mu;
std::condition_variable cond;
deque<int> buffer;
vector<int> customerArrivalTime;
vector<int> customerExecutionTime;
unsigned int maxBufferSize;
int currTime = 0;
int customerNo_cs = 0;


string tim()
{
    time_t my_time = time(NULL);
    return ctime(&my_time);
}


void que_stat(){

	cout << endl << "Queue Status" << endl;
	for(auto it = buffer.begin();it!=buffer.end();it++){
		cout << *it << "--";
	}

	cout << "end" << endl;
}


void customer(int val){
    int customerNo = 0;
    //int currTime = 0;
    int index = 0;
    //while(customerNo <= val){
    while(customerNo < val){
        std::this_thread::sleep_for(chrono::seconds(customerArrivalTime[index] - currTime));
        //cout << endl << "Arrived Customer: " << customerNo << endl;
        currTime = customerArrivalTime[index++];


        cout << endl << BLUE_COLOR "Arrived Customer: " << customerNo << " at time "<< tim() << COLOR_RESET;

        std::unique_lock<std::mutex> locker(mu);
        if(buffer.size() == maxBufferSize) {
	    que_stat();
            cout << endl << RED_COLOR "    Queue is Full right now! " << "Customer "<< customerNo << " has to go away." << COLOR_RESET<<endl;
            customerNo_cs++;
            customerNo++;
            locker.unlock();
            continue;
        }
        cond.wait(locker, [](){return buffer.size() < maxBufferSize;}); //if buffer.size() < maxBufferSize then mutex lock else mutex unlock
        buffer.push_back(customerNo);
        customerNo++;
        locker.unlock();
        cond.notify_all(); //??
    }
}

void counterService(int c_num, int val){
    int cnt = 0;
    bool wake_up;
    //while(true){
    while(customerNo_cs < val){
    //while(!buffer.empty()){
        std::unique_lock<std::mutex> locker(mu);
        que_stat();
        wake_up = buffer.empty();
        cond.wait(locker, [](){return buffer.size() > 0;}); //if buffer.size() > maxBufferSize then mutex lock else mutex unlock if nothing is in the buffer
        if(wake_up)
            cout<< endl << YELLOW_COLOR  "\t Counter No "<< c_num << " Wake up call by Customer " << buffer.front() << COLOR_RESET;
        cout << endl << GREEN_COLOR "\t Counter No "<< c_num << " Servicing Customer: " << buffer.front() << " at time " << tim() << COLOR_RESET;
        cnt = buffer.front();
        buffer.pop_front();
        customerNo_cs++;
        locker.unlock();
        cond.notify_one();
        //std::this_thread::sleep_for(chrono::seconds(rand() % 15 + 1));
        std::this_thread::sleep_for(chrono::seconds(customerExecutionTime[cnt]));
        cout << endl << GREEN_COLOR "\t Counter No "<< c_num << " Served customer " << cnt << " at " << tim() << COLOR_RESET;
	locker.lock();
	wake_up = buffer.empty();
	if(wake_up)
		cout<< endl << YELLOW_COLOR  "\t Queue empty Counter No "<< c_num << " Closed " <<COLOR_RESET << endl;
	locker.unlock();
    }
}

int main(){
    cout << "Enter the length of the Queue: " << endl;
    cin >>  maxBufferSize;

    cout << "Enter number of Customers: " << endl;
    int numberOfCustomers;
    cin >> numberOfCustomers;

    cout << "Enter number of Counters between 1 and 2: " << endl;
    int counter;
    cin >> counter;

    while(counter != 1 && counter != 2){
        cout << endl << "please Enter the Counter No. between 1 and 2 only: " << endl;
        cin >> counter;
    }

    customerArrivalTime.resize(numberOfCustomers);
    customerExecutionTime.resize(numberOfCustomers);

	srand(time(0));
	for(int i = 0; i < numberOfCustomers; i++)
	{
		if(i == 0)
		{
			customerArrivalTime[i] = rand() % upper + lower;
		}
		else
		{
			customerArrivalTime[i] = rand() % upper + customerArrivalTime[i - 1];
		}
		customerExecutionTime[i] = rand() % 15 + 1;
	}

	printf("\nArrival Times :");
	for (int i = 0; i < numberOfCustomers; ++i)
	{
		printf("%d\t", customerArrivalTime[i]);
	}

	printf("\nExecution Times :");
	for (int i = 0; i < numberOfCustomers; ++i)
	{
		printf("%d\t", customerExecutionTime[i]);
	}

	printf("\n");
	printf("\n");

    std::thread t1(customer, numberOfCustomers);

    if(counter == 2){
        std::thread t2(counterService,1,numberOfCustomers);
        std::thread t3(counterService,2,numberOfCustomers);

        t1.join();
        t2.join();
        t3.join();
    }
    else{
        std::thread t2(counterService,1,numberOfCustomers);
        t1.join();
        t2.join();
    }

    return 0;
}
