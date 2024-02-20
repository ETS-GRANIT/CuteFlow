#!/usr/bin/bash

# Vérifier si le nombre d'arguments est correct
if [ $# -ne 2 ]; then
    echo "Usage: ./Multi.sh  <Manning.txt> <initialisation.dat>"
    exit 1
fi

# Récupérer les noms des fichiers d'entrée depuis les arguments
manning_file="$1"
init_file="$2"

# Lecture du fichier d'initialisation passé en paramètre
let k=0
while read -r line;do
    let l=k+1
    # Extraction des valeurs 
    debit=`echo $line | cut -d " " -f 1`
    hamont=`echo $line | cut -d " " -f 2`
    haval=`echo $line | cut -d " " -f 3`
    let k=$k+1
done < "$init_file" 

# Lecture du fichier de manning passé en paramètre
let i=0
while read -r line;do
    let j=i+1
    #Lecture du fichier passé en paramètre
    average_manning=`echo $line | rev | cut -d "," -f 1 | rev`

    #Affichage des paramètres lus dans le fichier pour le cas $i
    echo "Case_$j $average_manning"

    # On se déplace dans le dossier Cas_$i
    cd Case_$j

    # Copie de l'executable dans le dossier courant 
    cp ../base_files/cuteflow .

    # Utilisation du script gen_manning.sh pour générer maillage pour le cas actuel
    ../gen_manning.sh $average_manning $j

    # Utilisation du script gen_donnees.sh pour générer le fichier de données pour 
    # le cas actuel

    ../gen_donnees.sh $debit $hamont $haval $j

    # On sort du dossier cas_$i
    cd ..

    let i=$i+1
done < "$manning_file" 
