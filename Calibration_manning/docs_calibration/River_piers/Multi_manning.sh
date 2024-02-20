#!/usr/bin/bash

for i in {1..256}
do
   cd Case_$i
   cp ../base_files/River_wth_piers.cgns .
   python3 gen_manning.py River_wth_piers.cgns

   mv manning_River_wth_piers.cgns manning_River_wth_piers_Case_$i.cgns

   rm River_wth_piers.cgns
   cd ..
done
let i=$i+1
