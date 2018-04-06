#include"NETCDF.h"
#include<netcdf>
#include<iomanip>
#include<algorithm>
#include<sstream>
#include<cstring>
using namespace netCDF;
NETCDF::NETCDF()
{
    filename.clear();
    file=NULL;
}
void NETCDF::open(const std::string& filename, const char* flags)//r read w readwrite c creat
{
    //如果文件已经打开要先关闭
    if(!file->isNull())
    {
        file->close();
        this->filename.clear();
    }
    try
    {
    switch(flags[0])
    {
        case 'r':
            file=new NcFile(filename,NcFile::read);
            break;
        case 'w':
            file=new NcFile(filename,NcFile::write);
            break;
        case 'c':
            file=new NcFile(filename,NcFile::replace);
            break;
        default:
            std::cout<<"unknown flags!!!!"<<std::endl;
    };
    }catch(exceptions::NcException e)
    {
        e.what();
        file->close();
        file=NULL;
    }
    this->flags=flags[0];
    this->filename=filename;
    auto fcoord=file->getCoordVars();
    for(auto att=fcoord.begin();att!=fcoord.end();att++)
    {
        filedims.push_back(att->second.getDim(att->first));
    }
    fileVars=file->getVars();
    
}
void NETCDF::getFileInfo()
{
    std::multimap<std::string,NcGroupAtt> fatt=file->getAtts();
    char buffer[1024];
    for(auto att=fatt.begin();att!=fatt.end();att++)
    {
        std::cout<<"******************************"<<std::endl;
        att->second.getValues(buffer);
        std::cout<<att->first<<":"<<buffer<<std::endl;
        
    }
    std::cout<<"******************************"<<std::endl;
    
    auto fcoord=file->getCoordVars();
    for(auto att=fcoord.begin();att!=fcoord.end();att++)
    {
        auto t=att->second.getDim(att->first);
        std::cout<<t.getName()<<":"<<t.getSize()<<":"<<(t.isUnlimited()?"unlimited":"limited")<<std::endl;
    }
    std::cout<<"******************************"<<std::endl;
}

std::map<std::string, int> NETCDF::getDims(netCDF::NcVar& d)
{
    auto Vdims=d.getDims();
    std::map<std::string, int> r;
    for(auto t=Vdims.begin();t!=Vdims.end();t++)
        r[t->getName()]=t->getSize();
    return r;
}

std::string NETCDF::getAttr(NcVarAtt & nva)
{
    std::string  r;
    std::strstream out;
    size_t len=nva.getAttLength();
    char * buffer=new char[100];
    switch(nva.getType().getId())
    {
        case NC_BYTE:
        case NC_STRING:
        case NC_CHAR:
        case NC_UBYTE:
            nva.getValues(r);
            return r;
            break;
        case NC_USHORT:
            nva.getValues((unsigned short *)buffer);
            for(int i=0;i<len;i++)
                out<<((unsigned short *)buffer)[i]<<" ";
            break;
        case NC_SHORT:
            nva.getValues((short *)buffer);
            for(int i=0;i<len;i++)
                out<<((short *)buffer)[i]<<" ";
            break;
        case NC_INT:
            nva.getValues((int *)buffer);
            for(int i=0;i<len;i++)
                out<<((int *)buffer)[i]<<" ";
            break;
        case NC_UINT:
            nva.getValues((unsigned *)buffer);
            for(int i=0;i<len;i++)
                out<<((unsigned *)buffer)[i]<<" ";
            break;
        case NC_INT64:
            nva.getValues((long *)buffer);
            for(int i=0;i<len;i++)
                out<<((long *)buffer)[i]<<" ";
            break;
        case NC_UINT64:
            nva.getValues((unsigned long *)buffer);
            for(int i=0;i<len;i++)
                out<<((unsigned long *)buffer)[i]<<" ";
            break;
        case NC_FLOAT:
            nva.getValues((float *)buffer);
            for(int i=0;i<len;i++)
                out<<((float *)buffer)[i]<<" ";
            break;
        case NC_DOUBLE:
            nva.getValues((double *)buffer);
            for(int i=0;i<len;i++)
                out<<((double *)buffer)[i]<<" ";
            break;
        default:
            std::cout<<"unknown type!!!!"<<std::endl;      
            
    }
    getline(out,r);
    delete[] buffer;
    return r;
}

std::map<std::string, std::string> NETCDF::getAttrs(netCDF::NcVar& d)
{
    auto Vattr=d.getAtts();
    std::map<std::string, std::string> r;
     for(auto t=Vattr.begin();t!=Vattr.end();t++)
     {
         r[t->first]=getAttr(t->second);
    }
        
    return r;
    
}

void NETCDF::showVar(NcVar & var)
{
    
    std::map<std::string,int> dims=getDims(var);
    std::map<std::string, std::string> attrs=getAttrs(var);
    std::cout<<"******************************"<<std::endl;
    std::cout<<var.getType().getName()<<" "<<var.getName()<<":";
    for(auto t=dims.begin();t!=dims.end();t++)
    {
        std::cout<<t->second<<"("<<t->first<<")"<<"x";
    }   
    std::cout<<std::endl;
    for(auto t=attrs.begin();t!=attrs.end();t++)
    {
        std::cout<<t->first<<":"<<t->second<<std::endl;
    }   
}
void NETCDF::showVar(const std::string & var)
{
    auto t=fileVars.begin();
    for(;t!=fileVars.end();t++)
         if(var.compare(t->first)==0)
         {
             showVar(t->second);
             break;
         }
    if(t==fileVars.end())
        std::cout<<"var:"<<var<<" is invalid showVar"<<std::endl;
    
}

void NETCDF::listVar()
{
   
    for(auto t=fileVars.begin();t!=fileVars.end();t++)
        showVar(t->second);
}

void NETCDF::showAttr(std::map<std::string, std::string>& t)
{
    for(auto s=t.begin();s!=t.end();s++)
        std::cout<<s->first<<":"<<s->second<<std::endl;
}

std::map<std::string,std::string> NETCDF::getVarAttr(const std::string& var)
{
    auto t=fileVars.begin();
    std::map<std::string,std::string> tmp_att;
    for(;t!=fileVars.end();t++)
         if(var.compare(t->first)==0)
         {
             tmp_att=getAttrs(t->second);
             return tmp_att;
         }
    if(t==fileVars.end())
        std::cout<<"var:"<<var<<" is invalid getVarAttr"<<std::endl;
    return tmp_att;
}

struct Data * NETCDF::getVar(const std::string& var)
{
      Data *d =new Data(0);
      auto t=fileVars.begin();
      for(;t!=fileVars.end();t++)
         if(var.compare(t->first)==0)
         {
            std::map<std::string,int> dims=getDims(t->second);
            size_t len=1;
            for(auto s=dims.begin();s!=dims.end();s++)
            {
                d->dims[d->rank]=s->second;
                std::strcpy(d->coordName[d->rank++],s->first.c_str());
                len*=s->second;
            }
            len*=t->second.getType().getSize();
            std::strcpy(d->type,t->second.getType().getName().c_str());
            d->data=new char[len];
            t->second.getVar((void *)d->data);
            break;
         }
    if(t==fileVars.end())
    {
        std::cout<<"var:"<<var<<" is invalid getVar"<<std::endl;
        delete d;
        d=NULL;
    }
    return d;
}



void NETCDF::creatVar(const std::string& var,struct Data * d)//自己写一般写成float类型
{
    if(flags=='r')
    {
        std::cout<<"error: Read Only File"<<std::endl;
        return;
    }
    std::vector<NcDim> dims;
    NcVar data;
    for(int i=0;i<d->rank;i++)
    {
       
            auto t=filedims.begin();
            for(;t!=filedims.end();t++)
                if(t->getName().compare(d->coordName[i])==0)
                {
                    dims.push_back(*t);
                    break;
                }
            if(t==filedims.end())
            {
                NcDim tmp=file->addDim(d->coordName[i],d->dims[i]);
                filedims.push_back(tmp);
                dims.push_back(tmp);
                break;
            }
       
        
    }
    if(std::strcmp("float",d->type)==0)
        data=file->addVar(var,ncFloat,dims);
    else if(std::strcmp("double",d->type)==0)
        data=file->addVar(var,ncDouble,dims);
    else if(std::strcmp("int",d->type)==0)
        data=file->addVar(var,ncInt,dims);
    else if(std::strcmp("char",d->type)==0)
        data=file->addVar(var,ncChar,dims);
    else if(std::strcmp("uint",d->type)==0)
        data=file->addVar(var,ncUint,dims);
    else if(std::strcmp("long",d->type)==0)
        data=file->addVar(var,ncInt64,dims);
    else
    {
        std::cout<<"unknown type"<<std::endl;
        return;
    }
    data.putVar((void *)d->data);
    fileVars=file->getVars();//刷新var
}

void NETCDF::putVar(const std::string& var, void* data)//提供给write使用的快捷版本
{
    auto t=fileVars.begin();
    for(;t!=fileVars.end();t++)
         if(var.compare(t->first)==0)
         {
             t->second.putVar(data);
             break;
         }
    if(t==fileVars.end())
        std::cout<<"var:"<<var<<" is invalid putVar"<<std::endl;
}

void NETCDF::setVarAttr(const std::string& var,std::map<std::string,std::string> & attrs)
{
    auto t=fileVars.begin();
    for(;t!=fileVars.end();t++)
         if(var.compare(t->first)==0)
         {
             for(auto attr=attrs.begin();attr!=attrs.end();attr++)
             {
                 t->second.putAtt(attr->first,ncChar,attr->second.length(),attr->second.c_str());
            }
             break;
         }
    if(t==fileVars.end())
        std::cout<<"var:"<<var<<" is invalid setVarAttr"<<std::endl;
}

void NETCDF::close()
{
    file->close();
}
NETCDF::~NETCDF()
{
    if(file)
    {
        close();
        delete file;
    }
    std::cout<<"NETCDF释放"<<std::endl;
}


