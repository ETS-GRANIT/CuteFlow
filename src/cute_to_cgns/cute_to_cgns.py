#!/usr/bin/python3

import numpy as np
import sys
import os

if(len(sys.argv) <= 2) : 
    print("[-] Erreur: Pas assez d'arguments d'entrée")
    print("[-] Utilisation: python3 cute_to_cgns.py input_mesh.txt multi_entree(0/1)")
    exit(0)

multi_entree = int(sys.argv[2])
if(multi_entree!=0 and multi_entree!=1):
    print("[-] Erreur: Pas assez d'arguments d'entrée")
    print("[-] Utilisation: python3 cute_to_cgns.py input_mesh.txt multi_entree(0/1)")
    exit(0)

#Lecture du fichier utilisant l'ancien format pour Cuteflow
output_file = os.path.splitext(sys.argv[1])[0] + str(".cgns")
with open(sys.argv[1], "r", encoding="latin-1") as f:
        lines = [line.rstrip() for line in f]

#Lecture des noeuds du maillage
nNodes = int(lines[1])
x_array = np.zeros(nNodes, dtype=np.float64)
y_array = np.zeros(nNodes, dtype=np.float64)
z_array = np.zeros(nNodes, dtype=np.float64)
for i in range(nNodes):
    x_array[i], y_array[i], z_array[i] = np.array(lines[2+i].split()[1:4]).astype(np.float64)

#Lecture des éléments du maillage
nElems = int(lines[3+nNodes])
elems = np.zeros(nElems*3, np.int64)
for i in range(nElems):
    elems[3*i], elems[3*i+1], elems[3*i+2] = np.array(lines[4+nNodes+i].split()[1:4]).astype(np.int64)


#Lecture de(s) entrées
if(multi_entree==0):
    nNe = int(lines[5+nNodes+nElems])
    entree_nodes = np.zeros(nNe, dtype=np.int64)
    for i in range(nNe):
        entree_nodes[i] = np.array(lines[6+nNodes+nElems+i]).astype(np.int32)
else:
    nNe = int(lines[5+nNodes+nElems].split()[0])
    nEntree = int(lines[5+nNodes+nElems].split()[1])
    entree_nodes = np.zeros(nNe, dtype=np.int64)
    entree_nodes_multi = np.zeros(nNe*nEntree, dtype=np.int64)
    k = np.zeros(nEntree, dtype=np.int32)
    for i in range(nNe):
        entree_nodes[i], p = np.array(lines[6+nNodes+nElems+i].split()).astype(np.int32)
        entree_nodes_multi[(p-1)*nNe+k[p-1]] = entree_nodes[i]
        k[p-1] = k[p-1] + 1

#Lecture de la sortie
nNs = int(lines[7+nNodes+nElems+nNe])
sortie_nodes = np.zeros(nNs, dtype=np.int64)
for i in range(nNs):
    sortie_nodes[i] = np.array(lines[8+nNodes+nElems+nNe+i]).astype(np.int64)

#Ecriture du fichier au format CGNS
#Documentation de pycgns https://pycgns.github.io/index.html
#Documentation du module PAT https://pycgns.github.io/PAT/_index.html
import CGNS.MAP
import CGNS.PAT.cgnskeywords as CK
import CGNS.PAT.cgnsutils as CGU
import CGNS.PAT.cgnslib as CGL
import CGNS.PAT.test.disk as data

T=CGL.newCGNSTree()

celldim = 2
physdim = 3
B=CGL.newBase(T,"Base",celldim,physdim)

s=np.zeros((1,3), dtype=np.int32)
s[0][0] = nNodes
s[0][1] = nElems
s[0][2] = 0
Z=CGL.newZone(B,"Zone",s,CK.Unstructured_s)

cx = CGL.newCoordinates(Z, CK.CoordinateX_s, x_array)
cy = CGL.newCoordinates(Z, CK.CoordinateY_s, y_array)
cz = CGL.newCoordinates(Z, CK.CoordinateZ_s, z_array)

S = CGL.newElements(Z, "Elements", CK.TRI_3, np.array([1, nElems]), elems)

ZBC = CGL.newZoneBC(Z)
if(multi_entree==1):
    for i in range(nEntree):
        Be = CGL.newBoundary(ZBC, "Inflow nodes "+str(i), np.array([entree_nodes_multi[i*nNe:i*nNe+k[i]]]), CK.BCInflow_s, "", CK.PointList_s)
else:
    Be = CGL.newBoundary(ZBC, "Inflow nodes", np.array([entree_nodes]), CK.BCInflow_s, "", CK.PointList_s)
Bs = CGL.newBoundary(ZBC, "Outflow nodes", np.array([sortie_nodes]), CK.BCOutflow_s, "", CK.PointList_s)

status = CGNS.MAP.save(output_file, T)
print("[+] ",output_file, " was written")
