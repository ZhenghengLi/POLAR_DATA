#include <iostream>
#include "Processor.hpp"
#include "SciFile1RR.hpp"
#include "SciFileL1W.hpp"
#include "OptionsManager.hpp"

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

    SciFile1RR scifile_1r;
    if (!scifile_1r.open(options_mgr.in_file.Data())) {
        cerr << "input file open failed: " << options_mgr.in_file.Data() << endl;
        return 1;
    }

    SciFileL1W scifile_l1;
    if (!scifile_l1.open(options_mgr.out_file.Data())) {
        scifile_1r.close();
        cerr << "output file open failed: " << options_mgr.out_file.Data() << endl;
        return 1;
    }

    Processor pro(&options_mgr);
    
    // == start process ====
    pro.convert_data(scifile_1r, scifile_l1);
    scifile_1r.gen_align_info_str();
    scifile_l1.gen_gps_result_str();
    // == end process ====

    scifile_l1.write_tree();
    pro.write_meta_info(scifile_1r, scifile_l1);

    scifile_l1.print_gps_span();
    scifile_1r.print_align_info();

    scifile_l1.close();
    scifile_1r.close();
    
    return 0;
}
