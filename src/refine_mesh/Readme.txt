#Le but du code refine_mesh est de rafiner un fichier de maillage. Dans sa
#version actuelle ce code ne marche que sur les fichiers au format txt et pas
#cgns. Ce code peut être mis a jour vers le format cgns en utilisant comme
#exemple le code cute_to_cgns qui écrit déjà des fichiers au format cgns en
#C++ ou bien le code refine_mesh peut être converti en Python avec comme
#exemple de lecture/ecriture les codes cute_to_cgns.py/gen_manning.py

#Pour compiler le code :
make

#Pour lancer le code :
./refine_mesh mesh.txt multi_entrees multi_sorties tol

mesh.txt le maillage au format txt
multi_entrees prend la valeur 0 ou 1 selon si le maillage contient plusieurs entrées ou non
multi_sorties prend la valeur 0 ou 1 selon si le maillage contient plusieurs sorties ou non
tol est le plus petit rayon de cercle inscrit dans le maillage après rafinement (tol=0.0 pour tout rafiner)
