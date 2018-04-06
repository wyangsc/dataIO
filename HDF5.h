#ifndef HDF5___
#define HDF5___
#include"dataIO.h"
#include"dataIOError.h"
#include<H5Cpp.h>
#include<vector>
using namespace H5;
class HDF5:public dataIO
{
private:
   //  filename
    H5std_string filename;
    H5File * file;
    Group * g;
    char flags;
    std::vector<std::string> split(const std::string & tmp ,char sp);
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
    std::map<std::string, std::string> listAttrInfo(const H5Object & );
    std::map<std::string, std::string> listMetaInfo(const DataSet & obj);
    void listOBJInfo(const Group &,const std::string & var);
    DataSet * findDataSet(const Group &,const std::string & var);
    void listVar(const std::string &);
    void listVar(DataSet &);
    //add group
    void addGroup(const std::string & );
    //delete group
    void deleteGroup(const std::string &);
    void deleteDataSet(const std::string &);
    
    //void ls()
    //void cd(const std::string & path);
    virtual void putVar(const std::string & var,void * data,hsize_t * count,hsize_t * offset,hsize_t * stride );
    virtual struct Data * getVar(const std::string & var,hsize_t * count,hsize_t * offset,hsize_t * stride);
    HDF5();
    virtual ~HDF5();
};
#endif
