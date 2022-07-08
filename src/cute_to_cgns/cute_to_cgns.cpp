#include "cgnslib.h"
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iostream>
#include <vector>
#include <iomanip>  // std::setprecision()
#include <cstdlib>


#if CGNSVERSION < 3100
# define cgsizet int
#else
# if CGBUILDSCOPE
# error enumeration scoping needs to be off
# endif
#endif


int main(int argc, char* argv[]){
  int index_file, index_base;
  /* open CGNS file for write */
  if (cg_open(argv[2],CG_MODE_WRITE,&index_file)) cg_error_exit();
  int icelldim=2;
  int iphysdim=3;
  const char *basename = "Base";
  cg_base_write(index_file,basename,icelldim,iphysdim,&index_base);

  int multi_entree = std::atoi(argv[3]);

  std::string filename=argv[1];
  std::stringstream fss;
  fss << filename;
  std::ifstream infile;
  infile >> std::fixed >> std::setprecision(14);
  std::cout << std::fixed << std::setprecision(14);

  infile.open(fss.str());
  std::string line;
  int nnodes, nelems, ninflow, noutflow, nwall, ninput;
  long int id1;
  getline(infile,line); //Table coord
  infile >> nnodes;
  double *x, *y, *z, d1;
  x = new double[nnodes];
  y = new double[nnodes];
  z = new double[nnodes];
  for(int i=0; i<nnodes; i++){
    infile >> id1 >> x[i] >> y[i] >> z[i] >> d1;
    /* infile >> id1 >> x[i] >> y[i] >> z[i] ; */
  }

  int ii=0;
  int s1,s2,s3;
  double x1,y1,x2,y2,x3,y3,s,a,b,c,area;

  getline(infile,line); //Blank
  getline(infile,line); //Table connect
  infile >> nelems;
  cgsize_t *elems = new cgsize_t[nelems*3];
  for(int i=0; i<nelems; i++){
    infile >> id1 >> elems[3*i] >> elems[3*i+1] >> elems[3*i+2] >> d1;
    ii++;


    /* infile >> id1 >> s1 >> s2 >> s3 >> d1; */
    /* x1 = x[s1]; */
    /* y1 = y[s1]; */
    /* x2 = x[s2]; */
    /* y2 = y[s2]; */
    /* x3 = x[s3]; */
    /* y3 = y[s3]; */
    /* a = sqrt(pow(x2-x1,2) + pow(y2-y1,2)); */
    /* b = sqrt(pow(x3-x2,2) + pow(y3-y2,2)); */
    /* c = sqrt(pow(x1-x3,2) + pow(y1-y3,2)); */
    /* s = 0.5*(a+b+c); */
    /* area = sqrt(s*(s-a)*(s-b)*(s-c)); */
    /* if(area > 1e-10){ */
    /*   elems[3*ii] = s1; */
    /*   elems[3*ii+1] = s2; */
    /*   elems[3*ii+2] = s3; */
    /*   ii++; */
    /* } */
  }

  cgsize_t *elems_pure = new cgsize_t[ii*3];
  for(int i=0; i<3*ii; i++){
    elems_pure[i] = elems[i];
  }

  getline(infile,line); //Blank
  getline(infile,line); //Noeuds entree

  cgsize_t *inflow_nodes, *inflow_nodes_multi;
  int *ninflow_per_input;

  /* SIMPLE ENTREE */
  if(multi_entree==0){
    infile >> ninflow;
    if(ninflow > 0){
      inflow_nodes = new cgsize_t[ninflow];
      for(int i=0; i<ninflow; i++){
        infile >> inflow_nodes[i];
      }
    }
  }
  else{
    /* MULTI ENTREE */
    infile >> ninflow >> ninput;
    ninflow_per_input = new int[ninput];
    for(int i=0; i<ninput; i++) ninflow_per_input[i] = 0;
    inflow_nodes = new cgsize_t[ninflow];
    inflow_nodes_multi = new cgsize_t[ninput*ninflow];
    int i_input;
    for(int i=0; i<ninflow; i++){
      infile >> inflow_nodes[i] >> i_input;
      inflow_nodes_multi[(i_input-1)*ninflow+ninflow_per_input[i_input-1]] = inflow_nodes[i];
      ninflow_per_input[i_input-1] += 1;

    }
  }

  getline(infile,line); //Blank
  getline(infile,line); //Noeuds sortie
  infile >> noutflow;
  cgsize_t *outflow_nodes;
  if(noutflow > 0){
    outflow_nodes = new cgsize_t[noutflow];
    for(int i=0; i<noutflow; i++){
      infile >> outflow_nodes[i];
    }
  }
  getline(infile,line); //Blank
  getline(infile,line); //Noeuds mur
  infile >> nwall;
  cgsize_t *wall_nodes;
  if(nwall > 0){
    wall_nodes = new cgsize_t[nwall];
    for(int i=0; i<nwall; i++){
      infile >> wall_nodes[i];
    }
  }
  infile.close();

  std::cout << nnodes << " " << nelems << " " << ninflow << " " << noutflow << " " << nwall << std::endl;

  int index_coord, index_zone, index_section, index_bc;
  /* cgsize_t nstart(1), nend(nelems); */
  cgsize_t nstart(1), nend(ii);

  std::cout << nelems-ii << " elements enlevés" << std::endl;

  const char *zonename;
  std::stringstream zss;
  zss << "Zone";
  zonename = zss.str().c_str();
  cgsize_t isize[1][3];
  isize[0][0]=nnodes;
  isize[0][1]=nend;
  isize[0][2]=0;

  cg_zone_write(index_file,index_base,zonename,*isize,CGNS_ENUMV(Unstructured),&index_zone);
  cg_coord_write(index_file,index_base,index_zone,CGNS_ENUMV(RealDouble),"CoordinateX",x,&index_coord);
  cg_coord_write(index_file,index_base,index_zone,CGNS_ENUMV(RealDouble),"CoordinateY",y,&index_coord);
  cg_coord_write(index_file,index_base,index_zone,CGNS_ENUMV(RealDouble),"CoordinateZ",z,&index_coord);
  /* cg_section_write(index_file,index_base,index_zone,"Elements",CGNS_ENUMV(TRI_3),nstart,nend,0,elems,&index_section); */
  cg_section_write(index_file,index_base,index_zone,"Elements",CGNS_ENUMV(TRI_3),nstart,nend,0,elems_pure,&index_section);

  if(multi_entree==0){
    /* SIMPLE ENTREE */
    if(ninflow > 0) {
      cg_boco_write(index_file,index_base,index_zone,"Inflow nodes",CGNS_ENUMV(BCInflow),CGNS_ENUMV(PointList),ninflow,inflow_nodes,&index_bc);
    }
  }
  else{
    /* MULTI ENTREE */
    int ss=0;
    for(int i=0; i<ninput; i++){
      std::stringstream in_name;
      in_name << "Inflow nodes " << i;
      std::cout << ninflow_per_input[i] << " " << ss << std::endl;
      /* cg_boco_write(index_file,index_base,index_zone,in_name.str().c_str(),CGNS_ENUMV(BCInflow),CGNS_ENUMV(PointList),ninflow_per_input[i],&inflow_nodes[ss],&index_bc); // BCInflow = 9 */
      cg_boco_write(index_file,index_base,index_zone,in_name.str().c_str(),CGNS_ENUMV(BCInflow),CGNS_ENUMV(PointList),ninflow_per_input[i],&inflow_nodes_multi[i*ninflow],&index_bc); // BCInflow = 9
      ss+=ninflow_per_input[i];
    }
  }

  if(noutflow > 0){
    cg_boco_write(index_file,index_base,index_zone,"Outflow nodes",CGNS_ENUMV(BCOutflow),CGNS_ENUMV(PointList),noutflow,outflow_nodes,&index_bc);
  }
  //BCOutflow = 13
  if(nwall > 0){
    cg_boco_write(index_file,index_base,index_zone,"Wall nodes",CGNS_ENUMV(BCWall),CGNS_ENUMV(PointList),nwall,wall_nodes,&index_bc);
  }

  cg_close(index_file);
  return(0);
}
