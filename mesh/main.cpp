#include "metis.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>


using namespace std;


void lecture(ifstream &mesh, vector<vector<double> > &nodes, vector<vector<double> > &elems,idx_t *&eind, idx_t *&eptr, vector<int> &entreNodes, vector<int> &sortieNodes, vector<int> &wallNodes){

  int i, nNodes, nElems, nSortie, nEntre, nWall;
  string str;
  vector<double> v(5);

  i=0;
  getline(mesh, str);
  mesh >> nNodes;
  cout.precision(4);
  //cout << fixed;
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
  eind = new idx_t[3*nElems];
  eptr = new idx_t[nElems+1];
  while(i<nElems){
    elems.push_back(v);
    mesh >> elems[i][0] >> elems[i][1] >> elems[i][2] >> elems[i][3] >> elems[i][4];
    eind[3*i] = elems[i][1];
    eind[3*i+1] = elems[i][2];
    eind[3*i+2] = elems[i][3];
    eptr[i] = i*3;
    i++;
  }
  getline(mesh, str);
  getline(mesh, str);
  mesh >> nEntre;
  cout << "Lecture noeuds entree " << nEntre << endl;
  i=0;
  int dum;
  while(i<nEntre){
    mesh >> dum;
    entreNodes.push_back(dum);
    i++;
  }
  getline(mesh, str);
  getline(mesh, str);
  mesh >> nSortie;
  cout << "Lecture noeuds sortie " << nSortie << endl;
  i=0;
  while(i<nSortie){
    mesh >> dum;
    sortieNodes.push_back(dum);
    i++;
  }
  getline(mesh, str);
  getline(mesh, str);
  mesh >> nWall;
  cout << "Lecture noeuds wall " << nWall << endl;
  i=0;
  while(i<nWall){
    mesh >> dum;
    wallNodes.push_back(dum);
    i++;
  }

  cout << "Lecture elem finie, nNodes:" << nNodes << " nElems:" << nElems << endl;
  eptr[nElems] = nElems*3;
}

void ecritureFem(int n, vector<vector<double> > nodes, vector<vector<double> > elems){

  int i, nNodes, nElems;
  nNodes = nodes.size();
  nElems = elems.size();
  ofstream out_mesh_n;
  ofstream out_mesh_e;
  stringstream ss;
  ss << "out_mesh." << n << "_nodes.txt";
  out_mesh_n.open(ss.str());
  out_mesh_n << fixed;
  ss.str("");
  ss << "out_mesh." << n << "_elements.txt";
  out_mesh_e.open(ss.str());
  out_mesh_e << fixed;
  ss.str("");

  i=0;
  while(i<nNodes){
    out_mesh_n << nodes[i][1] << " " << nodes[i][2] << endl ;
    i++; 
  }

  i=0;
  while(i<nElems){
    out_mesh_e << elems[i][1]<< " " << elems[i][2]<< " " << elems[i][3] << endl ;
    i++; 
  }
  out_mesh_n.close();
  out_mesh_e.close();
}

void ecritureFant(int n, vector<vector<double> > &nodes, vector<vector<double> > &elems, vector<int> &new_entreNodes, vector<int> &new_sortieNodes, vector<int> &new_wallNodes, vector<vector<int> > &fantElemsRecep, vector<vector<int> > &fantElemsEnvoi){
  
  int i, nNodes, nElems;
  nNodes = nodes.size();
  nElems = elems.size();
  ofstream out_mesh;
  stringstream ss;
  ss << "out_mesh." << n << "_fant.txt";
  out_mesh.open(ss.str());
  out_mesh << fixed ;

  int deb;
  deb = nElems-fantElemsRecep.size();
  for(int k=deb;k<nElems;k++){
    for(int l=0;l<3;l++){
      i = elems[k][1+l]-1;
      out_mesh << nodes[i][1]<< " " << nodes[i][2] << endl ;
    }
  }
  out_mesh.close();
}

void ecriture_base(int n, vector<vector<double> > &nodes, vector<vector<double> > &elems, vector<int> &new_entreNodes, vector<int> &new_sortieNodes, vector<int> &new_wallNodes){

  int i, nNodes, nElems;
  nNodes = nodes.size();
  nElems = elems.size();
  ofstream out_mesh;
  stringstream ss;
  ss << "out_mesh." << n << ".txt";
  out_mesh.open(ss.str());
  out_mesh.precision(4);
  ss.str("");
  out_mesh << fixed ;
  out_mesh << "Table de coordonnees" << endl;
  out_mesh << nNodes << endl;

  i=0;
  while(i<nNodes){
    out_mesh << (int) (nodes[i][0]) << " " << nodes[i][1]<< " " << nodes[i][2] << " " << nodes[i][3] <<" " << nodes[i][4] << endl ;
    i++; 
  }

  out_mesh << "Table de connectivités" << endl;
  out_mesh << nElems << " " << nElems << endl;
  i=0;
  while(i<nElems){
    out_mesh << (int) (elems[i][0]) << " " << (int) (elems[i][1]) << " " << (int) (elems[i][2]) << " " << (int) (elems[i][3]) << " " << elems[i][4] << endl ;
    i++; 
  }

  out_mesh << "Noeuds d'entre" << endl;
  out_mesh << new_entreNodes.size() << endl;
  i=0;
  while(i<new_entreNodes.size()){
    out_mesh << new_entreNodes[i] << endl ;
    i++; 
  }

  out_mesh << "Noeuds de sortie" << endl;
  out_mesh << new_sortieNodes.size() << endl;
  i=0;
  while(i<new_sortieNodes.size()){
    out_mesh << new_sortieNodes[i] << endl ;
    i++; 
  }

  out_mesh << "Noeuds de murs" << endl;
  out_mesh << new_wallNodes.size() << endl;
  i=0;
  while(i<new_wallNodes.size()){
    out_mesh << new_wallNodes[i] << endl ;
    i++; 
  }

  out_mesh.close();
}
void ecriture(string fileName, int n, vector<vector<double> > &nodes, vector<vector<double> > &elems, vector<int> &new_entreNodes, vector<int> &new_sortieNodes, vector<int> &new_wallNodes, vector<vector<int> > &fantElemsRecep, vector<vector<int> > &fantElemsEnvoi, vector<vector<int> > &infoEnvoi, vector<vector<int> > &infoRecep){

  int i, nNodes, nElems;
  nNodes = nodes.size();
  nElems = elems.size();
  ofstream out_mesh;
  stringstream ss;
  //ss << "out_mesh." << n << ".txt";
  ss << n << "_" << fileName;
  out_mesh.open(ss.str());
  ss.str("");
  out_mesh << fixed ;
  out_mesh.precision(4);
  out_mesh << "Table de coordonnees" << endl;
  out_mesh << nNodes << endl;

  i=0;
  while(i<nNodes){
    out_mesh << (int) (nodes[i][0]) << " " << nodes[i][1]<< " " << nodes[i][2] << " " << nodes[i][3] <<" " << nodes[i][4] << endl ;
    i++; 
  }

  out_mesh << "Table de connectivités" << endl;
  out_mesh << nElems << " " << nElems-fantElemsRecep.size() << endl;
  i=0;
  while(i<nElems){
    out_mesh << (int) (elems[i][0]) << " " << (int) (elems[i][1]) << " " << (int) (elems[i][2]) << " " << (int) (elems[i][3]) << " " << elems[i][4] << endl ;
    i++; 
  }

  out_mesh << "Noeuds d'entre" << endl;
  out_mesh << new_entreNodes.size() << endl;
  i=0;
  while(i<new_entreNodes.size()){
    out_mesh << new_entreNodes[i] << endl ;
    i++; 
  }

  out_mesh << "Noeuds de sortie" << endl;
  out_mesh << new_sortieNodes.size() << endl;
  i=0;
  while(i<new_sortieNodes.size()){
    out_mesh << new_sortieNodes[i] << endl ;
    i++; 
  }

  out_mesh << "Noeuds de murs" << endl;
  out_mesh << new_wallNodes.size() << endl;
  i=0;
  while(i<new_wallNodes.size()){
    out_mesh << new_wallNodes[i] << endl ;
    i++; 
  }

  out_mesh << "Mailles fantomes a receptionner" << endl;
  out_mesh << fantElemsRecep.size() << endl;
  i=0;
  while(i<fantElemsRecep.size()){
    //out_mesh << fantElemsRecep[i][0] << " " << fantElemsRecep[i][1] << endl ;
    out_mesh << fantElemsRecep[i][0] << " " << fantElemsRecep[i][1] << " " << fantElemsRecep[i][2] << endl;
    i++; 
  }
  out_mesh << "Mailles fantomes a envoyer" << endl;
  out_mesh << fantElemsEnvoi.size() << endl;
  i=0;
  while(i<fantElemsEnvoi.size()){
    //out_mesh << fantElemsEnvoi[i][0] << " " << fantElemsEnvoi[i][1] << endl ;
    out_mesh << fantElemsEnvoi[i][0] << " " << fantElemsEnvoi[i][1] << " " << fantElemsEnvoi[i][2]<< endl ;
    i++; 
  }
  out_mesh << "Mailles fantomes a receptionner par bloc" << endl;
  out_mesh << infoRecep.size() << endl;
  i=0;
  while(i<infoRecep.size()){
    out_mesh << infoRecep[i][0] << " " << infoRecep[i][1] << " " << infoRecep[i][2]<< endl ;
    i++; 
  }
  out_mesh << "Mailles fantomes a envoyer par bloc" << endl;
  out_mesh << infoEnvoi.size() << endl;
  i=0;
  while(i<infoEnvoi.size()){
    out_mesh << infoEnvoi[i][0] << " " << infoEnvoi[i][1] << " " << infoEnvoi[i][2]<< endl ;
    i++; 
  }
  out_mesh.close();
}

void liensLG(int nParts, int nElems, vector<vector<double> > &elems, idx_t *epart, vector<vector<int> > &nLTG,vector<vector<int> > &nGTL,vector<vector<int> > &eLTG,vector<vector<int> > &eGTL){

  int iE[nParts], iN[nParts];
  fill(iE, iE+nParts, 0);
  fill(iN, iN+nParts, 0);
  for(int i=0;i<nElems;i++){
    //Construction des 2 vecteurs liens elements
    eGTL[epart[i]][i] = iE[epart[i]]+1; 
    eLTG[epart[i]].push_back(i+1); 
    for(int j=0;j<3;j++){
      if(nGTL[epart[i]][elems[i][1+j]-1] == 0){
        nGTL[epart[i]][elems[i][1+j]-1] = iN[epart[i]]+1;
        nLTG[epart[i]].push_back(elems[i][1+j]);
        iN[epart[i]]++;
      }
    }
    iE[epart[i]]++;
  }
}

//constr(nParts, nNodes, nElems, nodes, elems, nLTG, nGTL, eLTG, eGLT); 
void constr(int nParts, int nNodes, int nElems, vector<vector<double> > &nodes, vector<vector<double> > &elems, vector<vector<int> > &nLTG,vector<vector<int> > &nGTL,vector<vector<int> > &eLTG,vector<vector<int> > &eGTL, vector<vector<vector<double> > > &newNodes, vector<vector<vector<double> > > &newElems){
  for(int p=0;p<nParts;p++){
    //Construction des new nodes
    for(int i=0;i<nLTG[p].size();i++){
      vector<double> dummy(5);
      dummy[0] = i+1;
      for(int j=0;j<4;j++){dummy[1+j] = nodes[nLTG[p][i]-1][1+j];}
      newNodes[p].push_back(dummy);
    }
    //Construction des new elems
    for(int i=0;i<eLTG[p].size();i++){
      vector<double> dummy(5);
      dummy[0] = i+1;
      for(int j=0;j<3;j++){dummy[1+j] = nGTL[p][(int) (elems[eLTG[p][i]-1][1+j])-1];}
      dummy[4] = elems[eLTG[p][i]-1][4];
      newElems[p].push_back(dummy);
    }
  }
}

//fantome(nodes, elems, newNodes, newElems, epart, fantElemsRecep, fantNodesRecep, nLTG, nGTL, eLTG, eGTL);
void fantome(int nParts, vector<vector<double> > &nodes, vector<vector<double> > &elems, vector<vector<vector<double> > > &newNodes, vector<vector<vector<double> > > &newElems, idx_t *epart, vector<vector<vector<int> > > &fantElemsRecep,vector<vector<vector<int> > > &fantElemsEnvoi, vector<vector<int> > &nLTG,vector<vector<int> > &nGTL,vector<vector<int> > &eLTG,vector<vector<int> > &eGTL){

  vector<int> commonNodes;
  int maxE[nParts], maxN[nParts];
  vector<vector<int> >  bnGTL(nParts), bnLTG(nParts), beGTL(nParts), beLTG(nParts);
  cout << "Copies de vecteurs " << endl;
  for(int p=0;p<nParts;p++){
    maxE[p] = newElems[p].size();
    maxN[p] = newNodes[p].size();
    for(int i=0;i<nGTL[p].size();i++){
      bnGTL[p].push_back(nGTL[p][i]); 
      beGTL[p].push_back(eGTL[p][i]); 
    }
    for(int i=0;i<nLTG[p].size();i++){
      bnLTG[p].push_back(nLTG[p][i]); 
    }
    for(int i=0;i<eLTG[p].size();i++){
      beLTG[p].push_back(eLTG[p][i]); 
    }
  }
  cout << "Fin de copies de vecteurs " << endl;


  //Fantome meshing
  for(int i=0;i<nParts;i++){
    for(int j=i+1;j<nParts;j++){

      //Creation/Remplissage du vecteur de noeud communs entre les parties i et j
      commonNodes.clear();
      for(int k=0;k<bnLTG[j].size();k++){
        if(bnGTL[i][bnLTG[j][k]-1]!=0){
          commonNodes.push_back(nLTG[j][k]);
        } 
      }
     
      if(commonNodes.size()!=0){
        stringstream ss;
        ss << "com." << i << "." << j << ".dat";
        ofstream nflu(ss.str());
        nflu << fixed;
        //Test plot noeuds communs
        for(int no=0;no<commonNodes.size();no++){
          nflu << commonNodes[no] << " " <<nodes[(int) (commonNodes[no])-1][1]<< " " << nodes[(int) (commonNodes[no])-1][2] << endl;
        }
        nflu.close();
        //}

        //Recherche des mailles fantomes pour i (2 noeuds dans les noeuds communs)
        for(int maille=0;maille<maxE[i];maille++){
          int nc=0; 
          int ncc[3] = {0};

          for(int ne=0;ne<3;ne++){
            for(int pa=0;pa<commonNodes.size();pa++){
              if(bnLTG[i][newElems[i][maille][1+ne]-1]==commonNodes[pa]){nc++;ncc[ne] = 1;}
              //if(bnGTL[i][commonNodes[pa]-1]!=0){nc++;ncc[ne] = 1;}
            }
          }

          if(nc>=2){
            //Ajout du noeud manquant si besoin
            for(int l=0;l<3;l++){
              if(ncc[l] == 0){
                vector<double> v(5);
                v = newNodes[i][newElems[i][maille][1+l]-1];
                v[0] = newNodes[j].size()+1; 
                newNodes[j].push_back(v);
                nLTG[j].push_back(nLTG[i][newElems[i][maille][1+l]-1]);
                nGTL[j][nLTG[i][newElems[i][maille][1+l]-1]-1] = nLTG[j].size();
              }
            }

            //Ajout de la maille fantome a newElems
            vector<double> v(5);
            v[0] = newElems[j].size()+1; 
            for(int l=0;l<3;l++){
              v[1+l] = nGTL[j][nLTG[i][newElems[i][maille][1+l]-1]-1];
            }
            v[4] = newElems[i][maille][4];
            newElems[j].push_back(v);
            eLTG[j].push_back(eLTG[i][maille]);
            eGTL[j][eLTG[i][maille]-1] = eLTG[j].size();

            //Mise a jour fantElemEnvoi et fantElemRecep
            vector<int> du(3);
            du[0] = maille+1;
            //du[1] = epart[eLTG[i][maille]-1];
            du[1] = j;
            du[2] = eLTG[i][maille];
            fantElemsEnvoi[i].push_back(du);
            du[0] = eGTL[j][eLTG[i][maille]-1];
            du[1] = i;
            du[2] = eLTG[i][maille];
            fantElemsRecep[j].push_back(du);
          }
        }

        //Recherche des mailles fantomes pour j (2 noeuds dans les noeuds communs)
        for(int maille=0;maille<maxE[j];maille++){
          int nc=0; 
          int ncc[3] = {0};

          for(int ne=0;ne<3;ne++){
            for(int pa=0;pa<commonNodes.size();pa++){
              if(bnLTG[j][newElems[j][maille][1+ne]-1]==commonNodes[pa]){nc++;ncc[ne] = 1;}
              //if(bnGTL[j][commonNodes[pa]-1]!=0){nc++;ncc[ne] = 1;}
            }
          }

          if(nc>=2){
            //Ajout du noeud manquant si besoin
            for(int l=0;l<3;l++){
              if(ncc[l] == 0){
                vector<double> v(5);
                v = newNodes[j][newElems[j][maille][1+l]-1];
                v[0] = newNodes[i].size()+1; 
                newNodes[i].push_back(v);
                nLTG[i].push_back(nLTG[j][newElems[j][maille][1+l]-1]);
                nGTL[i][nLTG[j][newElems[j][maille][1+l]-1]-1] = nLTG[i].size();
              }
            }

            //Ajout de la maille fantome a newElems
            vector<double> v(5);
            v[0] = newElems[i].size()+1; 
            for(int l=0;l<3;l++){
              v[1+l] = nGTL[i][nLTG[j][newElems[j][maille][1+l]-1]-1];
            }
            v[4] = newElems[j][maille][4];
            newElems[i].push_back(v);
            eLTG[i].push_back(eLTG[j][maille]);
            eGTL[i][eLTG[j][maille]-1] = eLTG[i].size();

            //Mise a jour fantElemEnvoi et fantElemRecep
            vector<int> du(3);
            du[0] = maille+1;
            //du[1] = epart[eLTG[j][maille]-1];
            du[1] = i;
            du[2] = eLTG[j][maille];
            fantElemsEnvoi[j].push_back(du);
            du[0] = eGTL[i][eLTG[j][maille]-1];
            du[1] = j;
            du[2] = eLTG[j][maille];
            fantElemsRecep[i].push_back(du);
          }
        }
      }
    }
  }
}

void boundary(int nParts, vector<vector<double> > &nodes, vector<vector<double> > &elems, vector<vector<vector<double> > > &newNodes, vector<vector<vector<double> > > &newElems, vector<int> &sortieNodes, vector<int> &entreNodes, vector<int> &wallNodes, vector<vector<int> > &new_sortieNodes, vector<vector<int> > &new_entreNodes,vector<vector<int> > &new_wallNodes,vector<vector<int> > &nLTG,vector<vector<int> > &nGTL,vector<vector<int> > &eLTG,vector<vector<int> > &eGTL){
  
  for(int p=0;p<nParts;p++){
    for(int i=0;i<newNodes[p].size();i++){
      for(int j=0;j<entreNodes.size();j++){
        if(nLTG[p][i] == entreNodes[j]){
          new_entreNodes[p].push_back(i+1);
        }
      }
      for(int j=0;j<sortieNodes.size();j++){
        if(nLTG[p][i] == sortieNodes[j]){
          new_sortieNodes[p].push_back(i+1);
        }
      }
      for(int j=0;j<wallNodes.size();j++){
        if(nLTG[p][i] == wallNodes[j]){
          new_wallNodes[p].push_back(i+1);
        }
      }
      
    }
    stringstream w;
    w << "wall." << p << ".dat" ;
    ofstream out(w.str());
    out << fixed ;
    for(int l=0;l<new_wallNodes[p].size();l++){
      out << newNodes[p][(int) (new_wallNodes[p][l])-1][1]<< " " << newNodes[p][(int) (new_wallNodes[p][l])-1][2]<< endl;
    }
    out.close();
}
}

void genOutWall(vector<vector<double> > &nodes, vector<int> &wallNodes){
  ofstream out("all_wall.dat");
  for(int l=0;l<wallNodes.size();l++){
    out << nodes[wallNodes[l]-1][1]<< " " << nodes[wallNodes[l]-1][2] << endl;
  }
  out.close();
}

//renum(nParts, newElems, fantElemsEnvoi);
void renum(int nParts, vector<vector<vector<double> > > &newElems, vector<vector<vector<double> > > &newElemsRenum, vector<vector<vector<int> > > &fantElemsRecep, vector<vector<vector<int> > > &fantElemsEnvoi,vector<vector<int> > &eLTG, vector<vector<int> > &eGTL, vector<vector<int> > &eLTGRenum, vector<vector<int> > &eGTLRenum){

  vector<double> dummy;
  eGTLRenum = eGTL;
  for(int p=0;p<nParts;p++){
    vector<vector<double> > tempEnvoi;
    vector<int> tempEnvoiLoc;
    vector<vector<double> > tempRecep;
    vector<int> tempRecepLoc;

    for(int m=0;m<fantElemsEnvoi[p].size();m++){
      int ml = fantElemsEnvoi[p][m][0]-1;
      tempEnvoi.push_back(newElems[p][ml]);
      tempEnvoiLoc.push_back(ml+1);
      newElems[p][ml][0] = -1;
    }

    for(int m=0;m<fantElemsRecep[p].size();m++){
      int ml = fantElemsRecep[p][m][0]-1;
      tempRecep.push_back(newElems[p][ml]);
      tempRecepLoc.push_back(ml+1);
      newElems[p][ml][0] = -1;
    }

    int ml = 0;
    for(int m=0;m<newElems[p].size();m++){
      if(newElems[p][m][0]!=-1){
        newElemsRenum[p].push_back(newElems[p][m]); 
        newElemsRenum[p][ml][0] = ml+1;
        //indice local avant renum m, après renum ml
        eLTGRenum[p].push_back(eLTG[p][m]);
        eGTLRenum[p][eLTG[p][m]-1] = ml+1;
        ml++;
      }
    }

    int ms = newElemsRenum[p].size();
    for(int m=0;m<tempEnvoi.size();m++){
      newElemsRenum[p].push_back(tempEnvoi[m]); 
      newElemsRenum[p][ms+m][0] = ms+m+1;
      fantElemsEnvoi[p][m][0] = ms+m+1;

      eLTGRenum[p].push_back(eLTG[p][tempEnvoiLoc[m]-1]);
      eGTLRenum[p][eLTG[p][tempEnvoiLoc[m]-1]-1] = ms+m+1;
    }
    ms = newElemsRenum[p].size();
    for(int m=0;m<tempRecep.size();m++){
      newElemsRenum[p].push_back(tempRecep[m]); 
      newElemsRenum[p][ms+m][0] = ms+m+1;
      fantElemsRecep[p][m][0] = ms+m+1;

      eLTGRenum[p].push_back(eLTG[p][tempRecepLoc[m]-1]);
      eGTLRenum[p][eLTG[p][tempRecepLoc[m]-1]-1] = ms+m+1;
    }

  }
}

//genInfoSendRecv(nParts, fantElemsEnvoi, fantElemsRecep, infoEnvoi, infoRecep);
void genInfoSendRecv(int nParts, vector<vector<vector<int> > > &fantElemsEnvoi, vector<vector<vector<int> > > &fantElemsRecep, vector<vector<vector<int> > > &infoEnvoi, vector<vector<vector<int> > > &infoRecep){

  int bSize(1);
  vector<int> dummy(3,-1) ;
  
  for(int p=0;p<nParts;p++){

    //InfoEnvoi
    bSize = 1;
    for(int m=0;m<fantElemsEnvoi[p].size()-1;m++){
      if(fantElemsEnvoi[p][m+1][1] != fantElemsEnvoi[p][m][1]){
        //Alors fin d'un bloc d'envoi 
        dummy[0] = fantElemsEnvoi[p][m-bSize+1][0];
        dummy[1] = bSize;
        dummy[2] = fantElemsEnvoi[p][m][1];
        infoEnvoi[p].push_back(dummy);
        bSize=1;
      }
      else{
        bSize++;
      }
    }
    dummy[0] = fantElemsEnvoi[p][fantElemsEnvoi[p].size()-bSize][0];
    dummy[1] = bSize;
    dummy[2] = fantElemsEnvoi[p][fantElemsEnvoi[p].size()-1][1];
    infoEnvoi[p].push_back(dummy);
  
    //InfoRecep
    bSize = 1;
    for(int m=0;m<fantElemsRecep[p].size()-1;m++){
      if(fantElemsRecep[p][m+1][1] != fantElemsRecep[p][m][1]){
        //Alors fin d'un bloc de recep
        dummy[0] = fantElemsRecep[p][m-bSize+1][0];
        dummy[1] = bSize;
        dummy[2] = fantElemsRecep[p][m][1];
        infoRecep[p].push_back(dummy);
        bSize=1;
      }
      else{
        bSize++;
      }
    }
    dummy[0] = fantElemsRecep[p][fantElemsRecep[p].size()-bSize][0];
    dummy[1] = bSize;
    dummy[2] = fantElemsRecep[p][fantElemsRecep[p].size()-1][1];
    infoRecep[p].push_back(dummy);
  }
}
void ecritureLiens(int n, vector<int> &nLTG){
  
  ofstream out;
  stringstream ss;
  ss << "out_lien_nodes." << n << ".txt";
  out.open(ss.str());

  for(int i=0;i<nLTG.size();i++){
    out << i+1 << " " << nLTG[i] << endl;
  }

  out.close();
}

int main(int argc, char* argv[]){

  idx_t objval, nParts;
  ifstream mesh;
  string fileName;

  if(argc < 2){
    cout << "Utilisation : ./main fichier_maillage nombre_partitions" << endl;
    return(0);
  }
  else{
    fileName = argv[1];
    mesh.open(argv[1]);
    nParts = atoi(argv[2]);
  }

  int n, i, nNodes, nElems, ncom(1);
  int new_nElems, new_nNodes;
  vector<int> entreNodes, sortieNodes, wallNodes;
  vector<vector<int> > new_entreNodes(nParts);
  vector<vector<int> > new_sortieNodes(nParts);
  vector<vector<int> > new_wallNodes(nParts);
  vector<vector<double> > nodes, elems;
  vector<vector<double> > dummy;
  vector<vector<vector<double> > > newNodes(nParts), newElems(nParts), newElemsCopie(nParts);
  vector<vector<vector<int> > > infoEnvoi(nParts), infoRecep(nParts);
  idx_t* eind;
  idx_t* eptr;

  cout << fixed;

  //Lecture du maillage
  lecture(mesh, nodes, elems, eind, eptr, entreNodes, sortieNodes, wallNodes);
  mesh.close();


  //Creation des vecteurs de lien entre indice global et indice local pour chaque sous maillage
  vector<vector<int> > nLTG(nParts, vector<int>(0));
  vector<vector<int> > nGTL(nParts, vector<int>(nodes.size()));
  vector<vector<int> > eLTG(nParts, vector<int>(0));
  vector<vector<int> > eGTL(nParts, vector<int>(elems.size()));
  vector<vector<int> > eLTGCopie(nParts, vector<int>(0));
  vector<vector<int> > eGTLCopie(nParts, vector<int>(elems.size()));
  
  //Ecriture de tous les noeud de murs dans un fichier "all_wall.dat"
  genOutWall(nodes,wallNodes);

  //Variables pour METIS
  nNodes = nodes.size();
  nElems = elems.size();
  idx_t npart[nodes.size()], epart[elems.size()];

  //METIS mesh split
  cout << "METIS ..." << endl;
  METIS_PartMeshDual(&nElems, &nNodes, eptr, eind, NULL, NULL, &ncom, &nParts, NULL, NULL, &objval, epart, npart);
  cout << "METIS OK" << endl;

  cout << "Construction des liens entres indices locaux et globaux " << endl;
  liensLG(nParts, nElems, elems, epart, nLTG, nGTL, eLTG, eGTL); 

  cout << "Construction des sous maillages" << endl;
  constr(nParts, nNodes, nElems, nodes, elems, nLTG, nGTL, eLTG, eGTL, newNodes, newElems); 
  int maxN(newNodes.size());

  cout << "Ajout de mailles fantomes à chaque sous maillage" << endl;
  vector<vector<vector<int> > > fantElemsRecep(nParts), fantElemsEnvoi(nParts);
  fantome(nParts, nodes, elems, newNodes, newElems, epart, fantElemsRecep, fantElemsEnvoi, nLTG, nGTL, eLTG, eGTL);

  cout << "Traitement des noeuds d'entre sortie et murs" << endl;
  boundary(nParts, nodes, elems, newNodes, newElems, sortieNodes, entreNodes, wallNodes, new_sortieNodes, new_entreNodes, new_wallNodes, nLTG, nGTL, eLTG, eGTL);

  cout << "Renumerotation des mailles a envoyer" << endl;
  renum(nParts, newElems, newElemsCopie, fantElemsRecep, fantElemsEnvoi, eLTG, eGTL, eLTGCopie, eGTLCopie);
  newElems = newElemsCopie;
  eLTG = eLTGCopie;
  eGTL = eGTLCopie;


  cout << "Génération des informations send/recp" << endl;
  genInfoSendRecv(nParts, fantElemsEnvoi, fantElemsRecep, infoEnvoi, infoRecep);

  //Ecriture des fichiers de maillages
  cout << "Ecriture des fichiers de maillages finaux." << endl;
  for(int p=0;p<nParts;p++){
    cout << "Part " << p << ", " << fantElemsRecep[p].size() << " mailles à recep et " << fantElemsEnvoi[p].size() << " à envoyer." << endl;
    //ecritureFem(p, newNodes[p], newElems[p]);
    //ecritureFant(p,newNodes[p], newElems[p], new_entreNodes[p], new_sortieNodes[p], new_wallNodes[p], fantElemsRecep[p], fantElemsEnvoi[p]);
    ecriture(fileName, p,newNodes[p], newElems[p], new_entreNodes[p], new_sortieNodes[p], new_wallNodes[p], fantElemsRecep[p], fantElemsEnvoi[p], infoRecep[p], infoEnvoi[p]);
    ecritureLiens(p, nLTG[p]);
  }

  //ecritureFem(-1, nodes, elems);
  //ecriture_base(-1,nodes, elems, entreNodes, sortieNodes, wallNodes);
  delete [] eptr;
  delete [] eind;
  eptr = NULL;
  eind = NULL;
  return(0);
}

