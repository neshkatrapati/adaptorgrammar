#include <iostream>
#include <cstdlib> 
class ThreadData{
public:
  int thread_id;
  FILE *fp;
  CYKParser *p;
  ThreadData ();
  ThreadData (int thread_id,  FILE *fp, CYKParser *p);
};

ThreadData::ThreadData (){
}
ThreadData::ThreadData (int thread_id,FILE *fp, CYKParser *p){
  this -> thread_id = thread_id;
  this -> fp = fp;
  this -> p = p;
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

void make_n_threads(void* (*threadMethod) (void *t), int n, FILE *fp, CYKParser *p){
  pthread_t threads[n];
  int rc,i;
  for (i =0; i < n; i++){
    ThreadData th(i,fp,p);
    rc = pthread_create(&threads[i], NULL, threadMethod,(void *) &th);
    if (rc) {
      cout << "Unable to make the thread" << endl;
    }
  }
  pthread_exit(NULL);
  
}
