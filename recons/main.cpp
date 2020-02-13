#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>

using namespace std;

void lecture(ifstream &file, double &tsol, vector<vector<double> > &sol){

  string str;
  vector<double> v(4);

  file >> tsol;
  while(!file.eof()){
    file >> v[0] >> v[1] >> v[2] >> v[3];
    sol.push_back(v);
  }
  //sol.pop_back();
}

void lecture_liens(ifstream &file, vector<vector<int> > &lien){

  string str;
  vector<int> v(2);

  while(!file.eof()){
    file >> v[0] >> v[1] ;
    lien.push_back(v);
  }
  //lien.pop_back();
}
int main(int argc, char* argv[]){
  int nParts;
  ifstream file;
  ofstream outFile;
  string fileName;
  vector<vector<double> > sol;
  vector<vector<vector<double> > > sol_fin;


  if(argc < 2){
    cout << "Utilisation : ./main fichier_maillage nombre_partitions" << endl;
    return(0);
  }
  else{
    fileName = argv[1];
    file.open(argv[1]);
    nParts = atoi(argv[2]);
  }

  vector<vector<vector<int> > > liens(nParts, vector<vector<int>>());

  double tsol;
  cout << "lecture maillage" << endl;
  lecture(file, tsol, sol);
  file.close();
  stringstream fName;
  for(int p=0;p<nParts;p++){
    fName.str("");
    fName << "out_lien_elems." << p << ".txt";
    file.open(fName.str());
    cout << "lecture lien " << p << endl;
    lecture_liens(file, liens[p]);
    file.close();
  }
  
  for(int p=0;p<nParts;p++){
    fName.str("");
    fName << p << "_" << fileName;
    cout << "ecriture " << fName.str() << endl;
    outFile.open(fName.str());
    outFile << tsol << endl;
    for(int j=0;j<liens[p].size();j++){
      outFile << sol[liens[p][j][1]-1][0] << " " << sol[liens[p][j][1]-1][1] << " "<< sol[liens[p][j][1]-1][2] << " " << sol[liens[p][j][1]-1][3] << endl;
    }
    outFile.close();
  } 
  return(0);
}
