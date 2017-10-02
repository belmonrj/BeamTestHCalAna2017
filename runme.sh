#!/bin/sh

root -b -l -q CDR_compare_plot.C
root -b -l -q DrawPrototype2HCAL.C
#root -b -l -q Fun4All_scan.C
root -b -l -q Resolution_generic.C
root -b -l -q Resolution_generic_FullCalo.C
#root -b -l -q balance_emcal_weight.C
root -b -l -q compare_data.C
root -b -l -q compare_signal_sim.C
root -b -l -q cosmics_hcal_compare.C
root -b -l -q cosmics_hcalin.C
root -b -l -q cosmics_hcalin_compare.C
root -b -l -q cosmics_hcalout.C
root -b -l -q cosmics_hcalout_compare.C
root -b -l -q hcal_balance.C
root -b -l -q plot_hcal.C
root -b -l -q self_check.C
#root -b -l -q self_check2.C

