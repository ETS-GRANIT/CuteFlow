#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>

using namespace std;

void lecture_restart(ifstream &file, double &tsol, vector<vector<double> > &sol){
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

void lecture_3d(ifstream &file, vector<vector<double> > &sol){
  //Lecture des fichiers de solution sur le domaine complet
  //pour les fichier txt avec 8 colonnes

  string str;
  vector<double> v(8);

  while(!file.eof()){
    file >> v[0] >> v[1] >> v[2] >> v[3] >> v[4] >> v[5] >> v[6] >> v[7];
    sol.push_back(v);
  }
  sol.pop_back();
}

void lecture_2d(ifstream &file, vector<vector<double> > &sol){
  //Lecture des fichiers de solution sur le domaine complet
  //pour les fichier txt avec 6 colonnes

  string str;
  vector<double> v(6);

  while(!file.eof()){
    file >> v[0] >> v[1] >> v[2] >> v[3] >> v[4] >> v[5] ;
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
  //Utilisation : ./merge_solutions {restart|sol2d|sol3d} nom_de_base_fichiers_a_combiner nombre_de_fichiers
  //Lancer le programme avec comme argument 1 le type de fichier a reconstruire
  // en argument 2 le nom de base des fichiers solution,
  //par exemple, Mille_Iles_solution.txt
  //si les fichiers s'appellent [0-9]_Mille_Iles_solutions.txt
  //comme argument 3 le nombre de fichiers
  //Exemple pour reconstruire les fichiers de restart à partir de 
  //0_Mille_Iles_solution_initiale_t67500.txt et 1_Mille_Iles_solution_initiale_t67500.txt :
  //./merge_solutions restart Mille_Iles_solution_initiale_t67500.txt 2
  //!! Attention il faut s'assurer que les fichiers de lien entre 
  //numérotation globale et locale soient présent dans le dossier ou le code est lancé.

  int nParts;
  string fileName, mode;

  if(argc < 4){
    cout << "Utilisation : ./merge_solutions {restart|sol2d|sol3d} nom_de_base_fichiers_a_combiner nombre_de_fichiers" << endl;
    return(0);
  }

  mode = argv[1];
  fileName = argv[2];
  nParts = atoi(argv[3]);

  if((mode!="restart")and(mode!="sol2d")and(mode!="sol3d")){
    cout << "Utilisation : ./merge_solutions {restart|sol2d|sol3d} nom_de_base_fichiers_a_combiner nombre_de_fichiers" << endl;
    return(0);
  }

  ofstream outFile;
  vector<vector<double> > sol_fin;

  
  ifstream file;

  int taille = 0;
  vector<double> tsol(nParts, 0);
  vector<vector<vector<double> > > sol(nParts, vector<vector<double>>());

  stringstream fName;

  for(int p=0;p<nParts;p++){
    fName.str("");
    fName << p << "_" << fileName ;
    cout << "Lecture " << fName.str() << endl;
    file.open(fName.str());
    if(mode == "restart"){
      lecture_restart(file, tsol[p], sol[p]);
    }
    else if(mode == "sol2d"){
      lecture_3d(file, sol[p]);
    }
    else if(mode == "sol3d"){
      lecture_2d(file, sol[p]);
    }
    file.close();
    taille += sol[p].size();
  }

  
  if(mode == "restart"){
    cout << "Nombre total d'éléments : " << taille << endl;
  }
  else{
    cout << "Nombre total de noeuds : " << taille << endl; 
  }

  if(taille == 0){
    cout << "Erreur de lecture des fichiers d'entrée, 0 éléments/noeuds trouvés" << endl; 
    return(0);
  }

  vector<vector<int> > lien(taille, vector<int>(2, -1));

  for(int p=0;p<nParts;p++){
    fName.str("");
    if(mode == "restart"){
      fName << p << "_liens_elems.txt";
    }
    else{
      fName << p << "_liens_nodes.txt";
    }
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

  if(mode == "restart"){
    outfile << "    " << tsol[0] << endl;
  }
  int i=0;
  while(lien[i][0] != -1){
    int p = lien[i][1];
    if(mode == "restart"){
      outfile << "      " << sol[p][lien[i][0]][0] << "       " << sol[p][lien[i][0]][1] << "       " << sol[p][lien[i][0]][2] << "       " << sol[p][lien[i][0]][3] << endl;
    }
    else if(mode == "sol2d"){
      outfile << "      " << sol[p][lien[i][0]][1] << "       " << sol[p][lien[i][0]][2] << "       " << sol[p][lien[i][0]][3]<< "      " << sol[p][lien[i][0]][4] << "       " << sol[p][lien[i][0]][5] << endl;
    }
    else if(mode == "sol3d"){
      outfile << "      " << (int) sol[p][lien[i][0]][0] << "       " << sol[p][lien[i][0]][1] << "       " << sol[p][lien[i][0]][2] << "       " << sol[p][lien[i][0]][3]<< "      " << sol[p][lien[i][0]][4] << "       " << sol[p][lien[i][0]][5] << "       " << sol[p][lien[i][0]][6] << "       " << sol[p][lien[i][0]][7] << endl;
    }
    i++;
  } 


  return(0);
}
