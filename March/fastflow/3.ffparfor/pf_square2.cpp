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
 * Date:   April 2019
 *
 */

#include <iostream>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
using namespace ff;

int main(int argc, char *argv[]) {    
    if (argc<3) {
        std::cerr << "use: " << argv[0]  << " nworkers stream-length\n";
        return -1;
    }
    const size_t nworkers = std::stol(argv[1]);
    const size_t slength =  std::stol(argv[2]);

    ParallelForReduce<float>* pf;
    if (nworkers>1) {
        pf = new ParallelForReduce<float>(nworkers);
    }
    
    float sum=0.0;
    ffTime(START_TIME);
    if (nworkers>1) {
        pf->parallel_reduce(sum, 0.0,
                           0,slength, 
                           [&](const long i, float& sum)     { sum+=i*i; },
                           [](float& sum, const float& elem) { sum+=elem; });
    } else {
        for(size_t i=0;i<slength;++i) sum+=i*i;
    }
    ffTime(STOP_TIME);
    std::cout << "sum = " << sum << "\n";    
    std::cout << "Time: " << ffTime(GET_TIME) << "\n";
    return 0;
}
