#include <iostream>
#include <iomanip>
#include "OptionsManager.hpp"
#include "Processor.hpp"

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 1;
    }

    Processor pro(&options_mgr);
    if (!pro.open_and_check()) {
        cerr << "Error occurred when opening 1P level SCI decoded files." << endl;
        return 1;
    } else {
        cout << "All files open successfully." << endl;
    }

    SciFileW scifile_w;
    if (!scifile_w.open(options_mgr.out_file.Data())) {
        cerr << "output file open failed: " << options_mgr.out_file.Data() << endl;
        return 1;
    }

    // == start process ====
    pro.set_start();
    while (pro.next_file(scifile_w)) {
        pro.write_the_file(scifile_w);
    }
    scifile_w.gen_gps_result_str();
    // == end process ====
    
    pro.write_meta_info(scifile_w);
    scifile_w.write_before_close();
    scifile_w.close();

    scifile_w.print_gps_span();
    
    return 0;
}
