#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>

using namespace std;

void gnuplot_entree_sorties(vector<vector<double> > &nodes, int nombre_entrees, vector<int> &entreNodes, vector<int> &numEntreNodes, vector<int> &sortieNodes){

  int i=0, entre=0;
  stringstream ss;
  ofstream out;
  i=0;
  for(int entre=0;entre<nombre_entrees;entre++){
    ss.str("");
    ss << "entre_" << entre << ".txt";
    out.open(ss.str());
    while(entre+1==numEntreNodes[i]){
      out << nodes[entreNodes[i]-1][0] << " " << nodes[entreNodes[i]-1][1] << endl;
      i++;
    }
    out.close();
  }

  out.open("sortie.txt");
  for(int i=0;i<sortieNodes.size();i++){
    out << nodes[sortieNodes[i]-1][0] << " " << nodes[sortieNodes[i]-1][1] << endl;
  }
  out.close();

  
}

void set_manning(int nNodes, vector<double> &manning){

  int i=0;
  manning.resize(nNodes);
  while(i<nNodes){
    manning[i] = 0.0200 ;
    i++;
  }

}

void ecriture(ofstream &out, vector<vector<double> > &nodes, vector<vector<int> > &elems, vector<double> &manning, vector<int> &entreNodes, vector<int> &numEntreNodes, vector<int> &sortieNodes, int nombre_entrees, int nombre_sorties){

  int i=0;
  double man_moy;
  cout.precision(4);
  out << fixed;
  out << "Table de coordonnes" << endl;
  out << nodes.size() << endl;
  while(i<nodes.size()){
    out << i+1 << " " << nodes[i][0] << " " << nodes[i][1] << " " << nodes[i][2] <<  " " << manning[i] << endl; 
    i++;
  }

  out << "Table de coonectivite" << endl;
  out << elems.size() << endl;
  i=0;
  while(i<elems.size()){
    man_moy = (manning[elems[i][1]-1]+manning[elems[i][2]-1]+manning[elems[i][3]-1])/3.;
    out << i+1 << " " << elems[i][1] << " " << elems[i][2] << " " << elems[i][3] <<  " " << man_moy << endl; 
    i++;
  }

  out << "Noeuds entree" << endl;
  out << entreNodes.size() << " " << nombre_entrees << endl;
  i=0;
  while(i<entreNodes.size()){
    out << entreNodes[i] << " " << numEntreNodes[i] << endl;
    i++;
  }
  out << "Noeuds sortie" << endl;
  i=0;
  out << sortieNodes.size() << endl;
  while(i<sortieNodes.size()){
    out << sortieNodes[i] << endl;
    i++;
  }
  out << "Noeuds de wall" << endl;
  out << "0"<< endl;
}

void lecture_slc_2(ifstream &slc,vector<int> &entreNodes, vector<int> &numEntreNodes, vector<int> &sortieNodes){
  int n, i(0), j(0);
  double dum;
  string str;

  slc >> n ;
  j=0;
  while(j<14){
    getline(slc, str, '$');
    getline(slc, str, '$');
    i=0;
    while(i<10){
      slc >> n;
      sortieNodes.push_back(n);
      i++;
    }
    j++;
  }

  j=0;
  while(j<11){
    getline(slc, str, '$');
    getline(slc, str, '$');
    i=0;
    while(i<10){
      slc >> n;
      entreNodes.push_back(n);
      numEntreNodes.push_back(1);
      i++;
    }
    j++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<8){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(1);
    i++;
  }
}
void lecture_slc(ifstream &slc,vector<int> &entreNodes, vector<int> &numEntreNodes, vector<int> &sortieNodes){
  int n, i(0), j(0);
  double dum;
  string str;

  slc >> n ;
  j=0;
  while(j<11){
    getline(slc, str, '$');
    getline(slc, str, '$');
    i=0;
    while(i<10){
      slc >> n;
      sortieNodes.push_back(n);
      i++;
    }
    j++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  slc >> n;
  sortieNodes.push_back(n);

  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<7){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(1);
    i++;
  }

  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<10){
    slc >> n;
    entreNodes.push_back(n);
    /* cout << n+1 << endl; */
    numEntreNodes.push_back(2);
    i++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<5){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(2);
    i++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<10){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(3);
    i++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<10){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(3);
    i++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<5){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(3);
    i++;
  }

  j=0;
  while(j<6){
    getline(slc, str, '$');
    getline(slc, str, '$');
    i=0;
    while(i<10){
      slc >> n;
      entreNodes.push_back(n);
      numEntreNodes.push_back(4);
      i++;
    }
    j++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<4){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(4);
    i++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<10){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(5);
    i++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<3){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(5);
    i++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<9){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(6);
    i++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<10){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(7);
    i++;
  }
  getline(slc, str, '$');
  getline(slc, str, '$');
  i=0;
  while(i<2){
    slc >> n;
    entreNodes.push_back(n);
    numEntreNodes.push_back(7);
    i++;
  }
}

void lecture_mesh(ifstream &mesh, vector<vector<double> > &nodes, vector<vector<int> > &elems){

  int i, nNodes, nElems;
  string str;
  vector<double> v(3);
  vector<int> w(4);

  i=0;
  //Removing header
  while(i<5){
    getline(mesh, str);
    i++;
  }
  getline(mesh, str, ']');
  getline(mesh, str, '\n');

  stringstream ss(str);
  ss >> nNodes;
  
  ss.str(" ");
  getline(mesh, str, ']');
  getline(mesh, str, '\n');
  ss.str(str);
  ss >> nElems;

  i=0;
  while(i<19){
    getline(mesh, str);
    i++;
  }

  cout.precision(4);
  i=0;
  while(i<nNodes){
    v[0] = 0.;
    v[1] = 0.;
    v[2] = 0.;
    mesh >> v[0] >> v[1] >> v[2];
    nodes.push_back(v);
    i++;
  }

  i=0;
  while(i<6){
    getline(mesh, str);
    i++;
  }

  i=0;
  while(i<nElems){
    w[0] = 0;
    w[1] = 0;
    w[2] = 0;
    w[3] = 0;
    mesh >> w[0] >> w[1] >> w[2] >> w[3];
    w[0] += 1;
    w[1] += 1;
    w[2] += 1;
    w[3] += 1;
    elems.push_back(w);
    i++;
  }
  cout << "Lecture elem finie, nNodes:" << nNodes << " nElems:" << nElems << endl;
}

int main(int argc, char* argv[]){

  int nombre_entree, nombre_sortie;
  vector<vector<double> > nodes;
  vector<vector<int> > elems;
  vector<double> manning, debits, hauteurs;
  vector<int> entreNodes, sortieNodes, numEntreNodes;

  cout << fixed;

  if(argc < 5){
    cout << "Utilisation : ./split_mesh fichier.mesh fichier.slc nombre_entree nombre_sortie" << endl;
    return(0);
  }

  nombre_entree = atoi(argv[3]);
  nombre_sortie = atoi(argv[4]);
  ifstream slc(argv[2]);
  //lecture_slc_2(slc, entreNodes, numEntreNodes, sortieNodes);
  lecture_slc(slc, entreNodes, numEntreNodes, sortieNodes);
  slc.close();

  //Lecture du maillage
  ifstream mesh(argv[1]);
  lecture_mesh(mesh, nodes, elems);
  mesh.close();

  /* ifstream man.open(argv[1]); */
  /* lecture_man(man, manning); */
  /* man.close() */

  set_manning(nodes.size(), manning);

  /* gnuplot_entree_sorties(nodes, nombre_entree, entreNodes, numEntreNodes, sortieNodes); */

  ofstream out("output_mesh.txt");
  ecriture(out, nodes, elems, manning, entreNodes, numEntreNodes, sortieNodes, nombre_entree, nombre_sortie);
  out.close();
  return(0);
}
