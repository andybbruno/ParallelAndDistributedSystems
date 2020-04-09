#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

#include "BLcode.cpp"

int main(int argc, char * argv[]) {

  if(argc == 1) {
    cout << "Usage is " << argv[0]
	 << " start stop msecS msecW msecD nw [trace]" << endl;
    return(-1);
  }
  
  int start = atoi(argv[1]);
  int stop  = atoi(argv[2]);
  int m1    = atoi(argv[3]);
  int m2    = atoi(argv[4]);
  int m3    = atoi(argv[5]);
  int nw    = atoi(argv[6]);
  bool debug = (argc != 7);
  
  // business logic code components
  MySource s(start,stop,m1);
  MyWorker f(m2);
  MyDrain  d(m3);

  // implementing flow control
  const int EOS = (-1);
  
  // declare a mutex to protect task queue accesses
  mutex task_mutex;
  // and one to protect result queue accesses
  mutex res_mutex;

  // declare a queue for the input tasks
  queue<int> t_queue;
  // and one for the results
  queue<int> r_queue;
  
  // kind of three concurrent activities
  // place input tasks into the input queue
  auto emit_task = [&] (MySource s) {
		     while(s.hasNext()) {
		       auto t = s.next();
		       task_mutex.lock();
		       t_queue.push(t);
		       task_mutex.unlock();
		       if(debug)
			 cout << "Emitted " << t << endl;
		     }
		     // for as many workers to terminate, push EOS
		     for(int i=0; i<nw; i++) {
		       task_mutex.lock();
		       t_queue.push(EOS);
		       task_mutex.unlock();
		     }
		     if(debug)
		       cout << "Emitted " << nw << " EOSs" <<endl;
		     return;
		   };
  
  // process results
  auto proc_res = [&] (MyDrain d, int nw) {
		    while(true) { 
		      res_mutex.lock();
		      if(!r_queue.empty()) {
			auto t = r_queue.front();
			r_queue.pop();
			res_mutex.unlock();
			if(debug)
			  cout << "Drain got " << t << endl;
			if(t == EOS && (--nw)==0)
			  break;
			d.process(t);
			if(debug)
			  cout << "Drain processed " << t << endl;
		      } else { // else re-try (after a while?)
			res_mutex.unlock();
		      }
		    }
		  };

  // processing tasks to results in parallel
  auto body = [&] (MyWorker w, int wn) {
		  while(true) {
		      task_mutex.lock();
		      if(!t_queue.empty()) {
			auto t = t_queue.front();
			t_queue.pop();
			task_mutex.unlock();
			if(debug)
			  cout << "W" << wn << " got " << t << endl;
			if(t == EOS)
			  break;
			auto r = w.compute(t);
			if(debug)
			  cout << "W" << wn << " computed "
			       << r << " out of " << t << endl;
			res_mutex.lock();
			r_queue.push(r);
			res_mutex.unlock();
		      } else {
			task_mutex.unlock();
		      }
		  }
		  if(debug)
		    cout << "W" << wn << " got EOS "<< endl;
		  res_mutex.lock();
		  r_queue.push(EOS);
		  res_mutex.unlock();
		  return;
		};

  auto t0 = chrono::system_clock::now();
  auto e_thr = new thread(emit_task,s);
  if(debug)
    cout << "Started source" << endl; 

  auto c_thr = new thread(proc_res,d,nw);
  if(debug)
    cout << "Started Drain" << endl;
  
  vector<thread*> tids(nw);
  for(int i=0; i<nw; i++) {
    if(debug)
      cout << "Going to start worker " << i << endl;
    tids[i] = new thread(body,f,i);
  }

  // now await termination
  if(debug)
    cout <<  "Awaiting Source "<< endl; 
  e_thr->join();
  for(int i=0; i<nw; i++) {
    if(debug)
      cout << "Awaiting worker " << i << endl; 
    tids[i]->join();
  }
  if(debug)
    cout << "Awaiting Drain" << endl; 
  c_thr->join();
  auto elapsed =
    chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - t0).count();

  auto tseq = (stop-start)*(m1+m2+m3);
  cout << endl
       << "Parallel execution with " << nw << " threads took " << elapsed << " msecs" << endl
       << "sequential computation would have taken " << tseq << " msecs"
       << endl
       << "speeup is " << ((float) tseq) / ((float) elapsed) << endl; 
  return(0); 
}

