#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <fcntl.h>

std::string inputFileString;
std::string outputFileString;
const char* inputFile;
const char* outputFile;
bool inputMode = false;
bool outputMode = false;

std::vector<std::string> findCommands(std::string line)
{
	std::vector<char> t{'|','<','>'};
	std::vector<std::string> cmdVect;
	auto current = line.begin();
	while (current != line.end())
	{
		auto result = std::find_first_of(current, line.end(), t.begin(), t.end());
		auto next = line.begin() + std::distance(line.begin(), result);	
		if (*next == '<')
		{
			cmdVect.emplace_back(current, next-1);
			auto distance = abs(std::distance(next+2, std::find_first_of(next+2, line.end(), t.begin(), t.end())));
			std::copy_n(next+2, distance, std::back_inserter(inputFileString));
			current = next + 2 + distance;
			inputMode = true;
		}
		else if (*next == '>')
		{
			cmdVect.emplace_back(current, next-1);
			auto distance = abs(std::distance(next+2, std::find_first_of(next+2, line.end(), t.begin(), t.end())));
			std::copy_n(next+2, distance, std::back_inserter(outputFileString));
			current = next + 2 + distance;
			outputMode = true;
		}
		else
		{	
			cmdVect.emplace_back(current, next);
			current = next;

		}
		if(current != line.end())
			++current;
	}
	
	return cmdVect;
} 


void parse(std::string line, std::vector<std::string> *wordVect)
{
        auto current = line.begin();                       //based off of in class code
        while (current != line.end())
        {
                auto next = std::find(current, line.end(), ' ');
                wordVect->emplace_back(current, next);
                current = next;
                if(current != line.end())
                        ++current;
        }

}


int main()
{
	std::cout<<"[CMD]:";
	std::string line;
	std::getline(std::cin,line);
	std::vector<std::string> lines = findCommands(line);
	std::cout<<"input file: "<<inputFileString<<std::endl;
	std::cout<<"output file: "<<outputFileString<<std::endl;

	
	std::vector<std::vector<std::string>> commands;
	for (auto &i : lines)
	{
		if (i[0] == ' ')
			i.erase(0, 1);
		std::cout<<i<<std::endl;
	}
		

	for(size_t i = 0; i < lines.size(); i++)
	{
		std::vector<std::string> *words = new std::vector<std::string>;
		parse(lines[i], words);
		commands.push_back(*words);
		
	}
	
//	for (auto i : commands)
//	{
//		for (auto j : i)
//		{
//			std::cout<<j<<std::endl;
//		}
//	}

	size_t count = 0;
	int p[2];       
	pipe(p);
	int fd;


  	while(count < commands.size())
	{
	if (commands[0][0] == "exit")
		exit(0);
	auto pid = fork();
	if (pid == 0)
	{
		if (inputMode && count == 0)
		{
			fd = open(inputFileString.c_str(), O_RDONLY);
			dup2(p[0], fd);
		}
		dup2(p[1], STDOUT_FILENO);   //write end to become STDOUT  (what would go to stdout goes to the pipe instead)

		std::vector<char *> args;
                for(size_t i = 0; i < commands[count].size(); i++)
                {
                	char *c = new char[commands[count][i].length()];
                        commands[count][i].copy(c, commands[count][i].length());
                        args.push_back(c);
                }
		++count;
                args.push_back(NULL);
                if(execvp(args[0],args.data()) < 0)
                {
                	perror("execvp");
                        exit(EXIT_FAILURE);
                }
		//++count;
	}

//	if (pid == 0)
//	{
//		close(p[1]);
//		close(STDIN_FILENO);
//		dup2(p[0], STDIN_FILENO);
//		 std::vector<char *> args;
  //              for(size_t i = 0; i < commands[count].size(); i++)
//                {
//                        char *c = new char[commands[count][i].length()];
//                        commands[count][i].copy(c, commands[count][i].length());
//                        args.push_back(c);
//                }
//
 //              args.push_back(NULL);
//                if(execvp(args[0],args.data()) < 0)
//                {
//                        perror("execvp");
//                        exit(EXIT_FAILURE);
//                }
//		count++;


//	}
	else
	{
		close(p[1]);
		close(p[0]);
		int status = 0;
		waitpid(pid, &status, 0);

		if (count == commands.size())
		{
			fd = open(outputFileString.c_str(), O_RDWR | O_CREAT);
			dup2(p[1], fd);
		}
		dup2(p[0], STDIN_FILENO);                   //read end to becom stdin

		std::vector<char *> args;
               for(size_t i = 0; i < commands[count].size(); i++)
                {
                        char *c = new char[commands[count][i].length()];
                        commands[count][i].copy(c, commands[count][i].length());
                        args.push_back(c);
                }

                args.push_back(NULL);
                if(execvp(args[0],args.data()) < 0)
                {
                        perror("execvp");
                        exit(EXIT_FAILURE);
                }
                ++count;

	}
	}
	close(p[0]);
	close(p[1]);
//	exit(0);
}

