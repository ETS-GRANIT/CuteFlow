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

#Points d'interpolation
xi = np.array([292780.9505926187,293630.8979144815,294267.7279965635,294444.64941000944,296858.89325867774,298436.47734251834,299908.4017718763,301459.6199652275,302612.16334976535,303647.07659505267,304700.1783846825,301949.4462004238,301593.7315237351,303627.65504811704,304690.98004778684,307186.98859399126,308627.93489410076,295480.16815198876,296466.61433280207,297257.6731355314,300284.27222212753,303645.2641893357,308728.47998152586,310125.2328527561,307228.5928085854], dtype=np.float64)
yi = np.array([5050402.622491601,5051821.805653197,5053450.872929014,5054341.85610797,5056338.961016703,5057662.523767,5058279.15286715,5059671.05266955,5061255.28925471,5062362.791906215,5062870.9946411215,5039516.269662259,5042964.397613532,5046911.33968053,5052617.894845575,5057199.225578135,5062057.024485555,5061404.626547684,5061426.705538602,5061125.683636666,5062304.60494403,5062463.9404099425,5052470.15903622,5066696.108016082,5067603.006517198], dtype=np.float64)
n = xi.size
zi = np.array([0.037,0.037,0.037,0.04,0.05,0.05,0.05,0.02,0.01,0.01,0.01,0.03,0.03,0.03,0.03,0.02,0.02,0.05,0.05,0.01,0.01,0.01,0.02,0.01,0.01], dtype=np.float64)

#Construction d'une regression polynomiale
M = np.zeros((n, 21), dtype=np.float64)
for i in range(n):
    M[i,0] = 1.
    M[i,1] = xi[i]
    M[i,2] = yi[i]
    M[i,3] = xi[i]*yi[i]
    M[i,4] = xi[i]**2
    M[i,5] = yi[i]**2
    M[i,6] = xi[i]*yi[i]**2
    M[i,7] = yi[i]*xi[i]**2
    M[i,8] = xi[i]**3
    M[i,9] = yi[i]**3
    M[i,10] = (xi[i]**1)*(yi[i]**3)
    M[i,11] = (xi[i]**2)*(yi[i]**2)
    M[i,12] = (xi[i]**3)*(yi[i]**1)
    M[i,13] = (xi[i]**4)
    M[i,14] = (yi[i]**4)
    M[i,15] = (xi[i]**1)*(yi[i]**4)
    M[i,16] = (xi[i]**2)*(yi[i]**3)
    M[i,17] = (xi[i]**3)*(yi[i]**2)
    M[i,18] = (xi[i]**4)*(yi[i]**1)
    M[i,19] = (xi[i]**5)
    M[i,20] = (yi[i]**5)
Q, R = np.linalg.qr(M)
c = np.linalg.solve(np.matmul(np.transpose(R),R), np.dot(np.transpose(M),zi))
print(np.dot(M,c)) #Verrification

#Fonction de regression
def fman(x,y):
    z = c[0]+c[1]*x+c[2]*y+c[3]*x*y+c[4]*x**2+c[5]*y**2+c[6]*x*y**2+c[7]*y*x**2+c[8]*x**3+c[9]*y**3+c[10]*x*y**3+c[11]*(x**2)*(y**2)+c[12]*(x**3)*y+c[13]*x**4+c[14]*y**4+c[15]*x*y**4+c[16]*(x**2)*(y**3)+c[17]*(x**3)*(y**2)+c[18]*(x**4)*y+c[19]*x**5+c[20]*y**5
    return(z)


#Lecture du fichier CGNS
(tree,links,paths)=CGNS.MAP.load(argv[1]) # Load le fichier CGNS donné en argument
node=CGU.getNodeByPath(tree,"/Base/Zone") # Récupère le noeud correspondant au chemin
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
for i in range(nelems):
    s1 = conn[3*i]-1
    s2 = conn[3*i+1]-1
    s3 = conn[3*i+2]-1
    cx = (x[s1]+x[s2]+x[s3])/3.
    cy = (y[s1]+y[s2]+y[s3])/3.

    #Moyenne pondérée des nombres de Manning sur les points les plus proches
    d = np.zeros((len(xi)), dtype=np.float64)
    for k in range(len(xi)):
        d[k] = sqrt((xi[k]-cx)**2+(yi[k]-cy)**2)
    ii = np.argsort(d)
    md = 0.
    mm = 0.
    for k in range(0,20):
        if(d[ii[k]] < 1000.) : d[ii[k]] = 1000
        mm = mm + zi[ii[k]]/d[ii[k]]
        md = md + (1./d[ii[k]])
    mannings[i] = mm/md

    # Utilisation de la fonction de regression
    # mannings[i] = fman(cx,cy)

    #Correction potentielle des nombres de mannings s'ils sont trop grands ou trop petits
    if(mannings[i] > 0.05 or mannings[i] < 0.): nout+=1
    if(mannings[i] > 0.05) : mannings[i] = 0.05
    if(mannings[i] < 0.) : mannings[i] = 0.01

print(nout,"values out of range 0.00 0.05")
man = CGL.newUserDefinedData(node, "Manning")
CGL.newGridLocation(man, CK.CellCenter_s)
CGL.newDataArray(man, '{DataArray}', value=mannings)
status = CGNS.MAP.save('manning_'+argv[1],tree) #Sauvegarde l'arbre dans un nouveau fichier

#Ecriture du fichier de maillage avec les nombre de manning pour visualisation avec Paraview
f = open("mannings.vtk","w")
f.write("# vtk DataFile Version 2.0\n")
f.write("Manning\n")
f.write("ASCII\n")
f.write("DATASET UNSTRUCTURED_GRID\n")
f.write("POINTS "+str(nnodes)+" double\n")
for i in range(nnodes):
    f.write(str(x[i])+ " " +str(y[i]) + " " + str(z[i]) + "\n")
f.write("CELLS " + str(nelems) + " " + str(nelems*4) +"\n")
for i in range(nelems):
    f.write("3 " + str(conn[3*i]-1)+ " " +str(conn[3*i+1]-1) + " " + str(conn[3*i+2]-1) + "\n")
f.write("CELL_TYPES " + str(nelems) +"\n")
for i in range(nelems):
    f.write("5\n")
f.write("CELL_DATA " + str(nelems) +"\n")
f.write("SCALARS Manning double\n")
f.write("LOOKUP_TABLE default\n")
for i in range(nelems):
    f.write(str(mannings[i]) + "\n")
