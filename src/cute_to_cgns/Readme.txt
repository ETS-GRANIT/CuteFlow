#Compute Canada Python virtual env https://docs.alliancecan.ca/wiki/Python
#Creer l'env virtuel
module load gcc cgns python/3.8.10
virtualenv ENV #juste la première fois
source ENV/bin/activate #Pour entrer dans l'ENV
pip install --no-index --upgrade pip #juste la première fois
pip install numpy scipy matplotlib pyCGNS #juste la première fois
deactivate #Pour sortir de l'ENV

#Pour lancer le code :
python3 cute_to_cgns.py mesh.txt multi_entree

mesh.txt est le fichier maillage d'entrée
multi_entree prend la valeur 1 ou 0 selon si le maillage contient plusieurs entrées ou non
un fichier mesh.cgns sera crée en sortie

#Note: Le fichier cute_to_cgns.cpp correspond à une version C++ de la
conversion des fichiers de maillages. Elle peut être utile pour garder en
mémoire comment utiliser la bibliothèque CGNS pour créer un fichier de
maillage au format CGNS en C++. Il faut dans ce genre de code faire
particulièrement attention à la précision qui est utilisée pour lire et écrire
les coordonnées des noeuds du maillage.
