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
        //std::cout<<"input: "<<inputFileString<<std::endl;
        //std::cout<<"output: "<<outputFileString<<std::endl;


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



	int p[2];       
	pipe(p);          
	auto pid = fork();
	if (pid == 0)
	{
		dup2(p[1], STDOUT_FILENO);   //write end to become STDOUT  (what would go to stdout goes to the pipe instead)
		execlp(, "ls");
	}
	else
	{
		close(p[1]);
		int status = 0;
		waitpid(pid, &status, 0);
		dup2(p[0], STDIN_FILENO);                   //read end to becom stdin
		execlp("cat", "cat");
	}

}
