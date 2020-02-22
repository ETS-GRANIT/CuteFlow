#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>

using namespace std;

void lecture_bluekenue(ifstream &file, int &nNodes, int &nElems, vector<vector<double> > &nodes, vector<vector<int> > &elems){
  string str;
  vector<double> v(3);
  vector<int> w(3);
  int nNodes_loc, nElems_loc;

  file >> str >> nNodes_loc;
  file >> str >> nElems_loc;
  nNodes += nNodes_loc;
  nElems += nElems_loc;
  file >> str >> str;
  file >> str;
  for(int i=0;i<nNodes_loc;i++){
    file >> v[0] >> v[1] >> v[2] ;
    nodes.push_back(v);
  }
  for(int i=0;i<nElems_loc;i++){
    file >> w[0] >> w[1] >> w[2] ;
    w[0]-=1;
    w[1]-=1;
    w[2]-=1;
    elems.push_back(w);
  }

}

void lecture_liens(ifstream &file, int p, int &taille, vector<vector<int> > &lien){

  vector<int> v(2);
  while(!file.eof()){
    file >> v[0] >> v[1] ;
    lien[v[1]-1][0] = v[0]-1;
    lien[v[1]-1][1] = p;
    if(taille < v[1]-1){
      taille = v[1] ;
    } 
  }
}

void lecture_liens_reverse(ifstream &file, int p, vector<vector<int> > &lien){

  vector<int> v(2);
  while(!file.eof()){
    file >> v[0] >> v[1] ;
    lien[v[0]-1][0] = v[1]-1;
    lien[v[0]-1][1] = p;
  }
}

int main(int argc, char* argv[]){

  int nParts=0, nElems=0, nNodes=0;
  int nElemsTrue=0, nNodesTrue=0;
  string fileName, mode;

  if(argc < 3){
    cout << "Utilisation : ./merge_bluekenue nom_de_base_fichiers_a_combiner nombre_de_fichiers" << endl;
    return(0);
  }

  fileName = argv[1];
  nParts = atoi(argv[2]);

  ofstream outFile;
  
  ifstream file;

  vector<vector<vector<double> > > nodes(nParts, vector<vector<double>>());
  vector<vector<vector<int> > > elems(nParts, vector<vector<int>>());

  stringstream fName;

  for(int p=0;p<nParts;p++){
    fName.str("");
    fName << p << "_" << fileName ;
    cout << "Lecture " << fName.str() << endl;
    file.open(fName.str());
    lecture_bluekenue(file, nNodes, nElems, nodes[p], elems[p]);
    file.close();
  }
  
  cout << "Nombre total d'éléments : " << nElems << endl;
  cout << "Nombre total de noeuds : " << nNodes << endl; 

  if((nElems==0)or(nNodes==0)){
    cout << "Erreur de lecture des fichiers d'entrée, 0 éléments/noeuds trouvés" << endl; 
    return(0);
  }

  vector<vector<int> > lienElems(nElems, vector<int>(2, -1));
  vector<vector<int> > lienNodes(nNodes, vector<int>(2, -1));
  vector<vector<vector<int> > > lienNodesReverse(nParts, vector<vector<int> >(nNodes, vector<int>(2, -1)));

  for(int p=0;p<nParts;p++){
    fName.str("");
    fName << p << "_liens_elems.txt";
    file.open(fName.str());
    if(!file.is_open()){
      cout << "Erreur lors de l'ouverture d'un fichier : " << fName.str() << " manquant." << endl;
      return(0);
    }
    cout << "Lecture fichier de lien " << p << endl;
    lecture_liens(file, p, nElemsTrue, lienElems);
    file.close();

    fName.str("");
    fName << p << "_liens_nodes.txt";
    file.open(fName.str());
    if(!file.is_open()){
      cout << "Erreur lors de l'ouverture d'un fichier : " << fName.str() << " manquant." << endl;
      return(0);
    }
    cout << "Lecture fichier de lien " << p << endl;
    lecture_liens(file, p, nNodesTrue, lienNodes);
    file.close();
    file.open(fName.str());
    lecture_liens_reverse(file, p, lienNodesReverse[p]);
    file.close();
  }

  fName.str("");
  fName << fileName;
  cout << "Ecriture du fichier final : " << fName.str() << endl;
  ofstream outfile(fileName);
  outfile << fixed ;
  outfile.precision(6);

  outfile << ":NodeCount " << nNodesTrue << endl;
  outfile << ":ElementCount " << nElemsTrue << endl;
  outfile << ":ElementType T3 " << endl;
  outfile << ":EndHeader" << endl;

  for(int i=0;i<nNodesTrue;i++){
    int p = lienNodes[i][1];
    outfile << nodes[p][lienNodes[i][0]][0] << " " << nodes[p][lienNodes[i][0]][1] << " " << nodes[p][lienNodes[i][0]][2] << endl;
  }


  for(int i=0;i<nElemsTrue;i++){
    int p = lienElems[i][1];
    outfile << lienNodesReverse[p][elems[p][lienElems[i][0]][0]][0]+1 << " " << lienNodesReverse[p][elems[p][lienElems[i][0]][1]][0]+1 << " " << lienNodesReverse[p][elems[p][lienElems[i][0]][2]][0]+1 << endl;
  }


  outfile.close();
  return(0);
}
