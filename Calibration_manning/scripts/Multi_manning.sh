#!/usr/bin/bash

for i in {1..128}
do
   cd Case_$i
   cp ../base_files/Mille_Iles_mesh_481930_elts.cgns .
   python3 gen_manning.py Mille_Iles_mesh_481930_elts.cgns

   mv manning_Mille_Iles_mesh_481930_elts.cgns manning_Mille_Iles_mesh_481930_elts_Case_$i.cgns

   rm Mille_Iles_mesh_481930_elts.cgns
   cd ..
done
let i=$i+1
