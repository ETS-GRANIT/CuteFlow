#!/usr/bin/bash

let i=0
while read -r line;do
  # if pour sauter la première ligne qui devrait contenir le nombre de cas a traiter
  if [ $i -ge 1 ];then

    #Lecture du fichier passé en parametre
    debit=`echo $line | cut -d " " -f 1`
    hamont=`echo $line | cut -d " " -f 2`
    haval=`echo $line | cut -d " " -f 3`
    maning=`echo $line | cut -d " " -f 4`

    #Affichage des parametres lus dans le fichier pour le cas $i
    echo "Cas $i $debit $hamont $haval $maning"

    #Création du dossier multi_$i si il n'existe pas déjà
    if [ ! -d multi_$i ]; then 
      mkdir multi_$i
    fi

    #On se déplace dans le dossier multi_î 
    cd multi_$i

    #Copie des fichiers dans base_files
    cp ../base_files/* .

    #Utilisation du script gen_donnees pour générer le fichier
    #de donnée pour le cas actuel
    ../gen_donnees.sh $debit $hamont $haval $maning

    #On sort du dossier multi_$i
    cd ..
  fi
  let i=$i+1
done < $1
