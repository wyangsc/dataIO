#include<iostream>
#include"NETCDF.h"
#include<fstream>
#include<gsl/gsl_cblas.h>
#include<cstring>
int main()
{
  float * data=new float[593*6];
  float lon[593];
  float lat[593];
  float station[593];
  float time[6]={1,2,3,4,5,6};
  std::ifstream infile("DBDQ_hjp1.txt",std::ios_base::in);
  std::ifstream dfile("sp_save.40",std::ios_base::in);
  float t;
  for(int i=0;i<593;i++)
  {
      infile>>lon[i]>>lat[i];
      dfile>>t;
      for(int j=0;j<6;j++)
          dfile>>data[i*6+j];
      for(int j=0;j<54;j++)
          infile>>t;
      station[i]=i+1;
  }  
  NETCDF nc;
  nc.open("eof.nc","c");
  Data * nclon =new Data(1);
  std::strcpy(nclon->coordName[0],"lon");
  nclon->dims[0]=593;
  nclon->rank=1;
  std::strcpy(nclon->type,"float");
  nclon->data=(char *)lon;
  
  Data * nclat =new Data(1);
  std::strcpy(nclat->coordName[0],"lat");
  nclat->dims[0]=593;
  nclat->rank=1;
  std::strcpy(nclat->type,"float");
  nclat->data=(char *)lat;
  
  Data * ncx =new Data(1);
  std::strcpy(ncx->coordName[0],"station");
  ncx->dims[0]=593;
  ncx->rank=1;
  std::strcpy(ncx->type,"float");
  ncx->data=(char *)station;
  
  Data * ncy =new Data(1);
  std::strcpy(ncy->coordName[0],"time");
  ncy->dims[0]=6;
  ncy->rank=1;
  std::strcpy(ncy->type,"float");
  ncy->data=(char *)time;
  
  
  Data * ncdata =new Data(1);
  std::strcpy(ncdata->coordName[0],"station");
  std::strcpy(ncdata->coordName[1],"time");
  ncdata->dims[0]=593;
  ncdata->dims[1]=6;
  ncdata->rank=2;
  std::strcpy(ncdata->type,"float");
  ncdata->data=(char *)data;
  
  nc.creatVar("time",ncy);
  nc.creatVar("station",ncx);
  nc.creatVar("lon",nclon);
  nc.creatVar("lat",nclat);
  nc.creatVar("data",ncdata);
  for(int i=0;i<593;i++)
      std::cout<<lat[i]<<std::endl;
  std::cout<<"OK"<<std::endl;
}
