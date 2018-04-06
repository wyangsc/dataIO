#include"HDF4.h"
#include<hdf/hdf.h>
#include<hdf/mfhdf.h>
#include<sstream>
#include<cstring>
HDF4::HDF4()
{
    sd_id=0;
}
void HDF4::open(const std::string& filename, const char* flags)
{
    if(sd_id!=0)
        SDend(sd_id);
    switch(flags[0])
    {
        case 'r':
            sd_id=SDstart(filename.c_str(),DFACC_RDONLY);
            break;
        case 'w':
            sd_id=SDstart(filename.c_str(),DFACC_RDWR);
            break;
        case 'c':
            sd_id=SDstart(filename.c_str(),DFACC_ALL);
            break;
        default:
            std::cout<<"unknown flags"<<std::endl;
            return;
    }
    if(sd_id<0)
    {
        std::cout<<filename<<"open failed"<<std::endl;
        sd_id=0;
        return;
    }
    this->flags=flags[0];
    this->filename=filename;
    
}

std::string HDF4::getAttr(int id, int idx,int datatype,int count)
{
        std::string r;
        std::stringstream buffer(r);
        char * fileattr_str;
        short * fileattr_short;
        int * fileattr_int;
        long * fileattr_long;
        float * fileattr_float;
        double * fileattr_double;
        switch(datatype)
        {
            case DFNT_CHAR:
                 fileattr_str=new char[count];
                 SDreadattr(id,idx,fileattr_str);
                 buffer<<fileattr_str<<std::endl;
                 break;
            case DFNT_INT16:
                 fileattr_short=new short[count];
                 SDreadattr(id,idx,fileattr_short);
                 for(int i=0;i<count;i++)
                     buffer<<fileattr_short[i]<<" ";
                 buffer<<std::endl;
                 break;
            case DFNT_INT32:
                 fileattr_int=new int[count];
                 SDreadattr(id,idx,fileattr_int);
                 for(int i=0;i<count;i++)
                     buffer<<fileattr_int[i]<<" ";
                 buffer<<std::endl;
                 break;
            case DFNT_INT64:
                 fileattr_long=new long[count];
                 SDreadattr(id,idx,fileattr_long);
                 for(int i=0;i<count;i++)
                     buffer<<fileattr_long[i]<<" ";
                 buffer<<std::endl;
                 break;
            case DFNT_FLOAT:
                 fileattr_float=new float[count];
                 SDreadattr(id,idx,fileattr_float);
                 for(int i=0;i<count;i++)
                     buffer<<fileattr_float[i]<<" ";
                 buffer<<std::endl;
                 break;
            case DFNT_DOUBLE:
                 fileattr_double=new double[count];
                 SDreadattr(id,idx,fileattr_double);
                 for(int i=0;i<count;i++)
                    buffer<<fileattr_double[i]<<" ";
                 buffer<<std::endl;
                 break;
            default:
                std::cout<<"unknown Type"<<std::endl;
        }
        r=buffer.str();
        return r;
}

void HDF4::showFileinfo()
{
    int32 dataset,attrs;
    if(SDfileinfo(sd_id,&dataset,&attrs)<0)
    {
        std::cout<<filename<<" invalid"<<std::endl;
        return;
    }
    for(int i=0;i<attrs;i++)
    {
        char name[20];
        int datatype;
        int count;
        SDattrinfo(sd_id,i,name,&datatype,&count);
        std::cout<<name<<std::endl<<getAttr(sd_id,i,datatype,count);
    }
}




void HDF4::listVar()
{
    int32 dataset,attrs;
    if(SDfileinfo(sd_id,&dataset,&attrs)<0)
    {
        std::cout<<filename<<" invalid"<<std::endl;
        return;
    }
     for(int i=0;i<dataset;i++)
    {
        int32 datatype,attrs;
        char name[20];
        int dims[H4_MAX_VAR_DIMS];
        int rank;
        int sds_id =SDselect(sd_id, i);
        SDgetinfo(sds_id,name,&rank,dims,&datatype,&attrs);
        SDendaccess(sds_id);
        std::cout<<"*******************"<<std::endl;
        listVar(name);
    }
}
void HDF4::listVar(const std::string& var)
{
    std::map<std::string, std::string> attrs=getVarAttr(var);
    std::cout<<var<<std::endl;
    for(auto attr=attrs.begin();attr!=attrs.end();attr++)
        std::cout<<attr->first<<":"<<attr->second<<std::endl;
}

std::map<std::string, std::string> HDF4::getVarAttr(const std::string& var)
{
    std::map<std::string, std::string> r;
    int sds_index=SDnametoindex(sd_id,var.c_str());
    if(sds_index==FAIL)
    {
        std::cout<<var<<"invalid"<<std::endl;
        return r;
    }
    int sds=SDselect(sd_id,sds_index);
    int32 datatype,attrs;
    char name[20];
    int dims[H4_MAX_VAR_DIMS];
    int rank;
    SDgetinfo(sds,name,&rank,dims,&datatype,&attrs);
    //MetaInfo
    r["rank"]=std::to_string(rank);
    for(int i=0;i<rank;i++)
        r["dims"]+=std::to_string(dims[i]),r["dims"]+=" ";
    switch(datatype)
    {
        case DFNT_CHAR:
            r["type"]="char";
            break;
        case DFNT_INT16:
             r["type"]="short";
              break;
        case DFNT_INT32:
             r["type"]="int";
              break;
        case DFNT_INT64:
             r["type"]="long";
              break;
        case DFNT_FLOAT:
             r["type"]="float";
              break;
        case DFNT_DOUBLE:
             r["type"]="double";
             break;
        default:
            std::cout<<"unknown Type"<<std::endl;
    }
    //attrinfo
    for(int i=0;i<attrs;i++)
    {
        char name[20];
        int datatype;
        int count;
        SDattrinfo(sds,i,name,&datatype,&count);
        r[name]=getAttr(sds,i,datatype,count);
    }
    SDendaccess(sds);
    return r;
}
struct Data * HDF4::getVar(const std::string& var)
{
    int sds_index=SDnametoindex(sd_id,var.c_str());
    if(sds_index==FAIL)
    {
        std::cout<<var<<"is invalid"<<std::endl;
        return NULL;
    }
    int sds=SDselect(sd_id,sds_index);
    int32 datatype,attrs;
    char name[20];
    int dims[H4_MAX_VAR_DIMS];
    int rank;
    SDgetinfo(sds,name,&rank,dims,&datatype,&attrs);
    SDendaccess(sds);
    int start[10]={0};
    int stride[10]={1,1,1,1,1,1,1,1,1};
    return getVar(var,dims,start,stride);
}
struct Data * HDF4::getVar(const std::string& var,int* edge,int* start, int* stride)
{
    int sds_index=SDnametoindex(sd_id,var.c_str());
    if(sds_index==FAIL)
    {
        std::cout<<var<<"is invalid"<<std::endl;
        return NULL;
    }
    Data * d =new Data(0);
    int sds=SDselect(sd_id,sds_index);
    int32 datatype,attrs;
    char name[20];
    int dims[H4_MAX_VAR_DIMS];
    int rank;
    SDgetinfo(sds,name,&rank,dims,&datatype,&attrs);
    //判断选取的位置是不是越界
    int len=1;
    for(int i=0;i<rank;i++)
    {
        if(edge[i]*stride[i]+start[i]>dims[i])
        {
            std::cout<<"dims error"<<std::endl;
            return NULL;
        }
        d->dims[i]=edge[i];
        len*=edge[i];
    }
    d->rank=rank;
    //读出数组的类型,设置读出数据堆空间
     switch(datatype)
        {
            case DFNT_CHAR:
                d->data=new char[len];
                std::strcpy(d->type,"char"); 
                break;
            case DFNT_INT16:
                d->data=new char[len*sizeof(short)];
                std::strcpy(d->type,"short"); 
                break;
            case DFNT_INT32:
                d->data=new char[len*sizeof(int)];
                std::strcpy(d->type,"int"); 
                break;
            case DFNT_INT64:
                d->data=new char[len*sizeof(long)];
                std::strcpy(d->type,"long"); 
                break;
            case DFNT_FLOAT:
                d->data=new char[len*sizeof(float)];
                std::strcpy(d->type,"float"); 
                break; 
            case DFNT_DOUBLE:
                d->data=new char[len*sizeof(double)];
                std::strcpy(d->type,"double"); 
                break;
            default:
                std::cout<<"unknown Type"<<std::endl;
        }
    if(SDreaddata(sds,start,stride,edge,d->data)<0)
    {
        delete d;
        std::cout<<"read error"<<std::endl;
        SDendaccess(sds);
        return NULL;
    }
    SDendaccess(sds);
    return d;
}




void HDF4::creatVar(const std::string& var, struct Data* d)
{
    if(flags=='r')
    {
        std::cout<<filename<<"IS READ ONLY"<<std::endl;
        return;
    }
    if(SDnametoindex(sd_id,var.c_str())<0&&flags=='w')
    {
        std::cout<<var<<" in "<<filename<<" is not EXIST"<<std::endl;
        return;
    }
    if(flags=='c');
        
    int dims[H4_MAX_VAR_DIMS];
    int stat;
    for(int i=0;i<d->rank;i++)
        dims[i]=d->dims[i];//兼容hdf5
    if(std::strcmp(d->type,"float")==0)
        stat=SDcreate(sd_id,var.c_str(),DFNT_FLOAT,d->rank,dims);
    else if(std::strcmp(d->type,"int")==0)
        stat=SDcreate(sd_id,var.c_str(),DFNT_INT32,d->rank,dims);
    else if(std::strcmp(d->type,"double")==0)
        stat=SDcreate(sd_id,var.c_str(),DFNT_DOUBLE,d->rank,dims);
    else if(std::strcmp(d->type,"char")==0)
        stat=SDcreate(sd_id,var.c_str(),DFNT_CHAR,d->rank,dims);
    else if(std::strcmp(d->type,"long")==0)
        stat=SDcreate(sd_id,var.c_str(),DFNT_INT64,d->rank,dims);
    else if(std::strcmp(d->type,"short")==0)
        stat=SDcreate(sd_id,var.c_str(),DFNT_INT16,d->rank,dims);
    else
    {
        std::cout<<"unknown Type"<<std::endl;
    }
    putVar(var,d->data);
}
void HDF4::putVar(const std::string& var, void* data)
{
    if(flags=='r')
    {
        std::cout<<filename<<"IS READ ONLY"<<std::endl;
        return;
    }
    int sds_index=SDnametoindex(sd_id,var.c_str());
    if(sds_index==FAIL)
    {
        std::cout<<var<<"is invalid"<<std::endl;
        return;
    }
    int sds=SDselect(sd_id,sds_index);
    int32 datatype,attrs;
    char name[20];
    int dims[H4_MAX_VAR_DIMS];
    int start[H4_MAX_VAR_DIMS]={0};
    int rank;
    SDgetinfo(sds,name,&rank,dims,&datatype,&attrs);
    if(SDwritedata(sds,start,NULL,dims,data)<0)
    {
        std::cout<<"write error"<<std::endl;
        SDendaccess(sds);
        return;
    }
    SDendaccess(sds);
}
void HDF4::putVar(const std::string& var, void* data, int* edge, int* start, int* stride)
{
    if(flags=='r')
    {
        std::cout<<filename<<"IS READ ONLY"<<std::endl;
        return;
    }
    int sds_index=SDnametoindex(sd_id,var.c_str());
    if(sds_index==FAIL)
    {
        std::cout<<var<<"is invalid"<<std::endl;
        return;
    }
    int sds=SDselect(sd_id,sds_index);
    int32 datatype,attrs;
    char name[20];
    int dims[H4_MAX_VAR_DIMS];
    int rank;
    SDgetinfo(sds,name,&rank,dims,&datatype,&attrs);
    //判断选取的位置是不是越界
    for(int i=0;i<rank;i++)
        if(edge[i]*stride[i]+start[i]>dims[i])
        {
            std::cout<<"dims error"<<std::endl;
            return ;
        }
    
    if(SDwritedata(sds,start,stride,edge,data)<0)
    {
        std::cout<<"write error"<<std::endl;
        SDendaccess(sds);
        return;
    }
    SDendaccess(sds);
}



void HDF4::setVarAttr(const std::string& var, std::map<std::string, std::string>& attrs)
{
    int sds_index=SDnametoindex(sd_id,var.c_str());
    if(sds_index==FAIL)
    {
        std::cout<<var<<"is invalid"<<std::endl;
        return;
    }
    int sds=SDselect(sd_id,sds_index);
    for(auto attr:attrs)
    {
        SDsetattr(sds,attr.first.c_str(),DFNT_CHAR,attr.second.size(),attr.second.c_str());
    }
    
    SDendaccess(sds);
    
    
}
void HDF4::close()
{
    if(sd_id!=0)
        SDend(sd_id);
}
HDF4::~HDF4()
{
    if(sd_id!=0)
        SDend(sd_id);
}
