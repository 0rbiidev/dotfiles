#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char const *argv[])
{
  string line;
  ifstream input("input.txt");
  char start, end;
  int output = 0;

  while (getline(input, line)){
    int i = 0;
    int found;

    for (i; i < line.length(); i++){
      if (atoi(&line[i])){
        start = line[i];
        cout << "found first " << i;
        found = i;
        break;
      }
    }
    end = start;

    for (i = found; i < line.length(); i++){
      if (atoi(&line[i])){
        end = line[i];
        
        cout << "found second" << i;
        break;
      }
    }
    string s;
    s += start;
    s += end;
    cout << s << "\n";
    output += stoi(s);
  }

  cout << output;
  return 0;
}
