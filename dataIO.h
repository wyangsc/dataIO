#ifndef DATAIO 
#define DATAIO
#include<string>
#include<map>
#include<iostream>
#include"dataIOError.h"
//#include"HDF4.h"
//#include"HDF5.h"
//#include"HDFEOS2.h"
//#include"HDFEOS5.h"
//#include"Grib1.h"
//#include"Grib2.h"
//#include"NETCDF.h"

struct Data
{
    char * data;
    unsigned long long dims[10];//SD_UNLIMITED HDF unlimit H5S_UNLIMITED HDF5 unlimited
    int rank;
    char type[10];
    char coordName[10][10];
    char units[10];
    char flags;//是不是需要我帮你释放
    Data(char s,char * d=NULL){flags=s,data=d,rank=0,units[0]=0;};
    ~Data()
    {
        if(data!=NULL&&flags==0)
            delete [] data;
        std::cout<<"Data释放"<<std::endl;
    };
};
class dataIO
{
public:

    virtual void open(const std::string & filename,const char *) =0;
    virtual void listVar()=0;
    virtual void creatVar(const std::string & var,struct Data *)=0;
    virtual std::map<std::string,std::string> getVarAttr(const std::string&)=0;
    virtual void setVarAttr(const std::string&,std::map<std::string,std::string> &)=0;
    virtual struct Data * getVar(const std::string & var) =0;
    virtual void putVar(const std::string & var,void * data ) =0;
    virtual void close()=0;
    virtual ~dataIO(){};
    //fileIO实现按后缀名识别文件
//     static dataIO * fileIO(const std::string & filename);
   
};
// static dataIO * dataIO::fileIO(const std::string & filename)
//  {
//         std::string suffixStr = filename.substr(filename.find_last_of('.') + 1);
//         if(suffixStr.compare("grb")==0||suffixStr.compare("grib")==0)
//             return new Grib1(filename);
//         else if(suffixStr.compare("grb2")==0||suffixStr.compare("grib2")==0)
//             return new Grib2(filename);
//         else if (suffixStr.compare("hdf")==0||suffixStr.compare("HDF")==0)
//             return new HDF4(filename);
//         else if (suffixStr.compare("hdf5")==0||suffixStr.compare("HDF5")==0)
//             return new HDF5(filename);
//         else if (suffixStr.compare("he")==0||suffixStr.compare("HE")==0)
//             return new HDFEOS2(filename);
//         else if (suffixStr.compare("he5")==0||suffixStr.compare("HE5")==0)
//             return new HDFEOS5(filename);
//         else if (suffixStr.compare("nc")==0||suffixStr.compare("nc4")==0)
//             return new NETCDF(filename);
//         else
//         {
//             std::cout<<"unknown"<<endl;
//             return NULL;
//         }
//     }
#endif
