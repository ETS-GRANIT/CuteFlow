#include "metis.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>

using namespace std;

void lecture(bool multi_entree, bool multi_sortie, ifstream &mesh, vector<vector<double> > &nodes, vector<vector<double> > &elems,idx_t *&eind, idx_t *&eptr, vector<int> &entreNodes, vector<int> &numEntreNodes, vector<int> &sortieNodes, vector<int> &numSortieNodes, vector<int> &wallNodes, int &nombreEntre, int &nombreSortie){

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
      entreNodes.push_back(dum_v[0]);
      numEntreNodes.push_back(dum_v[1]);
      i++;
    }
  }
  else{
    while(i<nEntre){
      mesh >> dum_v[0];
      dum_v[1] = 1; //unique entrée numéro 1
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
      sortieNodes.push_back(dum_v[0]);
      numSortieNodes.push_back(dum_v[1]);
      i++;
    }
  }
  else{
    while(i<nSortie){
      mesh >> dum_v[0];
      dum_v[1] = 1; //unique sortie numero 1
      sortieNodes.push_back(dum_v[0]);
      numSortieNodes.push_back(dum_v[1]);
      i++;
    }
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

void ecritureGnuplot(int n, vector<vector<double> > nodes, vector<vector<double> > elems){
  //Génère un fichier de sortie [0-9]_gnuplot_mesh.txt pour visualisation avec gnuplot
  //dans gnuplot il suffit de faire : plot "[0-9]_gnuplot_mesh.txt"

  int i, nNodes, nElems;
  nNodes = nodes.size();
  nElems = elems.size();
  ofstream out_mesh_n;
  stringstream ss;
  ss << n << "_gnuplot_mesh.txt";
  out_mesh_n.open(ss.str());
  out_mesh_n << fixed;

  i=0;
  while(i<nNodes){
    out_mesh_n << nodes[i][1] << " " << nodes[i][2] << endl ;
    i++; 
  }
  out_mesh_n.close();

}

void ecritureFantGnuplot(int n, vector<vector<double> > &nodes, vector<vector<double> > &elems, vector<int> &new_entreNodes, vector<int> &new_sortieNodes, vector<int> &new_wallNodes, vector<vector<int> > &fantElemsRecep, vector<vector<int> > &fantElemsEnvoi){
  //Ecriture des noeuds fantômes de chaque sous maillages pour débugger
  //meme principe que ecritureGnuplot

  int i, nNodes, nElems;
  nNodes = nodes.size();
  nElems = elems.size();
  ofstream out_mesh;
  stringstream ss;
  ss << n << "_nodes_fant.txt";
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

void ecriture(bool multi_sortie, bool multi_entree, string fileName, int n, vector<vector<double> > &nodes, vector<vector<double> > &elems, vector<int> &new_entreNodes, vector<int> &new_numEntreNodes, vector<int> &new_sortieNodes, vector<int> &new_numSortieNodes, vector<int> &new_wallNodes, vector<vector<int> > &fantElemsRecep, vector<vector<int> > &fantElemsEnvoi, vector<vector<int> > &infoEnvoi, vector<vector<int> > &infoRecep, int &nombreEntre, int &nombreSortie, vector<int> renumSD){

  int i, nNodes, nElems;
  nNodes = nodes.size();
  nElems = elems.size();
  ofstream out_mesh;
  stringstream ss;
  //ss << "out_mesh." << n << ".txt";
  ss << renumSD[n] << "_" << fileName;
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

  out_mesh << "Noeuds de murs" << endl;
  out_mesh << new_wallNodes.size() << endl;
  i=0;
  while(i<new_wallNodes.size()){
    out_mesh << new_wallNodes[i] << endl ;
    i++; 
  }

  /* out_mesh << "Mailles fantomes a receptionner" << endl; */
  /* out_mesh << fantElemsRecep.size() << endl; */
  /* i=0; */
  /* while(i<fantElemsRecep.size()){ */
  /*   //out_mesh << fantElemsRecep[i][0] << " " << fantElemsRecep[i][1] << endl ; */
  /*   out_mesh << fantElemsRecep[i][0] << " " << renumSD[fantElemsRecep[i][1]] << " " << fantElemsRecep[i][2] << endl; */
  /*   i++; */ 
  /* } */
  /* out_mesh << "Mailles fantomes a envoyer" << endl; */
  /* out_mesh << fantElemsEnvoi.size() << endl; */
  /* i=0; */
  /* while(i<fantElemsEnvoi.size()){ */
  /*   //out_mesh << fantElemsEnvoi[i][0] << " " << fantElemsEnvoi[i][1] << endl ; */
  /*   out_mesh << fantElemsEnvoi[i][0] << " " << renumSD[fantElemsEnvoi[i][1]] << " " << fantElemsEnvoi[i][2]<< endl ; */
  /*   i++; */ 
  /* } */

  out_mesh << "Mailles fantomes a receptionner par bloc" << endl;
  out_mesh << infoRecep.size() << endl;
  i=0;
  while(i<infoRecep.size()){
    out_mesh << infoRecep[i][0] << " " << infoRecep[i][1] << " " << renumSD[infoRecep[i][2]]<< endl ;
    i++; 
  }
  out_mesh << "Mailles fantomes a envoyer par bloc" << endl;
  out_mesh << infoEnvoi.size() << endl;
  i=0;
  while(i<infoEnvoi.size()){
    out_mesh << infoEnvoi[i][0] << " " << infoEnvoi[i][1] << " " << renumSD[infoEnvoi[i][2]]<< endl ;
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

  cout << "Construction d'adjency vector pour controle maille fantome" << endl;
  vector<vector<vector<int> > > adj(nParts);
  for(int p=0;p<nParts;p++){
    int n1,n2,n3;
    adj[p].resize(newNodes[p].size());
    for(int i=0;i<newElems[p].size();i++){
      n1 = newElems[p][i][1]-1;
      n2 = newElems[p][i][2]-1;
      n3 = newElems[p][i][3]-1;
      adj[p][n1].push_back(n2+1);
      adj[p][n1].push_back(n3+1);
      adj[p][n2].push_back(n1+1);
      adj[p][n2].push_back(n3+1);
      adj[p][n3].push_back(n1+1);
      adj[p][n3].push_back(n2+1);
    }
  }




  cout << "Debut de l'ajout des mailles fantomes" << endl;


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
        /* stringstream ss; */
        /* ss << "common_nodes." << i << "." << j << ".txt"; */
        /* ofstream nflu(ss.str()); */
        /* nflu << fixed; */
        /* //Test plot noeuds communs */
        /* for(int no=0;no<commonNodes.size();no++){ */
        /*   nflu << commonNodes[no] << " " <<nodes[(int) (commonNodes[no])-1][1]<< " " << nodes[(int) (commonNodes[no])-1][2] << endl; */
        /* } */
        /* nflu.close(); */

        //Recherche des mailles fantomes pour i (2 noeuds dans les noeuds communs)
        for(int maille=0;maille<maxE[i];maille++){
          int nc=0; 
          int ncc[3] = {0};
          bool controle=false;

          for(int ne=0;ne<3;ne++){
            for(int pa=0;pa<commonNodes.size();pa++){
              if(bnLTG[i][newElems[i][maille][1+ne]-1]==commonNodes[pa]){nc++;ncc[ne] = 1;}
              //if(bnGTL[i][commonNodes[pa]-1]!=0){nc++;ncc[ne] = 1;}
            }
          }

          if(nc==2){
            int ni1, ni2, nj1, nj2;
            if(ncc[0]==0){
              ni1=newElems[i][maille][2]-1;
              ni2=newElems[i][maille][3]-1;
            }
            else if(ncc[1]==0){
              ni1=newElems[i][maille][1]-1;
              ni2=newElems[i][maille][3]-1;
            }
            else if(ncc[2]==0){
              ni1=newElems[i][maille][1]-1;
              ni2=newElems[i][maille][2]-1;
            }

            nj1=bnGTL[j][bnLTG[i][ni1]-1]-1;
            nj2=bnGTL[j][bnLTG[i][ni2]-1]-1;

              for(int k=0;k<adj[j][nj1].size();k++){
                if(adj[j][nj1][k] == nj2+1){
                  controle=true; 
                }
              }
          } 
          else if(nc==3){
            controle=true; 
          }

          if(nc>=2 and controle){
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
          bool controle=false;

          for(int ne=0;ne<3;ne++){
            for(int pa=0;pa<commonNodes.size();pa++){
              if(bnLTG[j][newElems[j][maille][1+ne]-1]==commonNodes[pa]){nc++;ncc[ne] = 1;}
              //if(bnGTL[j][commonNodes[pa]-1]!=0){nc++;ncc[ne] = 1;}
            }
          }

          if(nc==2){
            int ni1, ni2, nj1, nj2;
            if(ncc[0]==0){
              nj1=newElems[j][maille][2]-1;
              nj2=newElems[j][maille][3]-1;
            }
            else if(ncc[1]==0){
              nj1=newElems[j][maille][1]-1;
              nj2=newElems[j][maille][3]-1;
            }
            else if(ncc[2]==0){
              nj1=newElems[j][maille][1]-1;
              nj2=newElems[j][maille][2]-1;
            }

            ni1=bnGTL[i][bnLTG[j][nj1]-1]-1;
            ni2=bnGTL[i][bnLTG[j][nj2]-1]-1;

              for(int k=0;k<adj[i][ni1].size();k++){
                if(adj[i][ni1][k] == ni2+1){
                  controle=true; 
                }
              }
          } 
          else if(nc==3){
            controle=true; 
          }

          if(nc>=2 and controle){
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

void boundary(int nParts, vector<vector<double> > &nodes, vector<vector<double> > &elems, vector<vector<vector<double> > > &newNodes, vector<vector<vector<double> > > &newElems, vector<int> &sortieNodes, vector<int> &numSortieNodes, vector<int> &entreNodes, vector<int> &numEntreNodes, vector<int> &wallNodes, vector<vector<int> > &new_sortieNodes, vector<vector<int> > &new_numSortieNodes,vector<vector<int> > &new_entreNodes, vector<vector<int> > &new_numEntreNodes, vector<vector<int> > &new_wallNodes,vector<vector<int> > &nLTG,vector<vector<int> > &nGTL,vector<vector<int> > &eLTG,vector<vector<int> > &eGTL){
  
  vector<int> boundTag(nodes.size(),0);
  vector<int> numEntreeSortie(nodes.size(),0);
  for(int i=0;i<entreNodes.size();i++){
    boundTag[entreNodes[i]-1] = -1; //Tag entree
    numEntreeSortie[entreNodes[i]-1] = numEntreNodes[i]; 
  }
  for(int i=0;i<sortieNodes.size();i++){
    boundTag[sortieNodes[i]-1] = -2; //Tag sortie
    numEntreeSortie[sortieNodes[i]-1] = numSortieNodes[i]; 
  }
  for(int i=0;i<wallNodes.size();i++){
    boundTag[wallNodes[i]-1] = -3; //Tag mur
  }

  for(int p=0;p<nParts;p++){
    for(int i=0;i<newNodes[p].size();i++){
        if(boundTag[nLTG[p][i]-1] == -1){
          new_entreNodes[p].push_back(i+1);
          new_numEntreNodes[p].push_back(numEntreeSortie[nLTG[p][i]-1]);
        }
      
        if(boundTag[nLTG[p][i]-1] == -2){
          new_sortieNodes[p].push_back(i+1);
          new_numSortieNodes[p].push_back(numEntreeSortie[nLTG[p][i]-1]);
        }
        if(boundTag[nLTG[p][i]-1] == -3){
          new_wallNodes[p].push_back(i+1);
        }
    }
    /* stringstream w; */
    /* w << p << "_wall.txt"; */
    /* ofstream out(w.str()); */
    /* out << fixed ; */
    /* for(int l=0;l<new_wallNodes[p].size();l++){ */
    /*   out << newNodes[p][(int) (new_wallNodes[p][l])-1][1]<< " " << newNodes[p][(int) (new_wallNodes[p][l])-1][2]<< endl; */
    /* } */
    /* out.close(); */
    /* stringstream w; */
    /* w << p << "_entree.txt"; */
    /* ofstream out(w.str()); */
    /* out << fixed ; */
    /* for(int l=0;l<new_entreNodes[p].size();l++){ */
    /*   out << newNodes[p][(int) (new_entreNodes[p][l])-1][1]<< " " << newNodes[p][(int) (new_entreNodes[p][l])-1][2]<< endl; */
    /* } */
    /* out.close(); */
}
}

void genOutWall(vector<vector<double> > &nodes, vector<int> &wallNodes){
  ofstream out("all_wall.txt");
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


void renumSousDomaines(int nParts, vector<int> &renumerotationSousDomaine, vector<vector<vector<int> > > &infoEnvoi, vector<vector<int> > &new_entreNodes){

  int deg=10000;
  vector<int> renum;

  //Calcul des vertex degree
  vector<int> vertexDegree(nParts);
  for(int p=0;p<nParts;p++){
    vertexDegree[p] = infoEnvoi[p].size(); //vertex degree est le nombre de voisins du domaine
  }

  //Initialisation du premier domaine
  renum.push_back(-1);

  //On recherche un domaine avec un noeud d'entrée pour servir de premier domaine
  for(int p=0;p<nParts;p++){
    if(new_entreNodes[p].size()>1){
      if(vertexDegree[p]<deg){
        renum[0] = p;
        deg=vertexDegree[p];
      }
    }
  }

  //Si on ne trouve pas de domaines d'entrée
  if(renum[0] == -1){
    deg=10000;
    for(int p=0;p<nParts;p++){
      if(vertexDegree[p]<deg){
        renum[0] = p;
        deg=vertexDegree[p];
      }
    }
  }

  vector<int> adjencySet;
  vector<int> adjencySetClean;

  //Cuthil-Mckee
  int np=0;
  while(renum.size()<nParts){

    //Nettoyage et construction de l'adjency vector
    adjencySet.clear();
    adjencySetClean.clear();
    
    for(int j=0;j<infoEnvoi[renum[np]].size();j++){
      adjencySet.push_back(infoEnvoi[renum[np]][j][2]);
    }
   
    //Enleve de l'adjency vector les domaines déja dans renum
    for(int j=0;j<adjencySet.size();j++){
      //Chesk si deja dans renum
      bool is_inside=false;
      for(int i=0;i<renum.size();i++){
        if(renum[i]==adjencySet[j]){
          is_inside=true;
          break;
        }
      }
      if(!is_inside){
        adjencySetClean.push_back(adjencySet[j]); 
      }
    }

    //Trie l'adjency vector (bubble sort)
    int temp;
    for(int i=0;i<adjencySetClean.size();i++){
      for(int j=0;j<adjencySetClean.size()-i-1;j++){
        if(vertexDegree[adjencySetClean[j]]<vertexDegree[adjencySetClean[j+1]]){
          temp = adjencySetClean[j];
          adjencySetClean[j] = adjencySetClean[j+1];
          adjencySetClean[j+1] = temp;
        }
      }
    }

    for(int i=0;i<adjencySetClean.size();i++){
      renum.push_back(adjencySetClean[i]); 
    }
    np++;
  }


  //remplie renumerotationSousDomaines
  for(int i=0;i<nParts;i++){
    renumerotationSousDomaine[renum[i]] = i;
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

void ecritureLiens(int n, vector<int> &nLTG, vector<int> &eLTG, vector<int> &renumSD){
  
  ofstream out;
  stringstream ss;
  ss << renumSD[n] << "_liens_nodes.txt";
  out.open(ss.str());

  for(int i=0;i<nLTG.size();i++){
    out << i+1 << " " << nLTG[i] << endl;
  }

  out.close();

  ss.str("");
  ss << renumSD[n] << "_liens_elems.txt";
  out.open(ss.str());

  for(int i=0;i<eLTG.size();i++){
    out << i+1 << " " << eLTG[i] << endl;
  }

  out.close();
}

int main(int argc, char* argv[]){

  idx_t objval, nParts;
  ifstream mesh;
  string fileName;
  bool multi_entree=0;
  bool multi_sortie=0;

  if(argc < 5){
    cout << "Utilisation : ./split_mesh fichier_de_maillage nombre_de_sous_domaines multi_entrees multi_sorties" << endl;
    cout << "multi_entree=1 uniquement si le maillage a découper posséde plusieurs entrées" << endl;
    cout << "multi_sortie=1 uniquement si le maillage a découper posséde plusieurs sorties" << endl;
    return(0);
  }
  else{
    fileName = argv[1];
    mesh.open(argv[1]);
    nParts = atoi(argv[2]);
    multi_entree = atoi(argv[3]);
    multi_sortie = atoi(argv[4]);
  }

  int n, i, nNodes, nElems, ncom(1), nombreEntre, nombreSortie;
  int new_nElems, new_nNodes;
  vector<int> entreNodes, sortieNodes, wallNodes, numEntreNodes, numSortieNodes, renumerotationSousDomaine(nParts);
  vector<vector<int> > new_entreNodes(nParts), new_numEntreNodes(nParts);
  vector<vector<int> > new_sortieNodes(nParts), new_numSortieNodes(nParts);
  vector<vector<int> > new_wallNodes(nParts);
  vector<vector<double> > nodes, elems;
  vector<vector<double> > dummy;
  vector<vector<vector<double> > > newNodes(nParts), newElems(nParts), newElemsCopie(nParts);
  vector<vector<vector<int> > > infoEnvoi(nParts), infoRecep(nParts);
  idx_t* eind;
  idx_t* eptr;

  cout << fixed;

  //Lecture du maillage
  lecture(multi_entree, multi_sortie, mesh, nodes, elems, eind, eptr, entreNodes, numEntreNodes, sortieNodes, numSortieNodes, wallNodes, nombreEntre, nombreSortie);
  mesh.close();


  //Creation des vecteurs de lien entre indice global et indice local pour chaque sous maillage
  vector<vector<int> > nLTG(nParts, vector<int>(0));
  vector<vector<int> > nGTL(nParts, vector<int>(nodes.size()));
  vector<vector<int> > eLTG(nParts, vector<int>(0));
  vector<vector<int> > eGTL(nParts, vector<int>(elems.size()));
  vector<vector<int> > eLTGCopie(nParts, vector<int>(0));
  vector<vector<int> > eGTLCopie(nParts, vector<int>(elems.size()));
  
  //Ecriture de tous les noeud de murs dans un fichier "all_wall.txt"
  //genOutWall(nodes,wallNodes);

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

  cout << "Ajout de mailles fantômes à chaque sous maillage" << endl;
  vector<vector<vector<int> > > fantElemsRecep(nParts), fantElemsEnvoi(nParts);
  fantome(nParts, nodes, elems, newNodes, newElems, epart, fantElemsRecep, fantElemsEnvoi, nLTG, nGTL, eLTG, eGTL);

  cout << "Traitement des noeuds d'entrée sortie et murs" << endl;
  boundary(nParts, nodes, elems, newNodes, newElems, sortieNodes, numSortieNodes, entreNodes, numEntreNodes, wallNodes, new_sortieNodes, new_numSortieNodes, new_entreNodes, new_numEntreNodes, new_wallNodes, nLTG, nGTL, eLTG, eGTL);

  cout << "Renumerotation des mailles à envoyer" << endl;
  renum(nParts, newElems, newElemsCopie, fantElemsRecep, fantElemsEnvoi, eLTG, eGTL, eLTGCopie, eGTLCopie);
  newElems = newElemsCopie;
  eLTG = eLTGCopie;
  eGTL = eGTLCopie;

  cout << "Génération des informations send/recp" << endl;
  genInfoSendRecv(nParts, fantElemsEnvoi, fantElemsRecep, infoEnvoi, infoRecep);

  cout << "Renumerotation des sous domaines" << endl;
  renumSousDomaines(nParts, renumerotationSousDomaine, infoEnvoi, new_entreNodes);

  /* for(int i=0;i<nParts;i++){ */
  /*   renumerotationSousDomaine[i] = i; */ 
  /* } */

  //Ecriture des fichiers de maillages
  cout << "Ecriture des fichiers de maillages finaux." << endl;
  for(int p=0;p<nParts;p++){
    cout << "Part " << renumerotationSousDomaine[p] << ", " << fantElemsRecep[p].size() << " mailles à recep et " << fantElemsEnvoi[p].size() << " à envoyer." << endl;
    //ecritureGnuplot(p, newNodes[p], newElems[p]);
    //ecritureFantGnuplot(p,newNodes[p], newElems[p], new_entreNodes[p], new_sortieNodes[p], new_wallNodes[p], fantElemsRecep[p], fantElemsEnvoi[p]);
    ecriture(multi_sortie, multi_entree, fileName, p,newNodes[p], newElems[p], new_entreNodes[p], new_numEntreNodes[p], new_sortieNodes[p], new_numSortieNodes[p], new_wallNodes[p], fantElemsRecep[p], fantElemsEnvoi[p],infoEnvoi[p], infoRecep[p], nombreEntre, nombreSortie,renumerotationSousDomaine);
    ecritureLiens(p, nLTG[p], eLTG[p], renumerotationSousDomaine);
  }

  delete [] eptr;
  delete [] eind;
  eptr = NULL;
  eind = NULL;
  return(0);
}

