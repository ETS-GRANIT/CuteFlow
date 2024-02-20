# Compute Canada Python virtual env https://docs.alliancecan.ca/wiki/Python
# Creer l'environnement vrituel d'exécution des codes
  module load gcc cgns python/3.8.10
  virtualenv ENV                                                                  #juste la première fois
  source ENV/bin/activate                                                         #Pour entrer dans l'ENV
  pip install --no-index --upgrade pip                                            #juste la première fois
  pip install openpyxl numpy pandas scipy matplotlib pyCGNS xlrd pyDOE sobol-seq  #juste la première fois
  python3 upgrade_bib.py                                                          # pour mettre à jour les bibliothèques installées
  deactivate                                                                      #Pour sortir de l'ENV


# Pour lancer le code
  python3 LHS.py
