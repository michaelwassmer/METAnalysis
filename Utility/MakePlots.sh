in_file=$1
out_dir=$2
process=$3
# Raw, T1, T1Smear and Gen
rootplot rootplot_config.py  $in_file pt_genmet pt_pfmet_raw pt_pfmet_t1 pt_pfmet_t1smear --processors=1 --size=1024x768 --xlabel="#slash{E}_{T}[GeV]" --ylabel="arbitrary units" --legend-location='upper right' --gridx --gridy -e pdf --draw='histe' --output="${out_dir}" --noclean --legend-entries='Gen,Raw,T1,T1Smear' --title="#bf{2018, ${process}, 13 TeV}"
mv ${out_dir}/plot.pdf ${out_dir}/METs.pdf
rootplot rootplot_config.py  $in_file pt_genmet pt_pfmet_raw pt_pfmet_t1 pt_pfmet_t1smear --processors=1 --size=1024x768 --xlabel="#slash{E}_{T}[GeV]" --ylabel="arbitrary units" --legend-location='upper right' --gridx --gridy -e pdf --draw='histe' --output="${out_dir}" --noclean --legend-entries='Gen,Raw,T1,T1Smear' --title="#bf{2018, ${process}, 13 TeV}" --logy
mv ${out_dir}/plot.pdf ${out_dir}/METs_log.pdf
rootplot rootplot_config.py  $in_file pt_genmet pt_pfmet_raw pt_pfmet_t1 pt_pfmet_t1smear pt_genmet --ratio=5 --processors=1 --size=1024x768 --xlabel="#slash{E}_{T}[GeV]" --ylabel="ratio to Gen" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='Gen,Raw,T1,T1Smear,bla' --title="#bf{2018, ${process}, 13 TeV}" -m --ymin=0.0 --ymax=2.0
mv ${out_dir}/plot.pdf ${out_dir}/MET_ratios.pdf
#rootplot rootplot_config.py  $in_file pt_pfmet_t1_div_pt_pfmet_raw pt_pfmet_t1smear_div_pt_pfmet_raw --processors=1 --size=1024x768 --xlabel="#slash{E}_{T,T1/T1Smear}/#slash{E}_{T,Raw}" --ylabel="arbitrary units" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='T1,T1Smear' --title="#bf{2018, ${process}, 13 TeV}"
#mv ${out_dir}/plot.pdf ${out_dir}/MET_t1_t1smear_div_raw.pdf
#rootplot rootplot_config.py  $in_file pt_pfmet_raw_div_pt_genmet pt_pfmet_t1_div_pt_genmet pt_pfmet_t1smear_div_pt_genmet --processors=1 --size=1024x768 --xlabel="#slash{E}_{T,Raw/T1/T1Smear}/#slash{E}_{T,Gen}" --ylabel="arbitrary units" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='Raw,T1,T1Smear' --title="#bf{2018, ${process}, 13 TeV}"
#mv ${out_dir}/plot.pdf ${out_dir}/MET_raw_t1_t1smear_div_gen.pdf

# T1 vs Raw
rootplot rootplot_config.py  $in_file pt_pfmet_raw pt_pfmet_t1 pt_pfmet_raw --ratio=3 --processors=1 --size=1024x768 --xlabel="#slash{E}_{T}[GeV]" --ylabel="ratio to Raw" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='Raw,T1,bla' --title="#bf{2018, ${process}, 13 TeV}" -m --ymin=0.0 --ymax=2.0
mv ${out_dir}/plot.pdf ${out_dir}/MET_ratio_raw_t1.pdf

# T1 vs T1Smear
rootplot rootplot_config.py  $in_file pt_pfmet_t1 pt_pfmet_t1smear pt_pfmet_t1 --ratio=3 --processors=1 --size=1024x768 --xlabel="#slash{E}_{T}[GeV]" --ylabel="ratio to T1" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='T1,T1Smear,bla' --title="#bf{2018, ${process}, 13 TeV}" -m --ymin=0.0 --ymax=2.0
mv ${out_dir}/plot.pdf ${out_dir}/MET_ratio_t1_t1smear.pdf
#rootplot rootplot_config.py  $in_file pt_pfmet_t1smear_div_pt_pfmet_t1 --processors=1 --size=1024x768 --xlabel="#slash{E}_{T,T1Smear}/#slash{E}_{T,T1}" --ylabel="arbitrary units" --legend-location='None' --gridx --gridy -e pdf --output="${out_dir}" --noclean --title="#bf{2018, ${process}, 13 TeV}"
#mv ${out_dir}/pt_pfmet_t1smear_div_pt_pfmet_t1.pdf ${out_dir}/MET_t1_div_t1smear.pdf

# T1 JES
rootplot rootplot_config.py  $in_file pt_pfmet_t1 pt_pfmet_t1_jes_up pt_pfmet_t1_jes_down pt_pfmet_t1 --ratio=4 --processors=1 --size=1024x768 --xlabel="#slash{E}_{T}[GeV]" --ylabel="ratio to T1" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='T1,T1JesUp,T1JesDown,bla' --title="#bf{2018, ${process}, 13 TeV}" -m --ymin=0.0 --ymax=2.0
mv ${out_dir}/plot.pdf ${out_dir}/MET_ratio_t1_t1jes.pdf
#rootplot rootplot_config.py  $in_file pt_pfmet_t1_jes_up_div_pt_pfmet_t1 pt_pfmet_t1_jes_down_div_pt_pfmet_t1 --processors=1 --size=1024x768 --xlabel="#slash{E}_{T,T1Jes}/#slash{E}_{T,T1}" --ylabel="arbitrary units" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='Up,Down' --title="#bf{2018, ${process}, 13 TeV}"
#mv ${out_dir}/plot.pdf ${out_dir}/MET_t1jes_div_t1.pdf

# T1 JER
rootplot rootplot_config.py  $in_file pt_pfmet_t1 pt_pfmet_t1_jer_up pt_pfmet_t1_jer_down pt_pfmet_t1 --ratio=4 --processors=1 --size=1024x768 --xlabel="#slash{E}_{T}[GeV]" --ylabel="ratio to T1" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='T1,T1JerUp,T1JerDown,bla' --title="#bf{2018, ${process}, 13 TeV}" -m --ymin=0.0 --ymax=2.0
mv ${out_dir}/plot.pdf ${out_dir}/MET_ratio_t1_t1jer.pdf
#rootplot rootplot_config.py  $in_file pt_pfmet_t1_jer_up_div_pt_pfmet_t1 pt_pfmet_t1_jer_down_div_pt_pfmet_t1 --processors=1 --size=1024x768 --xlabel="#slash{E}_{T,T1Jer}/#slash{E}_{T,T1}" --ylabel="arbitrary units" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='Up,Down' --title="#bf{2018, ${process}, 13 TeV}"
#mv ${out_dir}/plot.pdf ${out_dir}/MET_t1jer_div_t1.pdf

# T1 UnclEn
rootplot rootplot_config.py  $in_file pt_pfmet_t1 pt_pfmet_t1_uncen_up pt_pfmet_t1_uncen_down pt_pfmet_t1 --ratio=4 --processors=1 --size=1024x768 --xlabel="#slash{E}_{T}[GeV]" --ylabel="ratio to T1" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='T1,T1UnclEnUp,T1UnclEnDown,bla' --title="#bf{2018, ${process}, 13 TeV}" -m --ymin=0.0 --ymax=2.0
mv ${out_dir}/plot.pdf ${out_dir}/MET_ratio_t1_t1unclen.pdf
#rootplot rootplot_config.py  $in_file pt_pfmet_t1_uncen_up_div_pt_pfmet_t1 pt_pfmet_t1_uncen_down_div_pt_pfmet_t1 --processors=1 --size=1024x768 --xlabel="#slash{E}_{T,T1UnclEn}/#slash{E}_{T,T1}" --ylabel="arbitrary units" --legend-location='upper right' --gridx --gridy -e pdf --output="${out_dir}" --noclean --legend-entries='Up,Down' --title="#bf{2018, ${process}, 13 TeV}"
#mv ${out_dir}/plot.pdf ${out_dir}/MET_t1unclen_div_t1.pdf
