#!/usr/bin/bash

for i in {1..256}
do
   cd Case_$i
   cp ../base_files/Canal_divergent.cgns .
   python3 gen_manning.py Canal_divergent.cgns

   mv manning_Canal_divergent.cgns manning_Canal_divergent_Case_$i.cgns

   rm Canal_divergent.cgns
   cd ..
done
let i=$i+1
