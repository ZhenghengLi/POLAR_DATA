#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "RootInc.hpp"
#include "EventIterator.hpp"

using namespace std;

int main(int argc, char** argv) {

//  TApplication* rootapp = new TApplication("POLAR", &argc, argv);

    EventIterator eventIter;
    eventIter.open("output/sci_test.root");

    int counter = 0;
    while (eventIter.trigg_next()) {
        while (eventIter.event_next()) {
            if (eventIter.event.ct_num == 6) {
                if (eventIter.event.trigger_bit[9]) {
                    counter++;
                }
            }
        }
    }
    eventIter.close();
  
   cout << "counter: " << counter << endl; 

//  rootapp->Run();
    return 0;
}
