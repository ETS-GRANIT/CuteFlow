#!/usr/bin/python3
import numpy as np
from sys import argv
from math import *
from scipy.spatial import Delaunay

#Dimension spaciale du maillage
x0=0.; x1=10.
y0=0.; y1=1.

#Nombre de points dans la direction y
#C'est le seul paramètre à changer si on veut raffiner le maillage uniformément
ny = 16

# h est la longueur d'un côté des triangles équilatéraux
h = (y1-y0)/ny
dx = sqrt(3)/2 * h
dy = h

nx = int((x1-x0)/dx) + 1
if(nx%2==1): nx=nx+1

nNodes=(nx+1)*(ny+2)
nodes = np.ndarray((nNodes, 3))

inod = 0
for i in range(0,int(nx/2)):
    if(i%2==0):
        for j in range(0,ny+1):
            nodes[inod][0] = (x1+x0)/2. + i*dx
            nodes[inod][1] = y0 + j*h
            inod=inod+1
    else:
        for j in range(0,ny+2):
            nodes[inod][0] = (x1+x0)/2. + i*dx
            if(j==0): 
                nodes[inod][1] = y0
            elif(j==ny+1): 
                nodes[inod][1] = y1
            else:
                nodes[inod][1] = y0 + j*h - h/2.
            inod=inod+1

for i in range(1,int(nx/2)):
    nodes[inod][2] = 0.
    if(i%2==0):
        for j in range(0,ny+1):
            nodes[inod][0] = (x1+x0)/2. - i*dx
            nodes[inod][1] = y0 + j*h
            inod=inod+1
    else:
        for j in range(0,ny+2):
            nodes[inod][0] = (x1+x0)/2. - i*dx
            if(j==0): 
                nodes[inod][1] = y0
            elif(j==ny+1): 
                nodes[inod][1] = y1
            else:
                nodes[inod][1] = y0 + j*h - h/2.
            inod=inod+1

nodes.resize((inod,3))
nNodes = int(nodes.size/3)

#Triangulation avec scipy.Delaunay
#La liste de la connectivité sera contenue dans tri.simplices
tri = Delaunay(nodes[:,:2])
nElems = len(tri.simplices)

##Ajout du Bump si nécéssaire
#db = 5. #Début du bump
#mb = 7.5 #Miieu du bump
#fb = 10. #Fin du bump
#hb = 1.  #Hauteur du bump
#for i in range(1,int(nx/2)):
#     if(nodes[i][0] > db and nodes[i][0] < mb):
#         nodes[i][2] = (nodes[i][0]-db)*hb/(mb-db)
#     elif(nodes[i][0] > mb and nodes[i][0] < fb):
#         nodes[i][2] = hb - (nodes[i][0]-mb)*hb/(fb-mb)
#     else:
#         nodes[i][2] = 0.


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

x_array = np.zeros(nNodes, dtype=np.float64)
y_array = np.zeros(nNodes, dtype=np.float64)
z_array = np.zeros(nNodes, dtype=np.float64)
for i in range(nNodes):
    x_array[i] = nodes[i][0]
    y_array[i] = nodes[i][1]
    z_array[i] = nodes[i][2]

cx = CGL.newCoordinates(Z, CK.CoordinateX_s, x_array)
cy = CGL.newCoordinates(Z, CK.CoordinateY_s, y_array)
cz = CGL.newCoordinates(Z, CK.CoordinateZ_s, z_array)

elems = np.zeros(nElems*3, np.int64)
for i in range(nElems):
    elems[3*i] = tri.simplices[i][0]+1
    elems[3*i+1] = tri.simplices[i][1]+1
    elems[3*i+2] = tri.simplices[i][2]+1
CGL.newElements(Z, "Elements", CK.TRI_3, np.array([1, nElems]), elems)

output_name = "mesh_"+str(nElems)+".cgns"
status = CGNS.MAP.save(output_name, T)
print("[+] ",output_name, " was written")
