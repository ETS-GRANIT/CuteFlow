#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>
#include <unordered_map>
#include <map>
#include <cmath>

using namespace std;

void ecriture(bool multi_sortie, bool multi_entree, string fileName, vector<vector<long double> > &nodes, vector<vector<long double> > &elems, vector<int> &new_entreNodes, vector<int> &new_numEntreNodes, vector<int> &new_sortieNodes, vector<int> &new_numSortieNodes, int &nombreEntre, int &nombreSortie){


  int i, nNodes, nElems;
  nNodes = nodes.size();
  nElems = elems.size();
  ofstream out_mesh;
  stringstream ss;
  //ss << "out_mesh." << n << ".txt";
  ss << "Refined_" << fileName;
  out_mesh.open(ss.str());
  out_mesh << fixed ;
  out_mesh.precision(8);
  out_mesh << "Table de coordonnees" << endl;
  out_mesh << nNodes << endl;

  std::cout << "Ecriture du nouveau fichier de maillage '" << ss.str() << "' contenant " << nElems << " éléments" << std::endl;
  ss.str("");

  i=0;
  while(i<nNodes){
    out_mesh << (int) (nodes[i][0]) << " " << nodes[i][1]<< " " << nodes[i][2] << " " << nodes[i][3] <<" " << nodes[i][4] << endl ;
    i++; 
  }

  out_mesh << "Table de connectivités" << endl;
  out_mesh << nElems << endl;
  i=0;
  while(i<nElems){
    out_mesh << (int) (elems[i][0]) << " " << (int) (elems[i][1]) << " " << (int) (elems[i][2]) << " " << (int) (elems[i][3]) << " " << elems[i][4] << endl ;
    i++; 
  }

  out_mesh << "Noeuds d'entre" << endl;
  i=0;
  if(multi_entree){
    out_mesh << new_entreNodes.size() << " " << nombreEntre << endl;
    while(i<new_entreNodes.size()){
      out_mesh << new_entreNodes[i] <<  " " << new_numEntreNodes[i] << endl ;
      i++; 
    }
  }
  else{
    out_mesh << new_entreNodes.size() << endl;
    while(i<new_entreNodes.size()){
      out_mesh << new_entreNodes[i] << endl ;
      i++; 
    }
  }

  out_mesh << "Noeuds de sortie" << endl;
  i=0;
  if(multi_sortie){
    out_mesh << new_sortieNodes.size() <<  " " << nombreSortie << endl;
    while(i<new_sortieNodes.size()){
      out_mesh << new_sortieNodes[i] << " " << new_numSortieNodes[i] << endl ;
      i++; 
    }
  }
  else{
    out_mesh << new_sortieNodes.size() << endl;
    while(i<new_sortieNodes.size()){
      out_mesh << new_sortieNodes[i] << endl ;
      i++; 
    }
  }
  out_mesh.close();
}

void refine(vector<vector<long double> > &nodes, vector<vector<long double> > &elems, vector<int> &entreNodes, vector<int> &numEntreNodes, vector<int> &sortieNodes, vector<int> &numSortieNodes, vector<vector<int> > &boundTag, vector<vector<long double> > &new_nodes, vector<vector<long double> > &new_elems, vector<int> &new_entreNodes, vector<int> &new_numEntreNodes, vector<int> &new_sortieNodes, vector<int> &new_numSortieNodes, double tol){

  unsigned long long int ns = nodes.size();
  unsigned long long int s1, s2, s3;
  long double s, r, l12, l23, l31;
  bool ajout[elems.size()];
  long double s1x, s1y, s1z, s1m;
  long double s12x, s12y, s12z, s12m;
  long double s2x, s2y, s2z, s2m;
  long double s23x, s23y, s23z, s23m;
  long double s3x, s3y, s3z, s3m;
  long double s31x, s31y, s31z, s31m;
  unsigned long long int ns1, ns12, ns2, ns23, ns3, ns31;
  bool is1, is2, is3, is12, is23, is31;
  std::map<unsigned long long int, int> is_created;
  vector<long double> v(5);

  for(int i=0;i<elems.size();i++){
    //Ajout des nouveaux noeuds
    s1 = elems[i][1]-1;
    s2 = elems[i][2]-1;
    s3 = elems[i][3]-1;

    is1=(is_created.find(s1*ns+s1)==is_created.end());
    is2=(is_created.find(s2*ns+s2)==is_created.end());
    is3=(is_created.find(s3*ns+s3)==is_created.end());
    is12=((is_created.find(s1*ns+s2)==is_created.end()));
    is23=((is_created.find(s2*ns+s3)==is_created.end()));
    is31=((is_created.find(s3*ns+s1)==is_created.end()));

    if(is1){
      s1x = nodes[s1][1];
      s1y = nodes[s1][2];
      s1z = nodes[s1][3];
      s1m = nodes[s1][4];

      v[0] = new_nodes.size()+1;
      ns1=v[0]-1;
      is_created[s1*ns+s1] = ns1;
      v[1] = s1x;
      v[2] = s1y;
      v[3] = s1z;
      v[4] = s1m;
      new_nodes.push_back(v);
    }
    else{
      ns1=is_created[s1*ns+s1];
      s1x = new_nodes[ns1][1];
      s1y = new_nodes[ns1][2];
      s1z = new_nodes[ns1][3];
      s1m = new_nodes[ns1][4];
    }


    if(is2){
      s2x = nodes[s2][1];
      s2y = nodes[s2][2];
      s2z = nodes[s2][3];
      s2m = nodes[s2][4];

      v[0] = new_nodes.size()+1;
      ns2=v[0]-1;
      is_created[s2*ns+s2] = ns2;
      v[1] = s2x;
      v[2] = s2y;
      v[3] = s2z;
      v[4] = s2m;
      new_nodes.push_back(v);
    }
    else{
      ns2=is_created[s2*ns+s2];
      s2x = new_nodes[ns2][1];
      s2y = new_nodes[ns2][2];
      s2z = new_nodes[ns2][3];
      s2m = new_nodes[ns2][4];
    }

    if(is3){
      s3x = nodes[s3][1];
      s3y = nodes[s3][2];
      s3z = nodes[s3][3];
      s3m = nodes[s3][4];

      v[0] = new_nodes.size()+1;
      ns3=v[0]-1;
      is_created[s3*ns+s3] = ns3;
      v[1] = s3x;
      v[2] = s3y;
      v[3] = s3z;
      v[4] = s3m;
      new_nodes.push_back(v);
    }
    else{
      ns3=is_created[s3*ns+s3];
      s3x = new_nodes[ns3][1];
      s3y = new_nodes[ns3][2];
      s3z = new_nodes[ns3][3];
      s3m = new_nodes[ns3][4];
    }

    //Calcul du rayon du cercle inscrit pour savoir sio n ajoute ou non des elements
    l12 = sqrt(pow(s1x-s2x,2)+pow(s1y-s2y,2));
    l23 = sqrt(pow(s3x-s2x,2)+pow(s3y-s2y,2));
    l31 = sqrt(pow(s1x-s3x,2)+pow(s1y-s3y,2));
    s = (l12+l23+l31)/2.;
    r = sqrt((s-l12)*(s-l23)*(s-l31)/s);

    /* if(r<2.){ */
    if(r<tol){
      ajout[i]=false;
    }
    else{
      ajout[i]=true;
    }

    if(ajout[i]){
      if(is12){
        s12x = 0.5l*(s1x+s2x);
        s12y = 0.5l*(s1y+s2y);
        s12z = 0.5l*(s1z+s2z);
        s12m = 0.5l*(s1m+s2m);

        v[0] = new_nodes.size()+1;
        ns12=v[0]-1;
        is_created[s1*ns+s2] = ns12;
        is_created[s2*ns+s1] = ns12;
        v[1] = s12x;
        v[2] = s12y;
        v[3] = s12z;
        v[4] = s12m;
        new_nodes.push_back(v);
      }
      else{
        ns12=is_created[s1*ns+s2];
      }

      if(is23){
        s23x = 0.5l*(s2x+s3x);
        s23y = 0.5l*(s2y+s3y);
        s23z = 0.5l*(s2z+s3z);
        s23m = 0.5l*(s2m+s3m);

        v[0] = new_nodes.size()+1;
        ns23=v[0]-1;
        is_created[s2*ns+s3] = ns23;
        is_created[s3*ns+s2] = ns23;
        v[1] = s23x;
        v[2] = s23y;
        v[3] = s23z;
        v[4] = s23m;
        new_nodes.push_back(v);
      }
      else{
        ns23=is_created[s2*ns+s3];
      }

      if(is31){
        s31x = 0.5l*(s1x+s3x);
        s31y = 0.5l*(s1y+s3y);
        s31z = 0.5l*(s1z+s3z);
        s31m = 0.5l*(s1m+s3m);

        v[0] = new_nodes.size()+1;
        ns31=v[0]-1;
        is_created[s1*ns+s3] = ns31;
        is_created[s3*ns+s1] = ns31;
        v[1] = s31x;
        v[2] = s31y;
        v[3] = s31z;
        v[4] = s31m;
        new_nodes.push_back(v);
      }
      else{
        ns31=is_created[s3*ns+s1];
      }

    }

    //Ajout des nouveaux éléments
    if(ajout[i]){
      v[0] = new_elems.size()+1;
      v[1] = ns1+1;
      v[2] = ns12+1;
      v[3] = ns31+1;
      v[4] = elems[i][4];
      new_elems.push_back(v);

      v[0] = new_elems.size()+1;
      v[1] = ns12+1;
      v[2] = ns2+1;
      v[3] = ns23+1;
      v[4] = elems[i][4];
      new_elems.push_back(v);

      v[0] = new_elems.size()+1;
      v[1] = ns23+1;
      v[2] = ns3+1;
      v[3] = ns31+1;
      v[4] = elems[i][4];
      new_elems.push_back(v);

      v[0] = new_elems.size()+1;
      v[1] = ns12+1;
      v[2] = ns23+1;
      v[3] = ns31+1;
      v[4] = elems[i][4];
      new_elems.push_back(v);
    }
    /* else{ */
    /*   v[0] = new_elems.size()+1; */
    /*   v[1] = ns1+1; */
    /*   v[2] = ns2+1; */
    /*   v[3] = ns3+1; */
    /*   v[4] = elems[i][4]; */
    /*   new_elems.push_back(v); */
    /* } */

    //Gestion des entree sorties
    if(is1){
      if(boundTag[s1][0] == -1){
        new_entreNodes.push_back(ns1+1);
        new_numEntreNodes.push_back(boundTag[s1][1]);
      }
      if(boundTag[s1][0] == -2){
        new_sortieNodes.push_back(ns1+1);
        new_numSortieNodes.push_back(boundTag[s1][1]);
      }
    }

    if(is2){
      if(boundTag[s2][0] == -1){
        new_entreNodes.push_back(ns2+1);
        new_numEntreNodes.push_back(boundTag[s2][1]);
      }
      if(boundTag[s2][0] == -2){
        new_sortieNodes.push_back(ns2+1);
        new_numSortieNodes.push_back(boundTag[s2][1]);
      }
    }

    if(is3){
      if(boundTag[s3][0] == -1){
        new_entreNodes.push_back(ns3+1);
        new_numEntreNodes.push_back(boundTag[s3][1]);
      }
      if(boundTag[s3][0] == -2){
        new_sortieNodes.push_back(ns3+1);
        new_numSortieNodes.push_back(boundTag[s3][1]);
      }
    }

    if(ajout[i]){
      if(is12){
        if(boundTag[s1][0] == -1 and boundTag[s2][0] == -1){
          new_entreNodes.push_back(ns12+1);
          new_numEntreNodes.push_back(boundTag[s1][1]);
        }
        if(boundTag[s1][0] == -2 and boundTag[s2][0] == -2){
          new_sortieNodes.push_back(ns12+1);
          new_numSortieNodes.push_back(boundTag[s1][1]);
        }
      }

      if(is23){
        if(boundTag[s2][0] == -1 and boundTag[s3][0] == -1){
          new_entreNodes.push_back(ns23+1);
          new_numEntreNodes.push_back(boundTag[s2][1]);
        }
        if(boundTag[s2][0] == -2 and boundTag[s3][0] == -2){
          new_sortieNodes.push_back(ns23+1);
          new_numSortieNodes.push_back(boundTag[s2][1]);
        }
      }

      if(is31){
        if(boundTag[s3][0] == -1 and boundTag[s1][0] == -1){
          new_entreNodes.push_back(ns31+1);
          new_numEntreNodes.push_back(boundTag[s3][1]);
        }
        if(boundTag[s3][0] == -2 and boundTag[s1][0] == -2){
          new_sortieNodes.push_back(ns31+1);
          new_numSortieNodes.push_back(boundTag[s3][1]);
        }
      }
    }

  }

  for(int i=0;i<elems.size();i++){
    if(ajout[i] == false){
      //Ajout des nouveaux noeuds
      s1 = elems[i][1]-1;
      s2 = elems[i][2]-1;
      s3 = elems[i][3]-1;

      is1=(is_created.find(s1*ns+s1)==is_created.end());
      is2=(is_created.find(s2*ns+s2)==is_created.end());
      is3=(is_created.find(s3*ns+s3)==is_created.end());
      is12=((is_created.find(s1*ns+s2)==is_created.end()));
      is23=((is_created.find(s2*ns+s3)==is_created.end()));
      is31=((is_created.find(s3*ns+s1)==is_created.end()));

      if(not is1) ns1=is_created[s1*ns+s1];
      if(not is2) ns2=is_created[s2*ns+s2];
      if(not is3) ns3=is_created[s3*ns+s3];
      if(not is12) ns12=is_created[s1*ns+s2];
      if(not is23) ns23=is_created[s2*ns+s3];
      if(not is31) ns31=is_created[s3*ns+s1];

      if((is12==false and is23==false) and is31==false){
        v[0] = new_elems.size()+1;
        v[1] = ns1+1;
        v[2] = ns12+1;
        v[3] = ns31+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns12+1;
        v[2] = ns2+1;
        v[3] = ns23+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns23+1;
        v[2] = ns3+1;
        v[3] = ns31+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns12+1;
        v[2] = ns23+1;
        v[3] = ns31+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);
      }
      else if((is12==true and is23==false) and is31==false){
        v[0] = new_elems.size()+1;
        v[1] = ns2+1;
        v[2] = ns23+1;
        v[3] = ns31+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns1+1;
        v[2] = ns2+1;
        v[3] = ns31+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns3+1;
        v[2] = ns31+1;
        v[3] = ns23+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);
      }
      else if((is12==false and is23==true) and is31==false){
        v[0] = new_elems.size()+1;
        v[1] = ns1+1;
        v[2] = ns12+1;
        v[3] = ns31+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns12+1;
        v[2] = ns2+1;
        v[3] = ns31+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns31+1;
        v[2] = ns2+1;
        v[3] = ns3+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);
      }
      else if((is12==false and is23==false) and is31==true){
        v[0] = new_elems.size()+1;
        v[1] = ns2+1;
        v[2] = ns23+1;
        v[3] = ns12+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns1+1;
        v[2] = ns12+1;
        v[3] = ns23+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns1+1;
        v[2] = ns23+1;
        v[3] = ns3+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);
      }
      else if((is12==true and is23==true) and is31==false){
        v[0] = new_elems.size()+1;
        v[1] = ns1+1;
        v[2] = ns2+1;
        v[3] = ns31+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns2+1;
        v[2] = ns3+1;
        v[3] = ns31+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);
      }
      else if((is12==false and is23==true) and is31==true){
        v[0] = new_elems.size()+1;
        v[1] = ns12+1;
        v[2] = ns2+1;
        v[3] = ns3+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns12+1;
        v[2] = ns3+1;
        v[3] = ns1+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);
      }
      else if((is12==true and is23==false) and is31==true){
        v[0] = new_elems.size()+1;
        v[1] = ns1+1;
        v[2] = ns2+1;
        v[3] = ns23+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);

        v[0] = new_elems.size()+1;
        v[1] = ns1+1;
        v[2] = ns23+1;
        v[3] = ns3+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);
      }
      else if((is12==true and is23==true) and is31==true){
        v[0] = new_elems.size()+1;
        v[1] = ns1+1;
        v[2] = ns2+1;
        v[3] = ns3+1;
        v[4] = elems[i][4];
        new_elems.push_back(v);
      }

    }

  }

}

void lecture(bool multi_entree, bool multi_sortie, ifstream &mesh, vector<vector<long double> > &nodes, vector<vector<long double> > &elems, vector<int> &entreNodes, vector<int> &numEntreNodes, vector<int> &sortieNodes, vector<int> &numSortieNodes, int &nombreEntre, int &nombreSortie, vector<vector<int> > &boundTag){

  int i, nNodes, nElems, nSortie, nEntre;
  string str;
  vector<long double> v(5);

  i=0;
  getline(mesh, str);
  mesh >> nNodes;
  mesh.precision(8);
  boundTag.resize(nNodes, vector<int> (2,0));
  cout.precision(8);
  while(i<nNodes){
    v[0] = 0.;
    v[1] = 0.;
    v[2] = 0.;
    v[3] = 0.;
    v[4] = 0.;
    mesh >> v[0] >> v[1] >> v[2] >> v[3] >> v[4] ;
    nodes.push_back(v);
    i++;
  }
  i=0;
  getline(mesh, str);
  getline(mesh, str);
  mesh >> nElems;
  while(i<nElems){
    elems.push_back(v);
    mesh >> elems[i][0] >> elems[i][1] >> elems[i][2] >> elems[i][3] >> elems[i][4];
    i++;
  }
  getline(mesh, str);
  getline(mesh, str);
  if(multi_entree){
    mesh >> nEntre >> nombreEntre;
  }
  else{
    mesh >> nEntre;
    nombreEntre=1;
  }
  cout << "Lecture noeuds entree " << nEntre << endl;
  i=0;
  int dum;
  vector<int> dum_v(2);
  if(multi_entree){
    while(i<nEntre){
      mesh >> dum_v[0] >> dum_v[1];
      boundTag[dum_v[0]-1][0] = -1; //tag -1 pour entree
      boundTag[dum_v[0]-1][1] = dum_v[1]; //tag -1 pour entree
      entreNodes.push_back(dum_v[0]);
      numEntreNodes.push_back(dum_v[1]);
      i++;
    }
  }
  else{
    while(i<nEntre){
      mesh >> dum_v[0];
      dum_v[1] = 1; //unique entrée numéro 1
      boundTag[dum_v[0]-1][0] = -1; //tag -1 pour entree
      boundTag[dum_v[0]-1][1] = dum_v[1]; //tag -1 pour entree
      entreNodes.push_back(dum_v[0]);
      numEntreNodes.push_back(dum_v[1]);
      i++;
    }
  }
  getline(mesh, str);
  getline(mesh, str);
  if(multi_sortie){
    mesh >> nSortie >> nombreSortie;
  }
  else{
    mesh >> nSortie;
    nombreSortie=1;
  }
  cout << "Lecture noeuds sortie " << nSortie << endl;
  i=0;
  if(multi_sortie){
    while(i<nSortie){
      mesh >> dum_v[0] >> dum_v[1];
      boundTag[dum_v[0]-1][0] = -2; //tag -1 pour sortie
      boundTag[dum_v[0]-1][1] = dum_v[1]; //tag -1 pour entree
      sortieNodes.push_back(dum_v[0]);
      numSortieNodes.push_back(dum_v[1]);
      i++;
    }
  }
  else{
    while(i<nSortie){
      mesh >> dum_v[0];
      dum_v[1] = 1; //unique sortie numero 1
      boundTag[dum_v[0]-1][0] = -2; //tag -1 pour sortie
      boundTag[dum_v[0]-1][1] = dum_v[1]; //tag -1 pour entree
      sortieNodes.push_back(dum_v[0]);
      numSortieNodes.push_back(dum_v[1]);
      i++;
    }
  }
  cout << "Lecture elem finie, nNodes:" << nNodes << " nElems:" << nElems << endl;
}

int main(int argc, char* argv[]){

  ifstream mesh;
  string fileName;
  bool multi_entree=0;
  bool multi_sortie=0;
  double tol;

  if(argc < 4){
    cout << "Utilisation : ./refine_mesh fichier_de_maillage multi_entrees multi_sorties tol=0." << endl;
    cout << "multi_entree=1 uniquement si le maillage a découper posséde plusieurs entrées" << endl;
    cout << "multi_sortie=1 uniquement si le maillage a découper posséde plusieurs sorties" << endl;
    return(0);
  }
  else{
    fileName = argv[1];
    mesh.open(argv[1]);
    multi_entree = atoi(argv[2]);
    multi_sortie = atoi(argv[3]);
    tol = atof(argv[4]);
  }

  vector<int> entreNodes, sortieNodes, numEntreNodes, numSortieNodes;
  vector<int> new_entreNodes, new_sortieNodes, new_numEntreNodes, new_numSortieNodes;
  vector<vector<int> > boundTag;
  vector<vector<long double> > nodes, elems, new_nodes, new_elems;

  int n, i, nNodes, nElems, nombreEntre, nombreSortie;

  cout << fixed;

  lecture(multi_entree, multi_sortie, mesh, nodes, elems, entreNodes, numEntreNodes, sortieNodes, numSortieNodes, nombreEntre, nombreSortie, boundTag);

  refine(nodes, elems, entreNodes, numEntreNodes, sortieNodes, numSortieNodes, boundTag, new_nodes, new_elems, new_entreNodes, new_numEntreNodes, new_sortieNodes, new_numSortieNodes, tol);

  ecriture(multi_sortie, multi_entree, fileName, new_nodes, new_elems, new_entreNodes, new_numEntreNodes, new_sortieNodes, new_numSortieNodes, nombreEntre, nombreSortie);
}
