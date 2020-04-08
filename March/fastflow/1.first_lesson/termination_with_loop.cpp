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

#include <ff/ff.hpp>
#include <ff/pipeline.hpp>
using namespace ff;

struct Stage_0: ff_node_t<long>{
    long* svc(long*) {
	for(long i=1;i<=100;++i)
	    ff_send_out((long*)i);
	//sleep(100);	
	return EOS;
    }
};
struct Stage_A: ff_minode_t<long> {   // multi-input node
    long* svc(long* in) {
	if (fromInput()) {
	    ff_send_out(in);
	    ++onthefly;
	} else
	    if ((--onthefly<=0) && eosreceived) return EOS;
	return GO_ON;
    }

    void eosnotify(ssize_t) {
	eosreceived=true;	
	if (onthefly==0) ff_send_out(EOS);	
    }

    bool eosreceived=false;
    size_t onthefly=0;
};
struct Stage_B: ff_monode_t<long> {
    long* svc(long* in) {
	usleep(100*(long)in);
	ff_send_out_to(in, 0); // backward channel
	ff_send_out_to(in, 1); // forward channel
	return GO_ON;
    }
};
struct Stage_1: ff_node_t<long>{
    long* svc(long* in) {
	printf("S_1 received %ld\n", (long)in);
	return GO_ON;
    }
};

int main() {
    Stage_A S_A;
    Stage_B S_B;
    ff_Pipe pipeIn(S_A, S_B);
    pipeIn.wrap_around();    // feedback channel between S_B and S_A

    Stage_0 S_0;
    Stage_1 S_1;
    ff_Pipe pipeOut(S_0, pipeIn, S_1);

    if (pipeOut.run_and_wait_end()<0)
	error("running pipeOut\n");

    return 0;
}


	
