#include <iostream>
#include "Processor.hpp"
#include "HkFileL1Conv.hpp"
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


    HkFileL1Conv hkfile_l1_conv;
    if (!hkfile_l1_conv.open_r(options_mgr.in_file.Data())) {
        cerr << "input file open failed: " << options_mgr.in_file.Data() << endl;
        return 1;
    }

    if (!hkfile_l1_conv.open_w(options_mgr.out_file.Data())) {
        hkfile_l1_conv.close_r();
        cerr << "output file open failed: " << options_mgr.out_file.Data() << endl;
        return 1;
    }

    Processor pro(&options_mgr);
    
    // == start process ====
    pro.convert_data(hkfile_l1_conv);
    hkfile_l1_conv.pol_hk_gen_extra_info_str();
    hkfile_l1_conv.pol_hk_gen_gps_span_str();
    // == end process ====

    hkfile_l1_conv.pol_hk_write_tree();
    pro.write_meta_info(hkfile_l1_conv);

    hkfile_l1_conv.pol_hk_print_gps_span();
    hkfile_l1_conv.pol_hk_print_extra_info();

    hkfile_l1_conv.close_w();
    hkfile_l1_conv.close_r();
   
    return 0;
}
