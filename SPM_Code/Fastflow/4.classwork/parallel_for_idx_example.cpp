/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ***************************************************************************
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this
 *  file does not by itself cause the resulting executable to be covered by
 *  the GNU General Public License.  This exception does not however
 *  invalidate any other reasons why the executable file might be covered by
 *  the GNU General Public License.
 *
 ****************************************************************************
 */
/* 
 * Author: Massimo Torquati <torquati@di.unipi.it> 
 * Date:   2020
 *
 */
/* 
 *  A very basic example showing how to use the parallel_for_idx method
 *  of the class ParallelFor
 *
 */
#include <iostream>
#include <mutex>

#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
using namespace ff;

std::mutex mtx;

int main(int argc, char *argv[]) {    
    if (argc<3) {
        std::cerr << "use: " << argv[0]  << " nworkers size\n";
        return -1;
    }
    const size_t nworkers = std::stol(argv[1]);
    const size_t size     =  std::stol(argv[2]);

    std::vector<long> V(size);

    // for(size_t i=0;i<size; ++i) V[i] = i;
    
    ParallelFor pf(nworkers);
    pf.parallel_for_idx(0, size,    // start, stop indexes
                        1,          // step size
                        0,          // chunk size (0=static, >0=dynamic)
                        [&](const long begin, const long end, const long thid)  {
                            { // this is just to have a consistent printing
                              // on the screen
                                std::lock_guard<std::mutex> lck{mtx};
                                std::cout << "I'm Worker " << thid
                                          << " range=["    << begin
                                          << "," << end << ")\n";
                            }

                            // this is the body of the loop
                            for(long i=begin; i<end; ++i)
                                V[i] = i;
                        });
    
    return 0;
}
