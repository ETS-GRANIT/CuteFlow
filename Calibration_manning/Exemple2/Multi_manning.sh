#!/usr/bin/bash

for i in {1..16}
do
   cd Case_$i
   cp ../base_files/mesh_6072.cgns .
   python3 gen_manning.py mesh_6072.cgns

   mv manning_mesh_6072.cgns manning_mesh_6072_Case_$i.cgns

   rm mesh_6072.cgns
   cd ..
done
let i=$i+1
