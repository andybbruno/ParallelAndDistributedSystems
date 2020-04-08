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
 * Date:   March 2019
 */

#include <iostream>
#include <ff/ff.hpp>
using namespace ff;

struct myNode : ff_node_t<int>
{
    int svc_init()
    {
        std::cout << "Hello. I'm going to start\n";
        counter = 0;
        return 0;
    }
    int *svc(int *)
    {
        if (++counter > 5)
            return EOS;
        std::cout << "Hi! (" << counter << ")\n";
        return GO_ON; // keep calling the svc method
    }
    void svc_end()
    {
        std::cout << "Goodbye!\n";
    }
    // this is needed to start the node,
    // run() and wait() are protected methods of the ff_node_t class
    int run_and_wait_end(bool = false)
    {
        if (run() < 0)
            return -1;
        return wait();
    }
    long counter;
};
int main()
{
    myNode mynode;
    return mynode.run_and_wait_end();
}
