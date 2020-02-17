#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>

using namespace std;

void lecture_txt(ifstream &file, double &tsol, vector<vector<double> > &sol){
  //Lecture des fichiers de solution sur le domaine complet
  //pour les fichier txt avec 4 colonnes

  string str;
  vector<double> v(4);

  file >> tsol;
  while(!file.eof()){
    file >> v[0] >> v[1] >> v[2] >> v[3];
    sol.push_back(v);
  }
  sol.pop_back();
}

void lecture_liens(ifstream &file, int p, vector<vector<int> > &lien){

  vector<int> v(2);

  while(!file.eof()){
    file >> v[0] >> v[1] ;
    lien[v[1]-1][0] = v[0]-1;
    lien[v[1]-1][1] = p;
  }
}

int main(int argc, char* argv[]){
  //Utilisation : ./merge_solutions nom_de_base_fichiers_a_combiner nombre_de_fichiers
  //Lancer le programme avec comme argument 1 le nom de base des fichiers solution,
  //par exemple : Mille_Iles_solution.txt 
  //si les fichiers s'appellent [0-9]_Mille_Iles_solutions.txt
  //comme argument 2 le nombre de fichiers
  //!! Attention il faut s'assurer que les fichiers de lien entre 
  //numérotation globale et locale soient présent dans le dossier ou le code est lancé.

  int nParts;
  ofstream outFile;
  string fileName;
  vector<vector<double> > sol_fin;


  if(argc < 2){
    cout << "Utilisation : ./merge_solutions nom_de_base_fichiers_a_combiner nombre_de_fichiers" << endl;
    return(0);
  }
  
  ifstream file;
  fileName = argv[1];
  nParts = atoi(argv[2]);

  int totElems = 0;
  vector<double> tsol(nParts, 0);
  vector<vector<vector<double> > > sol(nParts, vector<vector<double>>());

  stringstream fName;

  for(int p=0;p<nParts;p++){
    fName.str("");
    fName << p << "_" << fileName ;
    cout << "Lecture " << fName.str() << endl;
    file.open(fName.str());
    lecture_txt(file, tsol[p], sol[p]);
    file.close();
    totElems += sol[p].size();
  }

  cout << "Nombre total d'éléments : " << totElems << endl;
  vector<vector<int> > lien(totElems, vector<int>(2, -1));

  for(int p=0;p<nParts;p++){
    fName.str("");
    fName << p << "_liens_elems.txt";
    file.open(fName.str());
    if(!file.is_open()){
      cout << "Erreur lors de l'ouverture d'un fichier : " << fName.str() << " manquant." << endl;
      return(0);
    }
    cout << "Lecture fichier de lien " << p << endl;
    lecture_liens(file, p, lien);
    file.close();
  }

  fName.str("");
  fName << fileName;
  cout << "Ecriture du fichier final : " << fName.str() << endl;
  ofstream outfile(fileName);
  outfile << fixed ;
  outfile.precision(6);

  outfile << "    " << tsol[0] << endl;
  int i=0;
  while(lien[i][0] != -1){
    int p = lien[i][1];
    outfile << "      " << sol[p][lien[i][0]][0] << "       " << sol[p][lien[i][0]][1] << "       " << sol[p][lien[i][0]][2] << "       " << sol[p][lien[i][0]][3] << endl;
    i++;
  } 

  return(0);
}
