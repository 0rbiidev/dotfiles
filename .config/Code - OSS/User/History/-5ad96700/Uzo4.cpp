#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char const *argv[])
{
  string line;
  ifstream input("input.txt");
  char start, end;

  while (getline(input, line)){
    int i;

    for (i = 0; i < line.length(); i++){
      if (atoi(&line[i])){
        start = line[i];
        break;
      }
    }

    end = start;

    for (i; i < line.length(); i++){
      if (atoi(&line[i])){
        end = line[i];
        break;
      }
    }

    
  }

  return 0;
}