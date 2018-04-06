#include<iostream>
#include"HDF5.h"
#include<vector>
#include<cstdlib>
#include<cstring>
using namespace H5;
HDF5::HDF5()
{
    file=NULL;
    filename.clear();
    
}

 void HDF5::open(const std::string& filename, const char* flags)//r,w,c
{
    if(file!=NULL)
        close();
    switch(flags[0])
    {
        case 'r':
            file=new H5File(filename,H5F_ACC_RDONLY);
            break;
        case 'w':
            file=new H5File(filename,H5F_ACC_RDWR);
            break;
        case 'c':
            
            file=new H5File(filename, H5F_ACC_TRUNC);
            break;
        default:
            std::cout<<"unknown flags"<<std::endl;
            return;
    }
    this->flags=flags[0];
    this->filename=filename;
    
}

std::map<std::string, std::string> HDF5::listAttrInfo(const H5::H5Object& obj)
{
    std::map<std::string, std::string> r;
    int n=obj.getNumAttrs();
    std::string buffer;
    float f;
    double d;
    int it;
    short s;
    char c;
    long l;
    unsigned short us;
    unsigned int ui;
    unsigned long ul;
    for(int i=0;i<n;i++)
    {
      Attribute a=obj.openAttribute(i);
      if(a.getDataType()==PredType::NATIVE_FLOAT)
      {
          a.read(a.getDataType(),&f);
          buffer=std::to_string(f);
      }
      else if(a.getDataType()==PredType::NATIVE_CHAR)
      {
          a.read(a.getDataType(),buffer);
    }
      else if(a.getDataType()==PredType::NATIVE_DOUBLE)
      {
          a.read(a.getDataType(),&d);
          buffer=std::to_string(d);
    }
      else if(a.getDataType()==PredType::NATIVE_INT32)
      {
          a.read(a.getDataType(),&it);
          buffer=std::to_string(it);
    }
      else if(a.getDataType()==PredType::NATIVE_INT64)
      {
          a.read(a.getDataType(),&l);
          buffer=std::to_string(l);
    }
      else if(a.getDataType()==PredType::NATIVE_INT16)
      {
          a.read(a.getDataType(),&s);
          buffer=std::to_string(s);
    }
    else if(a.getDataType()==PredType::NATIVE_UINT16)
    {
          a.read(a.getDataType(),&us);
          buffer=std::to_string(us);
    }
    else if(a.getDataType()==PredType::NATIVE_UINT32)
    {
          a.read(a.getDataType(),&ui);
          buffer=std::to_string(ui);
    }
    else if(a.getDataType()==PredType::NATIVE_UINT64)
    {
          a.read(a.getDataType(),&ul);
          buffer=std::to_string(ul);
    }
    else
    {
        a.read(a.getDataType(),buffer);
    }
      
      r[a.getName()]=buffer;
    }
    return r;
}

std::map<std::string, std::string> HDF5::listMetaInfo(const  DataSet& obj)
{
    std::map<std::string, std::string> r;
    //dataType
    H5T_class_t tmp=obj.getTypeClass();
    IntType it;
    FloatType ft;
    StrType st;
    std::string order;
    switch(tmp)
    {
        case H5T_INTEGER:
            it=obj.getIntType();
            it.getOrder(order);
            r["type"]="int";
            r["size"]=std::to_string(it.getSize());
            r["order"]=order;
            break;
        case H5T_FLOAT:
            ft=obj.getFloatType();
            ft.getOrder(order);
            r["type"]="float";
            r["size"]=std::to_string(ft.getSize());
            r["order"]=order;
            break;
        case H5T_STRING:
            st=obj.getStrType();
            st.getOrder(order);
            r["type"]="string";
            r["size"]=std::to_string(st.getSize());
            r["order"]=order;
            break;
        case H5T_TIME:
        default:
            std::cout<<"unknown type!!!!!Meta"<<std::endl;
    }
    //dataSpace
    DataSpace ds=obj.getSpace();
    hsize_t dimsize[H5S_MAX_RANK];
    int rank=ds.getSimpleExtentDims(dimsize,NULL);
    r["rank"]=std::to_string(rank);
    r["dims"]=" (";
    for(int i=0;i<rank;i++)
        r["dims"]+=std::to_string(dimsize[i]),r["dims"]+=",";
   r["dims"]+=")";
   return r;
    
}

void HDF5::listOBJInfo(const H5::Group& g,const std::string & var)
{
   int n=g.getNumObjs();
   Group p;
   DataSet d;
     for(int i=0;i<n;i++)
  {
    
    switch(g.getObjTypeByIdx(i))
    {
        case H5G_GROUP:
            p=g.openGroup(g.getObjnameByIdx(i));
           // listAttrInfo(p);
            listOBJInfo(p,var);
            break;
        case H5G_DATASET:
            if(var.size()==0||var.compare(g.getObjnameByIdx(i))==0)
            {
                std::cout<<"*******************************"<<std::endl;
                std::cout<<g.getObjnameByIdx(i)<<std::endl;
                d= g.openDataSet(g.getObjnameByIdx(i));
                listVar(d);
            }
            break;
        case  H5G_TYPE:
        case H5G_LINK:
        case H5G_UDLINK:  
        default:
            ;
    }
    
    }

}
H5::DataSet * HDF5::findDataSet(const H5::Group& g, const std::string& var)
{
   int n=g.getNumObjs();
   Group p;
   DataSet d;
   DataSet * tmp;
   for(int i=0;i<n;i++)
    {
    
    switch(g.getObjTypeByIdx(i))
    {
        case H5G_GROUP:
            p=g.openGroup(g.getObjnameByIdx(i));
           // listAttrInfo(p);
            tmp=findDataSet(p,var);
            if(tmp!=NULL)
                return tmp;
            break;
        case H5G_DATASET:
            if(var.compare(g.getObjnameByIdx(i))==0)
                return new DataSet(g.openDataSet(g.getObjnameByIdx(i)));
            break;
        default:
            ;
    }
    
    }
    return NULL;
}

void HDF5::listVar(H5::DataSet& ds)
{
    std::map<std::string, std::string> r;
    r=listAttrInfo(ds);
    std::map<std::string, std::string> t=listMetaInfo(ds);
    for(auto kv=t.begin();kv!=t.end();kv++)
        r[kv->first]=kv->second;
    for(auto kv=r.begin();kv!=r.end();kv++)
            std::cout<<kv->first<<"="<<kv->second<<std::endl;
}

void HDF5::listVar(const std::string& var)
{
   std::map<std::string, std::string> t=getVarAttr(var);
   for(auto kv=t.begin();kv!=t.end();kv++)
            std::cout<<kv->first<<"="<<kv->second<<std::endl;
}

void HDF5::listVar()
{
    std::string tmp;
    tmp.resize(0);
    listOBJInfo(file->openGroup("/"),tmp);
}

std::map<std::string, std::string> HDF5::getVarAttr(const std::string& var)
{
     std::map<std::string, std::string> r;
     //区分 相对路径和绝对路径
     if(var.find('/')!=std::string::npos)
     {
     
     try
    {
        DataSet ds=file->openDataSet(var);
        r=listAttrInfo(ds);
        std::map<std::string, std::string> t=listMetaInfo(ds);
        for(auto kv=t.begin();kv!=t.end();kv++)
            r[kv->first]=kv->second;
    }catch(...)
    {
        std::cout<<var<<" open failed!!!"<<std::endl;
        throw;
    }
     }
    else
     {
         DataSet * d=findDataSet(file->openGroup("/"),var);
         if(d==NULL)
         {
             std::cout<<var<<"is not found"<<std::endl;
             return r;
        }
         r=listAttrInfo(*d);
         std::map<std::string, std::string> t=listMetaInfo(*d);
         for(auto kv=t.begin();kv!=t.end();kv++)
                r[kv->first]=kv->second;
         delete d;
    }   
    return r;
}
std::vector<std::string> HDF5::split(const std::string & tmp ,char sp)
{
        std::vector<std::string> s;
        int begin=0,end=0,strsize=tmp.size();
        for(end=0;end<strsize;end++)
            {
            if(tmp[end]==sp&&end!=begin)
                {
                    s.push_back(tmp.substr(begin,end-begin));
                    begin=end+1;
                }

            }
        if(tmp[end]!=sp)
        s.push_back(tmp.substr(begin,end-begin+1));
    return s;
}
struct Data * HDF5::getVar(const std::string& var)
{
   
   
    int rank;
    hsize_t dims[10];
    hsize_t offset[10]={0};
    hsize_t stride[10]={1,1,1,1,1,1,1,1,1,1};
    DataSet ds;
    if(var.find('/')!=std::string::npos)
        ds=file->openDataSet(var);
    else
    {
        DataSet * d=findDataSet( file->openGroup("/"),var);
        if(d==NULL)
         {
             std::cout<<var<<"is not found"<<std::endl;
             return NULL;
        }
        ds=*d;
        delete d;
    };
    DataSpace dp=ds.getSpace();
    rank=dp.getSimpleExtentDims(dims);
    return  getVar(var,dims,offset,stride);
}

void HDF5::putVar(const std::string& var, void* data) //简单修改一个数据集
{
   
    DataSet ds;
    if(var.find('/')!=std::string::npos)
        ds=file->openDataSet(var);
    else
    {
        DataSet * d=findDataSet( file->openGroup("/"),var);
        if(d==NULL)
         {
             std::cout<<var<<"is not found"<<std::endl;
             return ;
        }
        ds=*d;
        delete d;
    }
    DataSpace space=ds.getSpace();
    hsize_t dims[10];
    hsize_t offset[10]={0};
    hsize_t stride[10]={1,1,1,1,1,1,1,1,1,1};
    space.getSimpleExtentDims(dims);
    putVar(var,data,dims,offset,stride);
    return;
    
}

void HDF5::creatVar(const std::string& var, struct Data* d)
{
       
        if(flags=='r')
        {
            std::cout<<"file Read ONLY !!!!"<<std::endl;
            return;
        }
        //检查数据是否存在
        if(file->attrExists(var))
        {
            if(flags=='w')
            {
                std::cout<<var<<" already exists"<<std::endl;
                return;
            }
            file->unlink(var);
        }
        //设置数据空间结构
        DataSpace ds(d->rank,d->dims);
        //设置数据的类型信息
        DataType dt;
        if(std::strcmp(d->type,"int")==0)
            dt=PredType::NATIVE_INT;
        else if(std::strcmp(d->type,"float")==0)
            dt=PredType::NATIVE_FLOAT;
        else if(std::strcmp(d->type,"double")==0)
            dt=PredType::NATIVE_DOUBLE;
        else if(std::strcmp(d->type,"long")==0)
            dt=PredType::NATIVE_INT64;
        else if(std::strcmp(d->type,"short")==0)
            dt=PredType::NATIVE_INT16;
        else if(std::strcmp(d->type,"char")==0)
            dt=PredType::NATIVE_INT8;
        else
        {
            std::cout<<"Data Type unknown"<<std::endl;
        }
        //创建变量
        DataSet dataset=file->createDataSet(var,dt,ds);
        //放入数据
        putVar(var,(void *)(d->data));
        
}

void HDF5::setVarAttr(const std::string& var, std::map<std::string, std::string>& attrs)
{
        DataSpace attr_dataspace(H5S_SCALAR);
        StrType strdatatype(PredType::C_S1, 256);
        
        DataSet ds;
        if(var.find('/')!=std::string::npos)
            ds=file->openDataSet(var);
    else
    {
        DataSet * d=findDataSet( file->openGroup("/"),var);
        if(d==NULL)
         {
             std::cout<<var<<"is not found"<<std::endl;
             return ;
        }
        ds=*d;
        delete d;
    }
        for(auto attr=attrs.begin();attr!=attrs.end();attr++)
        {
            Attribute myatt_in = ds.createAttribute(attr->first, strdatatype, attr_dataspace);
            myatt_in.write(strdatatype,attr->second);
        }
}
void HDF5::addGroup(const std::string& g)
{
    if(file->attrExists(g))
        return;
    file->createGroup(g);
}
void HDF5::deleteGroup(const std::string& g)
{
    if(!file->attrExists(g))
        return;
    file->unlink(g);
}

void HDF5::deleteDataSet(const std::string& d)
{
//     if(!file->attrExists(d))
//         return;
    file->unlink(d);
}

struct Data * HDF5::getVar(const std::string& var, hsize_t* count, hsize_t* offset, hsize_t* stride)
{
    struct Data * d=new Data(0);//自己申请内存自己释放
    DataSet ds;
    if(var.find('/')!=std::string::npos)
        ds=file->openDataSet(var);
    else
    {
        DataSet * d=findDataSet( file->openGroup("/"),var);
        if(d==NULL)
         {
             std::cout<<var<<"is not found"<<std::endl;
             return NULL;
        }
        ds=*d;
        delete d;
    }
    DataSpace dp=ds.getSpace();
    
    std::map<std::string, std::string> r=getVarAttr(var);
    //dims size
    int datasize=1;
    d->rank=dp.getSimpleExtentDims(d->dims);
    //测试输入的尺度是否合适
    for(int i=0;i<d->rank;i++)
    {
        if(count[i]*stride[i]+offset[i]>d->dims[i])
        {
            std::cout<<"dims error!!!!"<<std::endl;
            delete d;
            return NULL;
        }
        d->dims[i]=count[i];
        datasize*=d->dims[i];
    }
        
    
    //type getvar
    int ts=std::atoi(r["size"].c_str());
    datasize*=ts;
    d->data=new char[datasize];
    //设置文件中读取读取大小和偏移
    
    dp.selectHyperslab(H5S_SELECT_SET,d->dims,offset,stride);
    
    //设置在内存中的放置
    hsize_t moffset[7]={0};
    DataSpace memspace(d->rank,d->dims);
    memspace.selectHyperslab(H5S_SELECT_SET,d->dims,moffset);
    
    if(r["type"].compare("int")==0)
    {
        if(ts==4)
        {
            std::strcpy(d->type,"int");
            ds.read(d->data,PredType::NATIVE_INT,memspace,dp);
        }
        else if(ts==2)
        {
            std::strcpy(d->type,"short");
            ds.read(d->data,PredType::NATIVE_INT16,memspace,dp);
        }
        else if(ts==8)
        {
            std::strcpy(d->type,"long");
            ds.read(d->data,PredType::NATIVE_INT64,memspace,dp);
        }
        else if(ts==1)
        {
            std::strcpy(d->type,"char");
            ds.read(d->data,PredType::NATIVE_INT8,memspace,dp);
        }
        else
        {
            std::cout<<"unknown data Type"<<std::endl;
            delete d;
            return NULL;
        }
    }
    else if(r["type"].compare("float")==0)
    {
        if(ts==4)
        {
            std::strcpy(d->type,"float");
            ds.read(d->data,PredType::NATIVE_FLOAT,memspace,dp);
        }
        else if(ts==8)
        {
            std::strcpy(d->type,"double");
            ds.read(d->data,PredType::NATIVE_DOUBLE,memspace,dp);
        }
        else
        {
            std::cout<<"unknown data Type"<<std::endl;
            delete d;
            return NULL;
        }
    }
    else
    {
        std::cout<<r["type"]<<"unknown!!!!"<<std::endl;
        delete d;
        return NULL;
        
    }
    return d;
}

void HDF5::putVar(const std::string& var, void* data, hsize_t* count, hsize_t* offset, hsize_t* stride)
{
      if(flags=='r')
    {
            std::cout<<"file Read ONLY !!!!"<<std::endl;
            return;
    }
    //获取var的文件信息
    DataSet ds;
    if(var.find('/')!=std::string::npos)
        ds=file->openDataSet(var);
    else
    {
        DataSet * d=findDataSet( file->openGroup("/"),var);
        if(d==NULL)
         {
             std::cout<<var<<"is not found"<<std::endl;
             return;
        }
        ds=*d;
        delete d;
    }
    DataSpace space=ds.getSpace();
    hsize_t dims[10];
    int rank=space.getSimpleExtentDims(dims);
    
    //判断写入区域是否合法
     for(int i=0;i<rank;i++)
    {
        if(count[i]*stride[i]+offset[i]>dims[i])
        {
            std::cout<<"dims error!!!!"<<std::endl;
            return;
        }
    }
        
    space.selectHyperslab(H5S_SELECT_SET,count,offset,stride);
    //mem 
    DataSpace mem(rank,count);
    ds.write(data,ds.getDataType(),mem,space);
    return;
    
}



void HDF5::close()
{
    file->close();
    file=NULL;
    filename.clear();
}
HDF5::~HDF5()
{
    if(file!=NULL)
        file->close();
    
}
