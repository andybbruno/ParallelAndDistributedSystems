#include <iostream>
#include <functional>
#include <atomic>
//
// tasks to be executed in the program have a code (function to execute)
// an input parameter (to await) and a result (to deliver to the "next" task)
//

// we assume stream parallel patterns are defined by the grammar
// Sk ::= Seq(some business logic code) | Pipe (Sk, Sk, ...) |
//      | Comp(Sk, Sk, ...) | Farm(Sk, N)
//


// any stream parallel tree may be turned into a string of tasks
// hosting the sequential code in the leaves of the original pattern three
// grouped into a single task IFF they have a common Comp ancestor
// or used one by one
//
// Sample tree
//
//    Pipe(Farm(Pipe(S1, S2),n1), Farm(Comp(S3, S4)), S5)
//
//
//                    Pipe
//                  /  |   \
//                 /   |    \
//               Farm Farm  S5
//               /     |
//            Pipe    Comp
//           /   \   /   \
//         S1    S2 S3   S4
//
// Therefore we have
//      Task 1: computing S1
//      Task 2: computing S2
//      Task 34: computing (S4(S3(x)) (because of the common Comp Ancestor
//      Task 5: computing S5
//
//  Task 1 delivers result to Task 2 that in turn delivers to Task 34 etc.
//  Which corresponds to applying to the original program
//
//  1) Farm elimination (taking away all the farms)
//  2) Pipe Elimination (turning all pipeline into comps)
//  3) Summarizing all the comps into a single comp iff they where not
//     part of the original pattern three
//
//  1)             Pipe
//               /   |   \
//          Pipe   Comp  S5
//          / \    /  \
//         S1 S2  S3  S4
//
//  2)             Comp
//               /   |   \
//          Comp   COMP  S5
//          / \    /  \
//         S1 S2  S3  S4
//
// 3)           Comp
//          /  /  |    \
//         S1 S2 COMP  S5
//               /  \
//               S3  S4
// 

//
// NOW THE CODE
// 
// definition of a task

template<typename Tin, typename Tout>  // actually, to store tasks in the queue, they all need to be of the same typein/out
struct __task {
  std::function<Tout(Tin)> f;     // the business logic code to compute
  Tin x;                          // the input token
  int Sno;                        // i for Si, to identify the task and to be able to compute the next one
};


#include "BLcode.cpp"

__task<int,int> * EOS = NULL;  // idem

#include "queue.cpp"
syque<__task<int,int>*> repo;  // task repository, only hosting fireable MDFi
                               // in this simple case (to be fixed)

const bool debug = false;      // just to control print trace instructions

int main(int argc, char * argv[]) {

  if(argc == 1) {
    std::cout << "Usage is " << argv[0]
	      << " start stop msecS msecW msecD nw [trace]" << std::endl;
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
  // let's suppose all the Si in the tree compute this same inc code
  MyWorker f1(m2); // S1
  MyWorker f2(m2); // S2
  MyWorker f3(m2); // S3 
  MyWorker f4(m2); // S4
  MyWorker f5(m2); // S5
  // then this is the drain code
  MyDrain  d(m3);

  int m = stop-start;       // the total amount of tasks to compute
  std::atomic<int> count;   // this is for termination
  count = 4*m;              // sample tree : S1, S2, (S4(S3())), S5 -> 4 instructions per task
  
  
  // thread generating the stream:
  // body is:
  auto emitter = [&] (int m, int nw) {
		   while(s.hasNext()) {
		     auto i = s.next();
		     // emitter always creates S1 tasks
		     auto t = new __task<int,int>;
		     t->x = i;                          // emit i
		     t->Sno = 1;                        // business logic code is S1 
		     t->f = [&](int x) { return(f1.compute(x)); };  // that is (f1.compute())
		     if(debug)
		       std::cout << "E emitting "
				 << t->Sno << "(" << t->x << ") "
				 << std::endl;
		     repo.push(t);                      // this is a fireable task
		   }
		 };

  // executor thread: body is: 
  auto computer = [&] (int wn) {
		    int taskno = 0;
		    if(debug)
		      std::cout << "W" << wn << " started: " << std::endl; 
		    while(true) {
		      // get fireable task from repo 
		      if(count.fetch_sub(1) > 0) {
			auto t = repo.pop();
			taskno++;
			if(debug)
			  std::cout << "W" << wn << " computing task "
				    << t->Sno << "(" << t->x << ")"
				    << std::endl;
			
			// check if it is a final one
			// this is business logic specific
			if(t->Sno == 5) { // in case compute then Drain
			  auto res = (t->f)(t->x); // compute the result
			  if(debug)
			    std::cout << "W" << wn << " computed " << res
				      <<std::endl;
			  d.process(res);          // process drain
			  // and move to compute the next task, if any (i.e. next iteration please)
			  continue;  // not necessary, but it helps
			} else {              // compute and push continuation
			  auto x1 = (t->f)(t->x); // compute local f
			  auto t1 = new __task<int,int>;  //"W" << wn << " comput alloc continuation
			  t1->x = x1;                 // compute on partial res
			  // THIS IS BUSINESS LOGIC DEPENDENT
			  switch(t->Sno) {
			  case 1: {
			    t1->Sno = 2;
			    t1->f = [&](int x) { return f2.compute(x); };
			    break;
			  }
			  case 2: {
			    t1->Sno = 34;
			    t1->f = [&](int x) { return f4.compute(f3.compute(x)); };
			    break;
			  }
			  case 34: {
			    t1->Sno = 5;
			    t1->f = [&](int x) { return f5.compute(x); };
			    break;
			  }
			  default: {
			    std::cerr << "Error: trying to evaluate a non existing function : " << t->Sno << std::endl;
			    break;
			  }
			  }
			  if(debug)
			    std::cout << "W" << wn << " pushing cont "
				      << t1->Sno << std::endl;
			  repo.push(t1);              // store fireable to repo
			}
		      } else {
			// terminate
			break;
		      }
		    }
		    if(debug)
		      std::cout << "W" << wn << " computed " << taskno << " tasks " << std::endl;
		  };
  
  auto tstart = std::chrono::high_resolution_clock::now();
  std::thread * thr_e = new std::thread(emitter, m, nw);
  std::vector<std::thread*> thr_w(nw);
  for(int i=0; i<nw; i++)
    thr_w[i] = new std::thread(computer, i);

  // await threads
  thr_e->join();
  for(int i=0; i<nw; i++)
    thr_w[i]->join();
  auto elapsed = std::chrono::high_resolution_clock::now() - tstart;
  auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
  std::cout << "Spent " << msec << " msecs with " << nw << " threads " << std::endl;
  
  auto tseq = m * (m1 +5*m2 + m3);
  std::cout << "Speedup is " << ((float) tseq)/((float) msec) << " (should be " << nw << ")" << std::endl; 
  return(0); 
}
