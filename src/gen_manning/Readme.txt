#Compute Canada Python virtual env https://docs.alliancecan.ca/wiki/Python
#Creer l'env virtuel
module load gcc cgns python/3.8.10
virtualenv ENV #juste la première fois
source ENV/bin/activate #Pour entrer dans l'ENV
pip install --no-index --upgrade pip #juste la première fois
pip install numpy scipy matplotlib pyCGNS #juste la première fois
deactivate #Pour sortir de l'ENV

#Pour lancer le code
python3 main.py archi_mod.cgns #créer les fichiers mannings.vtk et manning_archi_mod.cgns

#Décomposer le fichier en 4 sous-domaines
mpirun -n 2 ../parmetis_ghost/build/main 4 manning_archi_mod.cgns 0 2 #créer un fichier Mesh_Output_pcgns_ch.cgns
mv Mesh_Output_pcgns_ch.cgns manning_archi_mod_4_2.cgns

#Ensuite utilise manning_archi_mod_4_2.cgns avec cuteflow avec 4GPUs
