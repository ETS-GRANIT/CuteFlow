#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>

using namespace std;

void lecture_4col(ifstream &file, double &tsol, vector<vector<double> > &sol){
  string str;
  vector<double> v(4);

  file >> tsol;
  while(!file.eof()){
    file >> v[0] >> v[1] >> v[2] >> v[3];
    sol.push_back(v);
  }
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

  int nParts, ncol;
  string fileName, mode;

  if(argc < 4){
    cout << "Utilisation : ./merge_solutions {noeuds|elements} nom_de_base_fichiers_a_combiner nombre_de_fichiers" << endl;
    return(0);
  }

  mode = argv[1];
  fileName = argv[2];
  nParts = atoi(argv[3]);

  if((mode!="elements")and(mode!="noeuds")){
    cout << "Utilisation : ./merge_solutions {noeuds|elements} nom_de_base_fichiers_a_combiner nombre_de_fichiers" << endl;
    return(0);
  }

  ofstream outFile;
  vector<vector<double> > sol_fin;
  
  ifstream file;

  int taille = 0;
  double dum;
  vector<double> tsol(nParts, 0);
  vector<vector<vector<double> > > sol(nParts, vector<vector<double>>());

  stringstream fName;

  for(int p=0;p<nParts;p++){
    fName.str("");
    fName << p << "_" << fileName ;
    cout << "Lecture " << fName.str() << endl;
    file.open(fName.str());
    if(mode == "elements"){
      ncol=4;
      cout << "Lecture par éléments" << endl;
      lecture_4col(file, tsol[p], sol[p]);
    }
    else if(mode == "noeuds"){
      ncol=4;
      cout << "Lecture par noeuds" << endl;
      lecture_4col(file, tsol[p], sol[p]);
    }
    file.close();
    taille += sol[p].size();
  }

  
  if(mode == "elements"){
    cout << "Nombre total d'éléments : " << taille << endl;
  }
  else if(mode=="noeuds"){
    cout << "Nombre total de noeuds : " << taille << endl; 
  }

  if(taille == 0){
    cout << "Erreur de lecture des fichiers d'entrée, 0 éléments/noeuds trouvés" << endl; 
    return(0);
  }

  vector<vector<int> > lien(taille, vector<int>(2, -1));

  for(int p=0;p<nParts;p++){
    fName.str("");
    if(mode == "elements"){
      fName << p << "_liens_elems.txt";
    }
    else if(mode=="noeuds"){
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

  //Ecriture du temps de simulation
  outfile << "    " << tsol[0] << endl;

  int i=0;
  while(lien[i][0] != -1){
    int p = lien[i][1];
    if(mode == "elements"){
      for(int j=0;j<ncol;j++){
        outfile << sol[p][lien[i][0]][j] << "\t";
      }
      outfile << endl;
    }
    else if(mode == "noeuds"){
      for(int j=0;j<ncol;j++){
        outfile << sol[p][lien[i][0]][j] << "\t";
      }
      outfile << endl;
    }
    i++;
  } 
  return(0);
}
