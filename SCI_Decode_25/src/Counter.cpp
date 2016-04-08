#include "Counter.hpp"

void Counter::clear() {
    frame = 0;
    packet = 0;
    trigger = 0;
    ped_trigger = 0;
    sec_ped_trigger = 0;
    tin_ped_trigger = 0;
    noped_trigger = 0;
    event = 0;
    frm_con_error = 0;
    frm_valid = 0;
    frm_invalid = 0;
    frm_crc_passed = 0;
    frm_crc_error = 0;
    frm_start_error = 0;
    pkt_valid = 0;
    pkt_invalid = 0;
    pkt_crc_passed = 0;
    pkt_crc_error = 0;
    pkt_too_short = 0;
    for (int i = 0; i < 25; i++) {
        ped_trig[i] = 0;
        ped_event[i] = 0;
        noped_trig[i] = 0;
        noped_event[i] = 0;
    }   
    aligned_event_sum = 0;
    timestamp_zero_sum = 0;
}

void Counter::print(ostream& os) {
    char str_buffer1[50];
    char str_buffer2[50];
    char str_buffer3[50];
    os << "===========================================================================================================" << endl;
    os << left;
    os << setfill(' ');
    os << setw(28) << "total frame count: " <<setw(20) << frame
         << setw(28) << "total packet count: " << setw(10) << packet << endl;
    os << setw(28) << "frame invalid count: " << setw(20) << frm_invalid
         << setw(28) << "- trigger packet count: " << setw(10) << trigger << endl;
    sprintf(str_buffer1, "%4.2f%%", frame > 0 ? static_cast<double>(frm_invalid) / frame * 100 : 0.0);
    os << setw(28) << "frame invalid percent: " << setw(20) << str_buffer1
         << setw(28) << "- event packet count: " << setw(10) << event << endl;
    os << setw(28) << "frame crc error count: " << setw(20) << frm_crc_error
         << setw(28) << "packet invalid count: " << setw(10) << pkt_invalid << endl;
    sprintf(str_buffer1, "%4.2f%%", frame > 0 ? static_cast<double>(frm_crc_error) / frame * 100 : 0.0);
    sprintf(str_buffer2, "%4.2f%%", packet > 0 ? static_cast<double>(pkt_invalid) / packet * 100 : 0.0);
    os << setw(28) << "frame crc error percent: " << setw(20) << str_buffer1
         << setw(28) << "packet invalid percent: " << setw(10) << str_buffer2 << endl;
    os << setw(28) << "frame interruption count: " << setw(20) << frm_con_error
         << setw(28) << "packet crc error count: " << setw(10) << pkt_crc_error << endl;
    sprintf(str_buffer2, "%4.2f%%", packet > 0 ? static_cast<double>(pkt_crc_error) / packet * 100 : 0.0);
    os << setw(28) << "frame start error count: " << setw(20) << frm_start_error
         << setw(28) << "packet crc error percent: " << setw(10) << str_buffer2 << endl;
    os << setw(28) << "total timestamp 0 count: " << setw(20) << timestamp_zero_sum
         << setw(28) << "packet too short count: " << setw(20) << pkt_too_short << endl;
    os << right;
    os << "-----------------------------------------------------------------------------------------------------------" << endl;
    int64_t trigg_sum = 0;
    int64_t event_sum = 0;
    int64_t noped_event_sum = 0;
    os << setw(4) << "ct"
         << setw(5) << "mod"
         << setw(3) << " >"
         << setw(11) << "ped_trig"
         << setw(11) << "ped_event"
         << setw(11) << "ped_lost"
         << setw(9) << "percent"
         << setw(5) << "   |"
         << setw(13) << "noped_trig"
         << setw(13) << "noped_event"
         << setw(13) << "noped_lost"
         << setw(9) << "percent"
         << endl;
    for (int i = 0; i < 25; i++) {
        os << setw(4) << i + 1
             << setw(5) << ModuleIndex[i]
             << setw(3) << " >"
             << setw(11) << ped_trig[i]
             << setw(11) << ped_event[i]
             << setw(11) << ped_trig[i] - ped_event[i];
        sprintf(str_buffer1, "%4.2f%%", ped_trig[i] > 0 ? static_cast<double>(ped_trig[i] - ped_event[i]) / ped_trig[i] * 100 : 0.0);
        os << setw(9) << str_buffer1
             << setw(5) << "   |"
             << setw(13) << noped_trig[i]
             << setw(13) << noped_event[i]
             << setw(13) << noped_trig[i] - noped_event[i];
        sprintf(str_buffer1, "%4.2f%%", noped_trig[i] ? static_cast<double>(noped_trig[i] - noped_event[i]) / noped_trig[i] * 100 : 0.0);
        os << setw(9) << str_buffer1
             << endl;
        trigg_sum += (ped_trig[i] + noped_trig[i]);
        event_sum += (ped_event[i] + noped_event[i]);
        noped_event_sum += noped_event[i];
    }
    os << "-----------------------------------------------------------------------------------------------------------" << endl;
    os << left;
    os << setw(20) << "trigg expected sum:" << setw(15) << trigg_sum 
         << setw(17) << "noped_trigger: " << setw(20) << noped_trigger
         << setw(17) << "ped_trigger:" << setw(20) << ped_trigger << endl;
    os << setw(20) << "event received sum:" << setw(15) << event_sum 
         << setw(17) << "noped_event_sum:" << setw(20) << noped_event_sum
         << setw(17) << "sec_ped_trigger:" << setw(20) << sec_ped_trigger << endl;
    sprintf(str_buffer1, "%4.2f%%", trigg_sum > 0 ? static_cast<double>(trigg_sum - event_sum) / trigg_sum * 100 : 0.0);
    sprintf(str_buffer2, "%d cnts/sec", sec_ped_trigger > 0 ? static_cast<int>(noped_trigger / (sec_ped_trigger * PedSecond)) : 0);
    sprintf(str_buffer3, "%d pkts/sec", sec_ped_trigger > 0 ? static_cast<int>(noped_event_sum / (sec_ped_trigger * PedSecond)) : 0);
    os << setw(20) << "total lost percent:" << setw(15) << str_buffer1
         << setw(17) << "mean event rate:"  << setw(20) << str_buffer2 
         << setw(17) << "np_evts per sec:" << setw(20) << str_buffer3 << endl;
    sprintf(str_buffer1, "%4.2f Mbps",  (sec_ped_trigger > 0 ? static_cast<double>(frm_valid * 2026 - tin_ped_trigger * (50 + 122 * 25)) * 8 / 1000 / 1000 / sec_ped_trigger : 0));
    sprintf(str_buffer3, "%4.2f%%", noped_event_sum > 0 ? static_cast<double>(aligned_event_sum) / noped_event_sum * 100 : 0.0);
    os << setw(20) << "transmission rate:" << setw(15) << str_buffer1
         << setw(17) << "aligned sum:" << setw(20) << aligned_event_sum 
         << setw(17) << "aligned percent:" << setw(20) << str_buffer3 << endl;
    os << right;
    os << "===========================================================================================================" << endl;
}

int Counter::get_time_span() {
    return static_cast<int>(sec_ped_trigger * PedSecond);
}

int Counter::get_mean_rate() {
    return (sec_ped_trigger > 0 ? static_cast<int>(noped_trigger / (sec_ped_trigger * PedSecond)) : 0);
}

float Counter::get_lost_percent() {
    int64_t trigg_sum = 0;
    int64_t event_sum = 0;
    for (int i = 0; i < 25; i++) {
        trigg_sum += (ped_trig[i] + noped_trig[i]);
        event_sum += (ped_event[i] + noped_event[i]);
    }
    return static_cast<float>(trigg_sum > 0 ? static_cast<double>(trigg_sum - event_sum) / trigg_sum * 100 : 0.0);
}

float Counter::get_aligned_percent() {
    int64_t noped_event_sum = 0;
    for (int i = 0; i < 25; i++) {
        noped_event_sum += noped_event[i];
    }
    return static_cast<float>(noped_event_sum > 0 ? static_cast<double>(aligned_event_sum) / noped_event_sum * 100 : 0.0);
}
