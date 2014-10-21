#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include "grammar.h"
#include "cykparser.h"
using namespace std;


class ThreadData{
public:
  int thread_id;
  FILE *fp;
  ThreadData ();
  ThreadData (int thread_id,  FILE *fp);
};

ThreadData::ThreadData (){
}
ThreadData::ThreadData (int thread_id,FILE *fp){
  this -> thread_id = thread_id;
  this -> fp = fp;
}

void *PrintRandom(void * threaddata){
  ThreadData *tdata = (ThreadData *) threaddata;
  int threadid = tdata -> thread_id;
  FILE *fp = tdata -> fp;
  size_t len = 0;
  ssize_t read;
  char *line = NULL; 
  while ((read = getline(&line, &len, fp)) != -1){
    cout << threadid << ":" << line;
  }
  pthread_exit(NULL);
}

void make_n_threads(int n, FILE *fp){
  pthread_t threads[n];
  int rc,i;
  for (i =0; i < n; i++){
    ThreadData th(i,fp);
    rc = pthread_create(&threads[i], NULL, PrintRandom,(void *) &th);
    if (rc) {
      cout << "Unable to make the thread" << endl;
    }
  }
  pthread_exit(NULL);
  
}

int main (int argc, char* argv[]){

  if (argc > 1){
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL){
      cout << "Cannot open input file !" << endl;
      exit(-1);
    }
    srand(time(NULL));
    Grammar g(fp);
    //    cout << g << endl;
    Grammar cnfGrammar = g.toCNF();
    //    cout << "CNF" << endl;
    // cout << cnfGrammar;
    // cout << endl;
    CYKParser parser(cnfGrammar);
    char * input = "walking";
    vector<char*> tokenized = stringToVec(input, '+');
    printRule(tokenized);
    cout << endl;
    vector <ParseTreeNode> parses = parser.parse(tokenized);
    cout << "I got " << parses.size() << " Parses" <<endl;
    for ( int i=0; i< parses.size(); i++){
      cout << parses[i].flatRepr() << endl;
    }

  }
}
