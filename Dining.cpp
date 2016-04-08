#include <iostream>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <unistd.h>
#include <random>
#include <vector>
#include <algorithm>

using namespace std;

int philo_num;                       // Number of philosophers
int act[15] = {0};                   // act[i] indicates the action of the ith philosopher, 0 is thinking, 1 is eating
int eat_times[15] = {0};             // The times of eating of philosophers
bool set_ter[15]={false};
sem_t forks[15];
sem_t output;                        // The output stream should not be interleaved

void think(int i){
  random_device rd;                  // Generate a random thinking time
  mt19937 e2(rd());
  uniform_real_distribution<> dist(0,1);
  double think_time = 9*dist(e2) + 1;
  // If it is the first time a philosopher thinks, just think
		if(act[i] != 0){
		
		sem_wait(&output);
                act[i] = 0;
                // Output format
		for(int j = 0; j!=philo_num; ++j){
			if(act[j] == 0){
		  			cout<<"  ";
			}
		else{
				cout<<"* ";
	 	}
       	}
		cout << endl;
		sem_post(&output);
		}
	sleep(think_time);
}

void eat(int i){
  random_device rd;
  mt19937 e2(rd());
  uniform_real_distribution<> dist(0,1);
  double eat_time = 2*dist(e2) + 1;
	
	sem_wait(&output);
        act[i] = 1;
		for(int j = 0; j!=philo_num; ++j){
		if(act[j] == 0){
			cout<<"  ";
		}
		else{
			cout<<"* ";
		}
	}
	cout << endl;
	sem_post(&output);
	sleep(eat_time);	
}

// Every philosopher should get the fork with smaller index first, then the fork with
// larger index, in order to avoid deadlock
void philosopher(int i){
  sem_init(&forks[i], 0, 1);
  while(true){
		think(i);
		if(i != philo_num - 1){
		   sem_wait(&forks[i]);
		   sem_wait(&forks[i+1]);
		}
		else{
		   sem_wait(&forks[0]);
		   sem_wait(&forks[i]);
		}
		eat(i);

		if(i != philo_num - 1){
		   sem_post(&forks[i+1]);
		   sem_post(&forks[i]);
		}
		else{
		   sem_post(&forks[i]);
		   sem_post(&forks[0]);
		}
                eat_times[i]++;
		// cout<<"Philo "<<i<<" is "<<eat_times[i]<<endl;
                for(int j=0;j<philo_num;j++){
                   if(eat_times[j]>=5){
                      set_ter[j]=true; 
                   }  
                }   
		// cout<<"set_ter is "<<set_ter[i]<<endl;
               if(count(set_ter,end(set_ter),true)>= philo_num){
                 exit(0);
               }    
                
  }
}



int main(int argc, char* argv[]){
	cout << "Enter the number of philosophers: " << endl;
	cin >> philo_num;  
        // The number of philosophers is not correct   
	if (philo_num > 15){
		cout << "The max philosophers number should be 15" << endl;
		return 1;
	}
	if (philo_num <= 0){
		cout << "The number of philosophers cannot be zero or negative" << endl;
		return 1;
	}
        
        // Initial output
	cout << "Eating Activity" << endl;
	if (philo_num <= 10){                                    
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

	sem_init(&output, 0, 1);

	vector<thread> ths;
	for(int i = 0; i!=philo_num; ++i){
		ths.push_back(thread(philosopher, i));
	}
	for_each(ths.begin(), ths.end(), mem_fn(&thread::join));
	return 0;
}

