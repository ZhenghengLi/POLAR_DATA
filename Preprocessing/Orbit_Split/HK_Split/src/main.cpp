#include <iostream>
#include <iomanip>
#include "OptionsManager.hpp"
#include "Processor.hpp"
#include "HkFileW.hpp"

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
        cerr << "Error occurred when opening 1P level HK decoded files." << endl;
        return 1;
    } else {
        cout << "All files open successfully." << endl;
    }

    HkFileW hkfile_w;
    if (!hkfile_w.open(options_mgr.out_file.Data())) {
        cerr << "output file open failed: " << options_mgr.out_file.Data() << endl;
        return 1;
    }

    // == start process ====
    pro.set_start();
    while (pro.next_file(hkfile_w)) {
        pro.write_the_file(hkfile_w);
    }
    hkfile_w.gen_gps_result_str();
    // == end process ====
    
    hkfile_w.write_before_close();
    pro.write_meta_info(hkfile_w);
    hkfile_w.close();

    hkfile_w.print_gps_span();
    
    return 0;
}
