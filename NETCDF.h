#ifndef NETCDF___
#define NETCDF___
#include"dataIO.h"
#include"dataIOError.h"
#include<string>
#include<map>
#include<netcdf>
#include<vector>
using namespace netCDF;
class NETCDF:public dataIO
{
private:
//for read
    std::string filename;
    NcFile * file;
    char flags;//如果只读打开防止写修改
//for write
        
    
    std::vector<NcDim> filedims;
    std::multimap<std::string,NcVar> fileVars;
    std::string getAttr(NcVarAtt & t);
    std::map<std::string,int> getDims(NcVar & d);
    std::map<std::string,std::string> getAttrs(NcVar & d);
public:
    
    virtual void open(const std::string & filename, const char* flags) ;
    virtual void listVar();
    virtual void creatVar(const std::string & var,struct Data *);
    virtual std::map<std::string,std::string> getVarAttr(const std::string&);
    virtual void setVarAttr(const std::string&,std::map<std::string,std::string> &);
    virtual struct Data * getVar(const std::string & var);
    virtual void putVar(const std::string & var,void * data );
    virtual void close();
    
//add     
    NETCDF();
    void getFileInfo();
    void showVar(netCDF::NcVar & var);
    void showVar(const std::string  & var);
    static void showAttr(std::map<std::string,std::string> &);
    virtual ~NETCDF();
}; 
#endif
