#include <systemc.h>
#include <unistd.h>

/*MSG*/
#include "SSpp-mips/simplescalar.hpp"
#include "bus/my_bus.hpp"
#include "bus/simple_bus_arbiter.hpp"
#include "timer.hpp"
#include "sharedmemory.hpp"

/*MSO (06/06/2008) Variable declared to obtain the environ directly
  In SystemC programs we dont have the main, but sc_main*/
extern char **environ;


int sc_main(int argc, char *argv[]){

  sharedmemory 	  *sharedmem  = new sharedmemory("mem",/*base*/ 0x80000000,/*final*/ 0x8fffffff,/*latency*/ 25);
  simple_bus_arbiter *arbiter = new simple_bus_arbiter("arbiter");
  my_bus 			 *bus 	  = new my_bus("bus", /*mode verbose*/ false);
//  timer 	*int_generator    = new timer("int_generator", /*number of interrupt*/ 1);

  //-----------------------------------------------------------------------------------
  const int argc1 = 14;
  char *argv1[argc1] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5], argv[6],
		  argv[7],argv[8],argv[9],argv[10],argv[11],argv[12], argv[20]};

  const int argc2 = 13;
  char *argv2[argc2] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5],argv[6],
		  argv[13],argv[8],argv[9],argv[10],argv[11],argv[12]};

  const int argc3 = 13;
  char *argv3[argc3] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5],argv[6],
		  argv[14],argv[8],argv[9],argv[10],argv[11],argv[12]};

  const int argc4 = 13;
  char *argv4[argc4] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5],argv[6],
		  argv[15],argv[8],argv[9],argv[10],argv[11],argv[12]};

  const int argc5 = 13;
  char *argv5[argc5] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5], argv[6],
		  argv[16],argv[8],argv[9],argv[10],argv[11],argv[12]};

  const int argc6 = 13;
  char *argv6[argc6] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5],argv[6],
		  argv[17],argv[8],argv[9],argv[10],argv[11],argv[12]};

  const int argc7 = 13;
  char *argv7[argc7] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5],argv[6],
		  argv[18],argv[8],argv[9],argv[10],argv[11],argv[12]};

  const int argc8 = 13;
  char *argv8[argc8] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5],argv[6],
		  argv[19],argv[8],argv[9],argv[10],argv[11],argv[12]};


  //-----------------------------------------------------------------------------------

 FILE *fd= fopen(argv[21], "w+" );
 sc_signal<bool> reset;
 sc_clock clock("CLOCK", 10, 0.5, 0.0);

 bus->clock(clock);
 sharedmem->clock(clock);

 bus->slave_port(*sharedmem);
 bus->arbiter_port(*arbiter);

 simplescalar scsp("PROC1", 1);
 scsp.setFd(fd);  // setting a file to dump the stats
 scsp.CLK(clock);
 scsp.bus_port(*bus);
 scsp.init(argc1, argv1, environ);
 scsp.reset(reset); // Ok, now this port is not used, processor starts immediatly

 simplescalar scsp2("PROC2", 2);
 scsp2.setFd(fd);  // setting a file to dump the stats
 scsp2.CLK(clock);
 scsp2.bus_port(*bus);
 scsp2.init(argc2, argv2, environ);
 scsp2.reset(reset); // Ok, now this port is not used, processor starts immediatly

 simplescalar scsp3("PROC3", 3);
  scsp3.setFd(fd);  // setting a file to dump the stats
  scsp3.CLK(clock);
  scsp3.bus_port(*bus);
  scsp3.init(argc3, argv3, environ);
  scsp3.reset(reset); // Ok, now this port is not used, processor starts immediatly

  simplescalar scsp4("PROC4", 4);
   scsp4.setFd(fd);  // setting a file to dump the stats
   scsp4.CLK(clock);
   scsp4.bus_port(*bus);
   scsp4.init(argc4, argv4, environ);
   scsp4.reset(reset); // Ok, now this port is not used, processor starts immediatly

   simplescalar scsp5("PROC5", 5);
   scsp5.setFd(fd);  // setting a file to dump the stats
   scsp5.CLK(clock);
   scsp5.bus_port(*bus);
   scsp5.init(argc5, argv5, environ);
   scsp5.reset(reset); // Ok, now this port is not used, processor starts immediatly

   simplescalar scsp6("PROC6", 6);
   scsp6.setFd(fd);  // setting a file to dump the stats
   scsp6.CLK(clock);
   scsp6.bus_port(*bus);
   scsp6.init(argc6, argv6, environ);
   scsp6.reset(reset); // Ok, now this port is not used, processor starts immediatly

   simplescalar scsp7("PROC7", 7);
    scsp7.setFd(fd);  // setting a file to dump the stats
    scsp7.CLK(clock);
    scsp7.bus_port(*bus);
    scsp7.init(argc7, argv7, environ);
    scsp7.reset(reset); // Ok, now this port is not used, processor starts immediatly

    simplescalar scsp8("PROC8", 8);
     scsp8.setFd(fd);  // setting a file to dump the stats
     scsp8.CLK(clock);
     scsp8.bus_port(*bus);
     scsp8.init(argc8, argv8, environ);
     scsp8.reset(reset); // Ok, now this port is not used, processor starts immediatly

 cout << "Starting";
 sc_start(clock,3e8);
cout << "Finished SystemC simulation" << endl;
 sc_stop();

// mshared->print_mem(0x80090000, 0x8fffffff);

 return 0;
}
