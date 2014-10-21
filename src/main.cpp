#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include "grammar.h"
#include "cykparser.h"
#include "threading.h"

using namespace std;



void * parseFiles(void * t){
  cout << "Hello !!";
}

int main (int argc, char* argv[]){

  if (argc > 1){
    FILE *grammar_file, *data_file;
    grammar_file = fopen(argv[1], "r");
    if (grammar_file == NULL){
      cout << "Cannot open grammar file !" << endl;
      exit(-1);
    }
    data_file = fopen(argv[2], "r");
    if (data_file == NULL){
      cout << "Cannot open data file !" << endl;
      exit(-1);
    }

    srand(time(NULL));
    Grammar g(grammar_file);
    Grammar cnfGrammar = g.toCNF();
    CYKParser parser(cnfGrammar);
    char * input = "walking";
    make_n_threads(parseFiles, 20, data_file, &parser);
    // vector<char*> tokenized = stringToVec(input, '+');
    // printRule(tokenized);
    // cout << endl;
    // vector <ParseTreeNode> parses = parser.parse(tokenized);
    // cout << "I got " << parses.size() << " Parses" <<endl;
    // for ( int i=0; i< parses.size(); i++){
    //   cout << parses[i].flatRepr() << endl;
    // }

  }
}
