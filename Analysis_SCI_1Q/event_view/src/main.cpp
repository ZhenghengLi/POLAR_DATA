#include <iostream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "EventCanvas.hpp"

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 2;
    }

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
    EventCanvas event_canvas;

    if (event_canvas.open(options_mgr.pol_event_filename.Data(), options_mgr.start, options_mgr.step)) {
        event_canvas.draw_event();
        rootapp->Run();
        return 0;
    } else {
        return 1;
    }

}
