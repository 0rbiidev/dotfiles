#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char const *argv[])
{
  string line;
  ifstream input("input.txt");
  char start, end;
  int output = 0;
  const string words []= {
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
  };

  while (getline(input, line)){
    int i = 0;
    string word;

    for (i; i < line.length(); i++){
      if (atoi(&line[i])){
        start = line[i];
        word = "";
        break;
      }
      else {
        word += line[i];
        for (int i = 0; i < 9; i++){
          if (word == words[i]){
            start = i+1;
            word = "";
            break;
          }
        }
      }
    }
    end = start;

    for (i++; i < line.length(); i++){
      if (atoi(&line[i])){
        end = line[i];
        word == "";
      }
      else {
        word += line[i];
        for (int i = 0; i < 9; i++){
          if (word == words[i]){
            cout << word;
            end = i+1;
          }
        }
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
