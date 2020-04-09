#include <iostream>
#include <omp.h>
#include <vector>

using namespace std;

int main(int argc, char * argv[]) {

  if(argc != 4) {
    std::cout << "Usage is: " << argv[0] << " num_steps nw cachepad"
	      << std::endl;
    return(-1);
  }
  long num_steps = atoi(argv[1]);
  int nw = atoi(argv[2]);
  int pad = atoi(argv[3]);
  
  double pi = 0.0;
  vector<double> sum(nw*pad,0.0);

  auto start = omp_get_wtime();
#pragma omp parallel num_threads(nw)
  {
    // long numiter = 0L;
    double step = 1.0/(double) num_steps;
    auto me = omp_get_thread_num(); 
    for (int i = me; i < num_steps; i += nw) {
      double x = (i+0.5)*step;
      sum[me*pad] = sum[me*pad] + 4.0/(1.0+x*x);
      // numiter++;
    }
    // #pragma omp critical
    //std::cout << omp_get_thread_num() << " " << numiter << std::endl;
    
  }

  double step = 1.0/(double) num_steps;
  for(int i = 0; i<nw; i++)
    pi += step * sum[i*pad];
  
  auto elapsed = omp_get_wtime() - start;
  
  std::cout << "Pi = " << pi << " (Computed in "
	    << elapsed << " secs)"  << std::endl; 
  return(0);
}