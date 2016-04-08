#include <iostream>
#include <semaphore.h>
#include <fstream>
#include <string>
#include <thread>
#include <algorithm>
#include <vector>
#include <random>
#include <set>
#include <unistd.h>
using namespace std;

int philo_num;               // Number of philosophers
int bottle_pos[15][15] ={0}; // The positions of bottles
sem_t bottles[15][15];         
int drink_times[15] = {0};   // Indicate the times the philosophers drink water
sem_t output;                // Ensure output stream not interleaved

void think(int i, vector<int>& held){
	random_device rd;                // Generate a random number to indicate the thinking time
	mt19937 e2(rd());
	uniform_real_distribution<> dist(0,1);
	double think_time = 8*dist(e2) + 2;

	// If it is the first time the philosopher begins to think, there is no other actions
	if(drink_times[i] != 0){
                // Output format
		string s = "Philosopher " + to_string(i+1) + ": putting down bottles ";
		for(int j = 0; j != held.size(); ++j){
			if(held[j]<i){
			    s+= "(" + to_string(held[j]+1) + ", " + to_string(i+1) + ")";
			}
			else{
				s+= "(" + to_string(i+1) + ", " + to_string(held[j]+1) + ")";
			}
			if(j == held.size() - 1){
				s+=".";
			}
			else{
				s+=", ";
			}
		}
		sem_wait(&output);
		cout << s << endl;
		sem_post(&output);
		held.clear();                // Not requiring any bottles while thinking
	}
	sleep(think_time);
}

// Generate a random set of bottles
vector<int> thirsty(int i, int total_bottle){
	random_device rd;
	mt19937 e2(rd());
	uniform_real_distribution<> dist(0,1);
	int bottle_num = round(dist(e2)*(total_bottle - 1)) + 1;      // The number of bottles
	set<int> index;                                               // The index of bottles
	for(int i = 0; i!=bottle_num; ++i){
		while(index.insert(round(dist(e2)*(bottle_num - 1))).second == false);                    // Loop until a new bottle is selected
	}
	vector<int> held;
	int b_pos = 0;
	for(int count = 0; count != bottle_num; count++){
		while(bottle_pos[i][b_pos] == 0)
			b_pos++;
		if(index.find(count) != index.end()){
			held.push_back(b_pos);
		}
		b_pos++;
	}
	return held;
}

void eat(int i, vector<int> held){
	random_device rd;
	mt19937 e2(rd());
	uniform_real_distribution<> dist(0,1);
	double eat_time = 3*dist(e2) + 2;
	string s = "Philosopher " + to_string(i+1) + ": drinking from bottles ";
		for(int j = 0; j != held.size(); ++j){
			if(held[j]<i){
			    s+= "(" + to_string(held[j]+1) + ", " + to_string(i+1) + ")";
			}
			else{
				s+= "(" + to_string(i+1) + ", " + to_string(held[j]+1) + ")";
			}
			if(j == held.size() - 1){
				s+=".";
			}
			else{
				s+=", ";
			}
		}
		sem_wait(&output);
		cout << s << endl;
		sem_post(&output);
		sleep(eat_time);
}

void philosopher(int i){

	vector<int> held;                    // The bottles the philosopher needs
	int total_bottle = 0;                // The total number of bottles the philosopher can have
	for(int j = 0; j!=philo_num; ++j){
		if(bottle_pos[i][j] == 1){
			total_bottle++;
		}
	}
	if(total_bottle == 0){               // The philosopher has no bottle to use
		return;
	}
	while(true){
		think(i, held);
		held = thirsty(i, total_bottle);
		for(int j = 0; j!=held.size(); ++j){
			if(held[j] < i){
				sem_wait(&bottles[held[j]][i]);
			}
			else{
				sem_wait(&bottles[i][held[j]]);
			}
		}
		eat(i, held);
		for(int j = held.size()-1; j>=0; --j){
			if(held[j] < i){
				sem_post(&bottles[held[j]][i]);
			}
			else{
				sem_post(&bottles[i][held[j]]);
			}
		}
		drink_times[i]++;
	}
	
	
}


int main(int argc, char* argv[]){
        // The input file indicates the bottles that philosophers share
        string file_name;
        cout << "Please enter the name of input file" << endl;
        cin >> file_name;
	ifstream ifs;
	ifs.open(file_name);
	if(ifs.is_open()){
	 string line;
	 int row = 0;
	 int column = 0;
	 while(getline(ifs, line)){
		 philo_num++;
		 for(int i = 0; i!=line.size(); ++i){
			 if(line[i] != ' '){
			    if(line[i] == '1'){
				   bottle_pos[row][column] = 1;
				}
				column++;
			 }
		 }
		 row++;
		 column = row;
	 }
	}
	else{
                // Fail to open file
		cout << "Cannot open the file" << endl;
		return 1;
	}
	
	// Make the matrix symmetric
	int row = 1;
	while(row < philo_num){
		int column = 0;
		while(column < row){
		   bottle_pos[row][column] = bottle_pos[column][row];
		   column++;
		}
		row++;
	}
	
	sem_init(&output, 0, 1);
	// Initial the semaphore where there is a bottle
        for(int i = 0; i< philo_num; ++i){
	for(int j = i+1; j< philo_num; ++j){
                if(bottle_pos[i][j] == 1)
		sem_init(&bottles[i][j], 0, 1);
	}
	}

	vector<thread> ths;
	for(int i = 0; i!=philo_num; ++i){
		ths.push_back(thread(philosopher, i));
	}
	for_each(ths.begin(), ths.end(), mem_fn(&thread::join));
	
	return 0;
}

