#!/usr/bin/bash

let i=0
while read -r line;do
  # if pour sauter la première ligne qui devrait contenir le nombre de cas a traiter
  if [ $i -ge 1 ];then

    #Lecture du fichier passé en parametre
    h1=`echo $line | cut -d " " -f 1`
    h2=`echo $line | cut -d " " -f 2`
    h3=`echo $line | cut -d " " -f 3`
    h4=`echo $line | cut -d " " -f 4`
    h5=`echo $line | cut -d " " -f 5`
    h6=`echo $line | cut -d " " -f 6`
    h7=`echo $line | cut -d " " -f 7`
    h8=`echo $line | cut -d " " -f 8`
    h9=`echo $line | cut -d " " -f 9`
    manning=`echo $line | cut -d " " -f 10`

    #Affichage des parametres lus dans le fichier pour le cas $i
    echo "Cas $i $h1 $h2 $h3 $h4 $h5 $h6 $h7 $h8 $h9 $manning"

    #Création du dossier multi_$i si il n'existe pas déjà
    if [ ! -d multi_$i ]; then 
      mkdir multi_$i
    fi

    #On se déplace dans le dossier multi_î 
    cd multi_$i

    #Copie des fichiers dans base_files
    # cp ../base_files/* .

    #Utilisation du script gen_donnees pour générer le fichier
    #de données pour le cas actuel
    ../gen_donnees2.sh $h1 $h2 $h3 $h4 $h5 $h6 $h7 $h8 $h9 $manning

    #Clean du dossier
    make clean

    #On sort du dossier multi_$i
    cd ..
  else
    #la premiere ligne contient le nombre de cas a traiter
    #ça ne nous sert pas ici
    nb=$line
  fi
  let i=$i+1
done < $1
