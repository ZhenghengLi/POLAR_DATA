#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <stdint.h>
#include "GPSTime.hpp"
#include "HkGPSIterator.hpp"
#include "SciTransfer.hpp"
#include "Processor.hpp"
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

    SciTransfer scitran;
    scitran.set_jumpsec(options_mgr.jumpsec);
    if (!scitran.open_read(options_mgr.scifile.Data())) {
        cerr << "root file open failed: " << options_mgr.scifile << endl;
        return 1;
    }

    HkGPSIterator hkGPSIter;
    if (!hkGPSIter.open(options_mgr.hkfile.Data())) {
        cerr << "root file open falied: " << options_mgr.hkfile << endl;
        scitran.close_read();
        return 1;
    }

    Processor pro;
    bool log_flag = false;
    if (!options_mgr.logfile.IsNull()) {
        log_flag = true;
        pro.set_log(log_flag);
        if (!pro.logfile_open(options_mgr.logfile.Data())) {
            cerr << "log file open failed: "
                 << options_mgr.logfile.Data() << endl;
            scitran.close_read();
            hkGPSIter.close();
            return 1;
        }
    } else {
        pro.set_log(log_flag);
    }

    hkGPSIter.initialize();
    if (!pro.check_sci_hk_match(scitran, hkGPSIter)) {
        cerr << "SCI file and HK file do not match in GPS time." << endl;
        scitran.close_read();
        hkGPSIter.close();
        if (log_flag) {
            pro.logfile_close();
        }
        return 1;
    }

    if (!options_mgr.outfile.IsNull()) {
        if (!scitran.open_write(options_mgr.outfile.Data())) {
            cerr << "root file open failed: "
                 << options_mgr.outfile.Data() << endl;
            scitran.close_read();
            hkGPSIter.close();
            if (log_flag) {
                pro.logfile_close();
            }
            return 1;
        }
    } else {
        if (!scitran.open_write("POL_SCI_decoded_data_time.root")) {
            cerr << "root file open failed: "
                 << "POL_SCI_decoded_data_time.root" << endl;
            scitran.close_read();
            hkGPSIter.close();
            if (log_flag) {
                pro.logfile_close();
            }
            return 1;
        }
    }

    // === Start Process Data =======================================

    pro.copy_modules(scitran);
    pro.calc_time_trigger(scitran, hkGPSIter);
    pro.copy_ped_modules(scitran);
    pro.calc_time_ped_trigger(scitran, hkGPSIter);

    // === End Process Data ====================================

    scitran.write_all_tree();
    pro.write_meta_info(scitran);

    scitran.close_read();
    scitran.close_write();
    hkGPSIter.close();
    if (log_flag) {
        pro.logfile_close();
    }

    pro.print_error_count(scitran);

    return 0;
}
