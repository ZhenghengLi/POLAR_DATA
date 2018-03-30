#!/bin/bash

environment1='#!/bin/bash'
environment2='source /workfs/hxmt/lizhengheng/software/thisenv.sh > /dev/null'
environment3='source /workfs/hxmt/lizhengheng/psdc_sw/gitlab/POLAR_DATA/env.sh > /dev/null'
environment4='source /workfs/hxmt/lizhengheng/psdc_sw/gitlab/POLAR_SIM/env.sh > /dev/null'

starttime='starttime=$(date +%s)'
endtime='endtime=$(date +%s)'
worktime='echo "All works have been done using: $((endtime-starttime)) S." '

###################################################

seed=100
number_of_events=100000000

GRB_NAME=GRB_170206A
polstr=pol

basedir_in=/workfs/hxmt/lizhengheng/polar_analysis/SCI_1Q_Analysis/GRB_POL_DA/$GRB_NAME/simulation
macrodir=$basedir_in/$polstr/gps

basedir_out=/scratchfs/hxmt/lizhengheng/POLAR_GRB_POL_DA/$GRB_NAME
jobsdir=$basedir_out/${polstr}_jobs
outputdir=$basedir_out/${polstr}_simout

sim_exe=/workfs/hxmt/lizhengheng/POLAR_SIM/POLARsim/bin/POLARsim
sim_cfg=$basedir_in/sim_config.mac

digi_exe=/workfs/hxmt/lizhengheng/psdc_sw/gitlab/POLAR_SIM/Analysis/bin/sim_digitalize
digi_cfg=$basedir_in/calib/GRB_170206A.cfg

energy_exe=/workfs/hxmt/lizhengheng/psdc_sw/gitlab/POLAR_DATA/Analysis_SCI_1Q/bin/energy_conv
angle_exe=/workfs/hxmt/lizhengheng/psdc_sw/gitlab/POLAR_DATA/Analysis_SCI_1Q/bin/angle_calc_1Q_m2

ped_temp_file=/workfs/hxmt/lizhengheng/POLAR_CALIB/in_orbit/ped_vs_temp/ped_noise_temp_20171227.root
nonlin_file=/workfs/hxmt/lizhengheng/POLAR_CALIB/in_orbit/nonlinearity/bank50/nonlin_norm_fun_bank50.root
xtalk_file=/workfs/hxmt/lizhengheng/POLAR_CALIB/in_orbit/crosstalk/xtalk_mat_bank50.root
gain_file=/workfs/hxmt/lizhengheng/POLAR_CALIB/in_orbit/gain_vs_hv/gain_bank50_refined.root
gain_temp_file=/workfs/hxmt/lizhengheng/POLAR_CALIB/in_orbit/gain_temp/gain_temp_mod_mean.root

for macfile in $macrodir/*.mac; do
    macfile_fn=$(basename $macfile)
    echo $macfile_fn

    run_file=$jobsdir/run_$macfile_fn
    echo "/random/setSeeds $seed $seed"     >  $run_file
    echo "/run/verbose 1"                   >> $run_file
    echo "/run/beamOn $number_of_events"    >> $run_file

    simout_file=$outputdir/sim_${macfile_fn}.root
    digiout_file=$outputdir/digi_sim_${macfile_fn}.root
    energy_file=$outputdir/energy_digi_sim_${macfile_fn}.root
    angle_file=$outputdir/angle_energy_digi_sim_${macfile_fn}.root

    sim_cmd="$sim_exe $macfile $run_file -c $sim_cfg -f -o $simout_file"
    digi_cmd="$digi_exe $simout_file -c $digi_cfg -t IHEP -o $digiout_file"
    energy_cmd="$energy_exe $digiout_file -P $ped_temp_file -f $nonlin_file -x $xtalk_file -g $gain_file -t $gain_temp_file -o $energy_file"
    angle_cmd="$angle_exe $energy_file -o $angle_file"

    jobs_file=$jobsdir/simjob_${macfile_fn}.sh

    echo $environment1 >  $jobs_file
    echo $environment2 >> $jobs_file
    echo $environment3 >> $jobs_file
    echo $environment4 >> $jobs_file
    echo $starttime    >> $jobs_file
    echo $sim_cmd      >> $jobs_file
    echo $digi_cmd     >> $jobs_file
    echo $energy_cmd   >> $jobs_file
    echo $angle_cmd    >> $jobs_file
    echo $endtime      >> $jobs_file
    echo $worktime     >> $jobs_file

    chmod +x $jobs_file

done
