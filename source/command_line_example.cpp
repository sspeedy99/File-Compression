#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
    fstream file;
    string word;
    file.open(argv[1]);
    while (file >> word) 
    { 
        // displaying content 
        cout << word << endl; 
    } 
    return 0;
}