#!/bin/bash

for i in `seq 0 $1`;do
echo "&DATAMESH  ALEAT=1 , MAIL=2 , MTRI=1 , NNX=153 , NNY=8 , meshfile='${i}_Mille_Iles_mesh_743968_elts.txt' , 
           elt_bound=0, numerot=1 , zcolumn=1,
	   ncel=0,
           elts=140,  911, /" >> "${i}_donneemai_mille_iles.txt"
done

