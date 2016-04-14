#include <string.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include "enum.h"
#include <vector>
#include "pipe.h"


using namespace std;

bool ispiped(vector<string>cur){  // determine the command line contains pipe or not
	
	int i = 0;

	while (i < cur.size()){
		if (cur[i]=="|"){ //command line contains pipe
		  cout<<"pipe"<<endl;
			return true;
				break;
		}
		i++;
	}
	return false;
}


bool in_redirection(vector<string>cur){
	int i = 0;
	while (i < cur.size()){
		if ((cur[i]=="<") && ((cur[i + 1]=="0" )|| (cur[i + 1]=="1") )){
			return true;
			break;
		}
		i++;
	}
	return false;
}

bool out_redirection(vector<string>cur){
	int i = 1;
	while (i < cur.size()){
		if ((cur[i] == ">") && ((cur[i + 1] == "0") || (cur[i + 1] == "1"))){
			return true;
			break;
		}
		i++;
	}
	return false;
}


int exec_pipe(vector<string>cur){
	int fd[4][2];
	pid_t pid[4] = {-1};
	int count = 0;
	int pipe_result;
	vector<string>pipe_number;
	vector<string>store_command;
	//vector<string>store_command2;
        vector<string>command_output;
	char* newargv[] = { (char*)cur[1].c_str(), NULL };
        char* newargv2[] = {(char*)command_output[1].c_str(),NULL};
	char* envp[] = { 0 };
        int pipe_position[4]={0};
        int position_count=1;
       
        int count2=0;
        cout<<"start"<<endl;
	for (int i = 1; i < cur.size(); i++){     //count the number of pipes in command cur;
		if (cur[i]=="|"){
			pipe_number.push_back("|");
                        pipe_position[position_count]=i;
                        position_count++;
                        
		}
	       	if ((cur[i] != "|")){//store all the command line in the vector
			store_command.push_back(cur[i]);


	       	}
	}

        for(int i=0;i<=store_command.size();i++){
			  cout<<store_command[i]<<endl;
	}

	while (1){
	        command_output.clear();
	  	pipe_result = pipe(fd[count]);
		if (pipe_result ==1){
			perror("pipe_error");// fail to set up a pipe
			exit(pipe_failure);
		}
		pid[count] = fork();//generate a child process
		if (pid[count]<0){
			perror("fork_error");
			exit(fork_failure);
		}else if (pid[count]==0){  //it is child process
			if (count == 0){   // 
				close(fd[0][FD_READ]);
			}
			else{
				dup2(fd[count - 1][FD_READ], 0);
			}

			if (pipe_number.size()<=count){       //if there doesn't exit next pipe
				close(fd[count][FD_WRITE]);
			}
			else{
				dup2(fd[count][FD_WRITE], 1); //there is next pipe, duplicate the output
			}

			//CLOSE ALL THE PIPES
			for (int i = 0; i <= count; i++){
				close(fd[i][FD_WRITE]);
				close(fd[i][FD_READ]);
			}
			//test
			for(int i=0;i<=store_command.size();i++){
			  cout<<store_command[i]<<endl;
			}
			//test end
                        
                        for(int i=pipe_position[count2];i<pipe_position[count2+1];i++){
			  command_output.push_back(store_command[i]);
			}
                        count2=count2+2;
                        execve(command_output[0].c_str(),newargv2,envp);
			//	execve(].c_str(),newargv,envp);
		       	//	execve(store_command.c_str(),newargv.envp);
                        cout << "Path does not exist" << endl;
                        return -1;
			
		}
		else{                 
			if (pipe_number.size()>count){//there exits next pipe
			     //fetch the next command;
				count++;
				continue;
			}

			for (int i = 0; i <= count; i++){
				close(fd[i][FD_WRITE]);
				close(fd[i][FD_READ]);
			}
			for (int i = 0; i <= count; i++){ // wait for all the child processes to return
				waitpid(pid[i], NULL, 0);
			}
                        return-1;
			 
		}
	}
}

