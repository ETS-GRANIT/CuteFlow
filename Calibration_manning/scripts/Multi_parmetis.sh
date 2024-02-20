#!/usr/bin/bash
ordre=1;
# Vérifiez si un argument est fourni
if [ $# -ne 1 ]; then
    echo "Usage: <scriptname> <maillage.cgns>"
    exit 1
fi
mesh_name="$1"
new_mesh_name="manning_${mesh_name}"

for i in {1..2}
do
    cd Case_$i
    cp ../base_files/main .
    mpirun -n 2 ./main 2 $new_mesh_name 0 $ordre 

    mv Mesh_Output_pcgns_ch.cgns "${new_mesh_name%.*}_"$ordre"_Case_$i.${new_mesh_name##*.}"
    rm main
    cd ..
done
