# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#                  	       LATIN HYPERCUBE SAMPLING METHOD FOR Design Of Experiment (DOE)
#                              Procédure de calibration du coefficient de rugosité de Manning
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

# INPUTS
    # - N: Nombre d'échnatillons à générer.
    # - D: Nombre de dimensions (paramètres) à échantilloner.
    # - Range: Matrice de dimension Dx2 telle que Range(i,:)=[min_val_i, max_val_i] pour la dimension i.

# OUTPUTS
    # - Echantillons: Matrice de dimension NxD telle que chaque ligne represente un D-uplet d'échantillons

# Importation des bibliothèques
import numpy as np
from pyDOE import lhs

# Paramètres d'entrée
N = 10
D = 3 
Range = [(0, 1), (2, 2.2), (3, 3.5)]  # Plages pour chaque dimension

# Génération d'un échantillon LHS
Echantillons = lhs(D,samples=N)

# Redimensionnement des échantillons en fonction des intervalles
scaled_samples = np.zeros_like(Echantillons)

for i in range(D):
    min_val, max_val = Range[i]
    scaled_samples[:,i] = min_val + Echantillons[:, i]*(max_val-min_val)

# Exportation vers un fichier txt
output_file = 'Manning.txt'

with open(output_file, 'w') as file:
    for row in scaled_samples:
        file.write(','.join([f'{val:.4e}' for val in row]) + '\n')

# Affichage du message de fin d'exécution
print(f'Fichier {output_file} généré avec succès.')
