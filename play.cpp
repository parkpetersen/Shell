#include <algorithm>
#include <iostream>
#include <vector>
 
int main()
{
    std::string v = "ls > txt.txt";
    std::vector<int> t{'|', '<', '>'};
 
    auto result = std::find_first_of(v.begin(), v.end(), t.begin(), t.end());
 	std::cout<<*v.end()<<std::endl;
    if (result == v.end()) {
        std::cout << "no elements of v were equal to 3, 19, 10 or 2\n";
    } else {
        std::cout << "found a match at "
                  << std::distance(v.begin(), result) << "\n";
    }

 }
