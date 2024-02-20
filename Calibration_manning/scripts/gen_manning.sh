#!/usr/bin/bash

echo "
#Documentation de pycgns https://pycgns.github.io/index.html
#Documentation du module PAT https://pycgns.github.io/PAT/_index.html

import CGNS.MAP
import CGNS.PAT.cgnskeywords as CK
import CGNS.PAT.cgnsutils as CGU
import CGNS.PAT.cgnslib as CGL
import CGNS.PAT.test.disk as data
from math import *
import numpy as np
import matplotlib.pyplot as plt
from sys import argv
from scipy import interpolate

# Définition du nombre de manning dominant
average_manning = float($1)

# Valeurs du coefficient de manning aux cellules à modifier
cell_index = np.array([], dtype=np.int64)
manning_value = np.array([],dtype=np.float64)

# Lire les données à partir du fichier text
with open('./Entree_gen_manning_$2.txt', 'r') as file:
    lines = file.readlines()

# Extraire les valeurs de Cells IDs et Manning_values
cell_ids_line = lines[1]
manning_values_line = lines[3]

# Supprimer les caractères de nouvelle ligne
cell_ids_line = cell_ids_line.strip()
manning_values_line = manning_values_line.strip()

# Convertir les lignes en listes d'entiers et de nombres à virgule flottante
cell_ids = [int(value) for value in cell_ids_line.split(',')]
manning_values = [float(value) for value in manning_values_line.split(',')]

# Remplir les tableaux
cell_index = np.array(cell_ids, dtype=np.int64)
manning_value = np.array(manning_values, dtype=np.float64)

#Lecture du fichier CGNS
(tree,links,paths)=CGNS.MAP.load(argv[1]) # Load le fichier CGNS donné en argument
node=CGU.getNodeByPath(tree,\"/Base/Zone\") # Récupère le noeud correspondant au chemin
zone = tree[2][1][2]
coord = zone[0][2][1][2]
conn = zone[0][2][2][2][1][1]
x = coord[0][1]
y = coord[1][1]
z = coord[2][1]
nnodes = int(len(x))
nelems = int(len(conn)/3)
mannings = np.zeros(int(len(conn)/3), dtype=np.float64)
nout = 0

for i in range (nelems):
    mannings[i]=average_manning

for j in range (len(cell_index)):
    mannings[cell_index[j]]=manning_value[j]

#Correction potentielle des nombres de mannings s'ils sont trop grands ou trop petits
if(mannings[i] > 0.05 or mannings[i] < 0.): nout+=1
if(mannings[i] > 0.05) : mannings[i] = 0.05
if(mannings[i] < 0.) : mannings[i] = 0.01

print(nout,\"values out of range [0.00 ; 0.05]\")
man = CGL.newUserDefinedData(node, \"Manning\")
CGL.newGridLocation(man, CK.CellCenter_s)
CGL.newDataArray(man, '{DataArray}', value=mannings)
status = CGNS.MAP.save('manning_'+argv[1],tree) #Sauvegarde l'arbre dans un nouveau fichier

#Ecriture du fichier de maillage avec les nombre de manning pour visualisation avec Paraview
f = open(\"mannings.vtk\",\"w\")
f.write(\"# vtk DataFile Version 2.0\\n\")
f.write(\"Manning\\n\")
f.write(\"ASCII\\n\")
f.write(\"DATASET UNSTRUCTURED_GRID\\n\")
f.write(\"POINTS \"+str(nnodes)+\" double\\n\")
for i in range(nnodes):
    f.write(str(x[i])+ \" \" +str(y[i]) + \" \" + str(z[i]) + \"\\n\")
f.write(\"CELLS \" + str(nelems) + \" \" + str(nelems*4) +\"\\n\")
for i in range(nelems):
    f.write(\"3 \" + str(conn[3*i]-1)+ \" \" +str(conn[3*i+1]-1) + \" \" + str(conn[3*i+2]-1) + \"\\n\")
f.write(\"CELL_TYPES \" + str(nelems) +\"\\n\")
for i in range(nelems):
    f.write(\"5\\n\")
f.write(\"CELL_DATA \" + str(nelems) +\"\\n\")
f.write(\"SCALARS Manning double\\n\")
f.write(\"LOOKUP_TABLE default\\n\")
for i in range(nelems):
    f.write(str(mannings[i]) + \"\\n\")
"> gen_manning.py