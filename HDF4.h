#ifndef HDF4___
#define HDF4___
#include"dataIO.h"
#include"dataIOError.h"
class HDF4:public dataIO
{
private:
    int sd_id;//hdf文件的打开标志
    std::string filename;
    char flags;//打开的状态
public:
    virtual void open(const std::string & filename, const char* flags) ;
    virtual void listVar();
    virtual void listVar(const std::string & );
    virtual void creatVar(const std::string & var,struct Data *);
    virtual std::map<std::string,std::string> getVarAttr(const std::string&);
    virtual void setVarAttr(const std::string&,std::map<std::string,std::string> &);
    virtual struct Data * getVar(const std::string & var);
    virtual void putVar(const std::string & var,void * data );
    virtual void close();
//add 
    std::string getAttr(int id,int idx,int datatype,int count);
    void showFileinfo();
    struct Data * getVar(const std::string & var,int * edge,int * start,int * stride);
    void putVar(const std::string & var,void * data,int * edge,int * start,int * stride);
    HDF4();
    ~HDF4();
};
#endif
