#include <iostream>
#include <thread>
#include <semaphore>
#include <random>
#include <vector>
#include <algorithm>

using namespace std;

int philo_num;
vector<int> act(philo_num, 0);

semaphore fork[philo_num] = {1};


void think(int i){
	default_random_engine e(time(0) + i);
	double think_time = 9*(double)e() / (double)(e.max()-e.min()) + 1;
	if(act[i] != 0){
	act[i] = 0;
	for(int j = 0; j!=act.size(); ++j){
		if(act[j] == 0){
			cout<<"  ";
		}
		else{
			cout<<"* ";
		}
	}
	cout << endl;
	}
	sleep(think_time);
}

void eat(int i){
	default_random_engine e(time(0) + i);
	double eat_time = 2*(double)e() / (double)(e.max()-e.min()) + 1;
	act[i] = 1;
		for(int j = 0; j!=act.size(); ++j){
		if(act[j] == 0){
			cout<<"  ";
		}
		else{
			cout<<"* ";
		}
	}
	cout << endl;
	sleep(eat_time);
}

// Every philosopher should get the fork with smaller index first, then the fork with
// larger index, in order to avoid deadlock
void philosopher(int i){
	while(true){
		think(i);
		if(i != philo_num - 1){
		    wait(fork[i]);
			wait(fork[i+1]);
		}
		else{
			wait(fork[0]);
			wait(fork[i]);
		}
		eat(i);
		if(i != philo_num - 1){
			signal(fork[i+1]);
			signal(fork[i]);
		}
		else{
			signal(fork[i]);
			signal(fork[0]);
		}
	}
}



int main(int argc, char* argv[]){
	cout << "Enter the number of philosophers: " << endl;
	cin >> philo_num;           
	if (philo_num > 15){
		cout << "The max philosophers number should be 15" << endl;
		return 1;
	}
	if (philo_num <= 0){
		cout << "The number of philosophers cannot be zero or negative" << endl;
		return 1;
	}
	//vector<int> act(philo_num, 0);
	cout << "Eating Activity" << endl;
	if (philo_num <= 10){                                    // Output format
		for (int i = 0; i != philo_num; ++i){
			cout << i << " ";
		}
		cout << endl;
	}
	else{
		for (int i = 0; i != philo_num; ++i){
			if (i >= 10){
				cout << "1 ";
			}
			else{
				cout << "  ";
			}
		}
		cout << endl;
		for (int i = 0; i != philo_num; ++i){
			cout << i % 10 << " ";
		}
		cout << endl;
	}
	
	vector<thread> ths;
	for(int i = 0; i!=philo_num; ++i){
		ths.push_back(thread(philosopher, i));
	}
	for_each(ths.begin(), ths.end(), mem_fn(&thread::join));\
	
	return 0;
}