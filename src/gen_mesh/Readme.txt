#Compute Canada Python virtual env https://docs.alliancecan.ca/wiki/Python
#Creer l'env virtuel
module load gcc cgns python/3.8.10
virtualenv ENV #juste la première fois
source ENV/bin/activate #Pour entrer dans l'ENV
pip install --no-index --upgrade pip #juste la première fois
pip install numpy scipy matplotlib pyCGNS #juste la première fois
deactivate #Pour sortir de l'ENV

#Pour lancer le code
python3 gen_mesh.py
