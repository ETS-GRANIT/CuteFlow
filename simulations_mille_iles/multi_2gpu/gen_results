#!/usr/bin/bash

if [ $# -le 1 ]; then
  echo "Utilisation: ./gen_results INPUT_MONTE_CARLO.dat nombre_gpu_par_simul"
  echo "Exemple: ./gen_results INPUTE_MONTE_CARLO.dat 2"
  echo "Pour générer un fichier solution à partir de 10 cas tests chacun ayant utilisé 2GPU,"
  exit
fi

if [ ! -d results ]; then
  mkdir results
else
  if [ $(ls results/ | wc -l) -ge 1 ]; then
    echo "[*] Attention les fichiers dans le dossier results vont être éffacés "
    rm results/*
  fi
fi

let ncas=$(wc -l $1 | cut -d " " -f 1)-1
cd results
let ngpu=$2
for i in `seq 1 $ncas`;do
  echo "[*] Traitement du cas $i"
  cd ../multi_$i
  if [ -f 0_SOLUTION_FINALE_PAR_ELEMENT.txt ] && [ $(wc -l 0_SOLUTION_FINALE_PAR_ELEMENT.txt | cut -d " " -f 1) -ge 1 ]; then
    echo "[+] Cas $i, Fichiers restart trouvés"
    echo "[*] ../base_files/merge_solutions restart SOLUTION_FINALE_PAR_ELEMENT.txt $ngpu"
    ../base_files/merge_solutions restart SOLUTION_FINALE_PAR_ELEMENT.txt $ngpu
  else
    echo "[-] Cas $i, Aucun fichier de restart trouvé !!"
  fi

  if [ -f 0_SOL_FV_MULTISIM_2D.txt ] && [ $(wc -l 0_SOL_FV_MULTISIM_2D.txt | cut -d " " -f 1) -ge 1 ]; then
    echo "[+] Cas $i, Fichiers sol2d trouvés"
    echo "[*] ../base_files/merge_solutions sol2d SOL_FV_MULTISIM_3D.txt $ngpu"
    ../base_files/merge_solutions sol2d SOL_FV_MULTISIM_2D.txt $ngpu
  else
    echo "[-] Cas $i, Aucun fichier de solution 2D trouvé !!"
  fi

  if [ -f 0_SOL_FV_MULTISIM_3D.txt ] && [ $(wc -l 0_SOL_FV_MULTISIM_3D.txt | cut -d " " -f 1) -ge 1 ]; then
    echo "[+] Cas $i, Fichiers sol3d trouvés"
    echo "[*] ../base_files/merge_solutions sol3d SOL_FV_MULTISIM_3D.txt $ngpu"
    ../base_files/merge_solutions sol3d SOL_FV_MULTISIM_3D.txt $ngpu
    cat SOL_FV_MULTISIM_3D.txt >> ../results/GLOBAL_SOL_FV_MULTISIM_3D.txt
    let j=i+1
    sed -n ${j}p ../$1 >> ../results/INPUT_MONTE_CARLO.tmp
  else
    echo "[-] Cas $i, Aucun fichier de solution 3D trouvé !!"
  fi
  cd ../results
done

echo $(wc -l INPUT_MONTE_CARLO.tmp | cut -d " " -f 1) > INPUT_MONTE_CARLO.dat
cat INPUT_MONTE_CARLO.tmp >> INPUT_MONTE_CARLO.dat
rm INPUT_MONTE_CARLO.tmp
