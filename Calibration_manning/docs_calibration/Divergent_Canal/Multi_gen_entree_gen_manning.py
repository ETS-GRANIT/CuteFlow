'''Script de création des fichiers d'entrée pour générer les Manning
==========================================================================
Doit être exécuté après le code Zonage.m
 INPUTS :
  * Liste des dossiers :               Case_%d
  * Liste des fichiers de zonage :     zonage_case_%d
 OUTPUTS :
  * Fichier d'entrée du script gen_manning.py pour dans chaque dossier
                                       Entree_gen_manning_%d
========================================================================='''

import os
import pandas as pd

# Chemin du répertoire de travail
directory_path = './'

# Liste des dossiers à accéder avec le modèle 'case_*'
liste_dossiers = [nom for nom in os.listdir(directory_path) if os.path.isdir(os.path.join(directory_path, nom)) and nom.startswith('Case_')]

# Parcourir chacun des dossiers
for i, nom_dossier in enumerate(liste_dossiers, start=1):
    chemin_acces = os.path.join(directory_path, nom_dossier)

    # Ouverture du fichier Excel
    file_path = os.path.join(chemin_acces,f'zonage_{nom_dossier}.xlsx')

    # Charger les données à partir du fichier Excel
    data = pd.read_excel(file_path,engine='openpyxl', header=None)

    # Extraire les colonnes de données (CellIDS et Manning)
    cellIDs = data.iloc[:,1]
    manning = data.iloc[:,2]
    manningValues = [float(value) for value in manning[1:]]

    # Écrire les données dans un fichier CSV
    outputFilename = os.path.join(chemin_acces, f'Entree_gen_manning_{nom_dossier.split("_")[-1]}.txt')
    with open(outputFilename, 'w') as fid:
        # Écriture des IDs des cellules 
        fid.write('Cells IDs\n')
        fid.write(','.join(map(str, cellIDs[1:-1])))
        fid.write(f',{cellIDs.iloc[-1]}\n')

        # Écriture des valeurs de Manning
        fid.write('Manning_values\n')
        fid.write(','.join([f'{x:.3f}' for x in manningValues[:-1]]))
        fid.write(f',{manningValues[-1]:.3f}\n')

        print(f'\nFichier {file_path} traité et enregistré sous {outputFilename}.')
print('\nFin du programme!!\n')
