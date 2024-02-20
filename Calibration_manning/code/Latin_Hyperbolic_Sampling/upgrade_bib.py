import subprocess
# Liste des bibliothèques à mettre à jour 
bibliotheques_updt = ['openpyxl','numpy','pandas', 'scipy', 'matplotlib','pyCGNS', 'xlrd', 'pyDOE', 'sobol-seq']

# Mise à jour de chacune des bibliothèques
for bibliotheque in bibliotheques_updt :
    subprocess.run(['pip','install','--upgrade', bibliotheque])

# Message de vérification
print('\n Bibliothèques à jour !! \n')