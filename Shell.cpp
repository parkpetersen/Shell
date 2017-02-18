#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <chrono>


std::chrono::steady_clock::time_point start;
std::chrono::steady_clock::time_point  end;
std::chrono::duration<double> timeElapsed;
std::vector<std::string> lineVect;


void parse(std::string line, std::vector<std::string> &wordVect)
{
	auto current = line.begin();                       //based off of in class code
        while (current != line.end())
        {
                auto next = std::find(current, line.end(), ' ');
                wordVect.emplace_back(current, next);
                current = next;
                if(current != line.end())
                        ++current;
        }

}

void execute(std::vector<std::string> words)
{

                        if (words[0] == "exit")
                        {
                                std::vector<char *> args;
                                std::string exitString = "exit";
                                char *p = new char[exitString.length()];
                                exitString.copy(p, exitString.length());
                                args.push_back(p);
                                args.push_back(NULL);
                                execvp(args[0],args.data());
                        }

                        else if (words[0] == "ptime")
                        {                                                                                                                                                                               std::cout<<"Time spent executing last command: "<<std::chrono::duration_cast<std::chrono::milliseconds>(timeElapsed).count()<< " milliseconds"<<std::endl;
                                exit(0);
                        }

			else if (words[0] == "history")
                        {
                                std::cout<<"Command History: "<<std::endl;
                                std::for_each(lineVect.begin(), lineVect.end(), [](std::string l){std::cout<<l<<std::endl;});
                                exit(0);
                        }

                        else if (words[0] == "^")
			{	
				int num = stoi(words[1]);	
				words.clear();
				std::string line = lineVect[num-1];
				parse(line,words);
				std::cout<<"Executing "<<words[0]<<std::endl;
				execute(words);
			}


                        else
                        {
                                std::vector<char *> args;
                                for(size_t i = 0; i < words.size(); i++)
                                {
                                        char *p = new char[words[i].length()];
                                        words[i].copy(p, words[i].length());
                                        args.push_back(p);
                                }

                                args.push_back(NULL);
                                if(execvp(args[0],args.data()) < 0)
                                {
                                        perror("execvp");
                                        exit(EXIT_FAILURE);
                                }
                        } 

}


int main()
{

	std::string line;
	while (line != "exit")
	{
		std::getline(std::cin,line);
		std::vector<std::string> words;
		if (line != "history")
			lineVect.push_back(line);

		parse(line, words);
		
		start = std::chrono::steady_clock::now();
		auto PID = fork();

		if (PID < 0)
		{
			perror("Error");
			exit(0);
		}
		else if (PID == 0)
		{
			execute(words);

		}
	
		else
		{
			int status;
			waitpid(PID, &status, 0);
			end = std::chrono::steady_clock::now();
			timeElapsed = (end - start);	
				
		}	


	}


	return 0;
}
