#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>

using namespace std;

void lecture(ifstream &file, double &tsol, vector<vector<double> > &sol){
  //Lecture du fichier de solution sur domaine complet

  string str;
  vector<double> v(4);

  file >> tsol;
  while(!file.eof()){
    file >> v[0] >> v[1] >> v[2] >> v[3];
    sol.push_back(v);
  }
  sol.pop_back();
}

void lecture_liens(ifstream &file, vector<vector<int> > &lien){
  //Lecture de fichiers de correspondance entre numérotation locale
  // dans les sous domaines et la numérotation globales
  // ces fichiers sont générés lors de la découpe du maillage

  string str;
  vector<int> v(2);

  while(!file.eof()){
    file >> v[0] >> v[1] ;
    lien.push_back(v);
  }
  lien.pop_back();
}

int main(int argc, char* argv[]){
  //Utilisation : ./split fichier_sur_domaine_entier nombre_partitions
  //Lancer le programme avec comme argument 1 le fichier à partitionner,
  //comme argument 2 le nombre de partitions voulues
  //!! Attention il faut s'assurer que les fichiers de lien entre 
  //numérotation globale et locale soient présent dans le dossier ou le code est lancé.

  int nParts;
  ifstream file;
  ofstream outFile;
  string fileName;
  vector<vector<double> > sol;
  vector<vector<vector<double> > > sol_fin;


  if(argc <= 2){
    cout << "Utilisation : ./split fichier_sur_domaine_entier nombre_partitions" << endl;
    return(0);
  }
  else{
    fileName = argv[1];
    file.open(argv[1]);
    nParts = atoi(argv[2]);
  }

  vector<vector<vector<int> > > liens(nParts, vector<vector<int>>());

  double tsol;
  cout << "Lecture " << fileName << endl;
  lecture(file, tsol, sol);
  file.close();
  stringstream fName;
  for(int p=0;p<nParts;p++){
    fName.str("");
    fName << p << "_liens_elems.txt";
    file.open(fName.str());
    if(!file.is_open()){
      cout << "Erreur lors de l'ouverture d'un fichier : " << fName.str() << " manquant." << endl;
      return(0);
    }
    cout << "Lecture fichier de lien " << p << endl;
    lecture_liens(file, liens[p]);
    file.close();
  }
  
  for(int p=0;p<nParts;p++){
    fName.str("");
    fName << p << "_" << fileName;
    cout << "Ecriture fichier de sortie " << fName.str() << endl;
    outFile.open(fName.str());
    outFile << fixed ;
    outFile.precision(8);
    outFile << tsol << endl;
    for(int j=0;j<liens[p].size();j++){
      outFile << sol[liens[p][j][1]-1][0] << " " << sol[liens[p][j][1]-1][1] << " "<< sol[liens[p][j][1]-1][2] << " " << sol[liens[p][j][1]-1][3] << endl;
    }
    outFile.close();
  } 
  return(0);
}
