#Compute Canada Python virtual env https://docs.alliancecan.ca/wiki/Python
#Creer l'env virtuel
module load gcc cgns python/3.8.10
virtualenv ENV #juste la première fois
source ENV/bin/activate #Pour entrer dans l'ENV
pip install --no-index --upgrade pip #juste la première fois
pip install numpy scipy matplotlib pyCGNS #juste la première fois
deactivate #Pour sortir de l'ENV

#Pour lancer le code :
python3 cute_to_cgns.py input_mesh.txt multi_entree

input_mesh.txt est le fichier maillage d'entrée
multi_entree prends la valeur 1 ou 0 selon si le maillage contient plusieurs entrées ou non
