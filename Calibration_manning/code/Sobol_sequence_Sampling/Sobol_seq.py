#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#                                 SOBOL's SEQUENCE METHOD FOR Design Of Experiment (DOE)
#                            Procédure de calibration du coefficient de rugosité de Manning
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

# INPUTS
    # - n_variables: Nombre de variables
    # - n_echantillons== 2**m_echantillons: nombres d'échantillons.
    # - Range: Matrice de dimension n_variablesx2 telle que Range(i,:)=[min_val_i, max_val_i] pour chaque variable i.
    # - cv: coefficient de variation
    # - z: score correspondant au niveau de confiance désirée (z=1.96 pour un niveau de confiance de 95%)
    # - manning: vecteur des valeurs de référence des coefficients de manning

# OUTPUTS
    # - Echantillons: Matrice de dimension n_variables * n_echantillons contenant les échantillons


# Importation des bibliothèques
from scipy.stats import qmc
import numpy as np

# Fonction de calcul des intervalles de recherche (Range) pour les nombres de Manning
def calculRange(cv: float, z: float, manning):
    I_manning = np.zeros((2,len(manning)))
    for n in range(len(manning)):
        ecart_type=cv*manning[n]
        I_manning[0,n]=manning[n]-(z*ecart_type)
        I_manning[1,n]=manning[n]+(z*ecart_type)
    return I_manning

space = calculRange(0.15, 1.96, [1, 2, 4, 8, 15])

# Configuration de l'échantillonnage QMC : Paramètres d'entrée
n_variables = len(space[0])
m_échantillons = 4    # Le nombre d'échantillons est n_echantillons = 2**m_echantillons.

# Echantillonnage
sampler = qmc.Sobol(d=n_variables, scramble=True, bits = 10, seed=None, optimization="random-cd")
sample = sampler.random_base2(m=m_échantillons)

# Définition des bornes supérieures et inférieures
l_bounds = np.zeros(n_variables)
u_bounds = np.zeros(n_variables)
for i in range(n_variables):
    l_bounds[i] = space[0][i]
    u_bounds[i] = space[1][i]

l_bounds = eval('[' + ', '.join(map(str, l_bounds)) + ']')
u_bounds = eval('[' + ', '.join(map(str, u_bounds)) + ']')

# Mise à l'échelle des échantillons
Echantillons = qmc.scale(sample, l_bounds, u_bounds)


# Exportation vers un fichier texte
Output_file = 'Manning.txt'
with open(Output_file, 'w') as fichier:
    # Écrire les noms de colonnes
#    fichier.write('\t'.join(noms_colonnes) + '\n')
    
    # Écrire les données
    for ligne in Echantillons:
        fichier.write(','.join([f'{val:.3e}' for val in ligne]) + '\n')
print(f'Fichier {Output_file} généré avec succès.')
