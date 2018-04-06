#include "grib.h"
#include<iostream>
#include<grib_api.h>
#include<string.h>
using namespace std;
grib::grib(const char * file,const char * flag)
{
    fp=fopen(file,flag);
    if(!fp)
    {
       cout<<"ERROR: unable to open file "<<file<<endl;
       exit(-1);
    }
    h=NULL;
    values=NULL;
}
int grib::ListGribInfo()
{
    char name[1024];
    char buffer[10240];
    size_t len=10240;
    int err;
    int count;
    unsigned long key_iterator_filter_flags = GRIB_KEYS_ITERATOR_ALL_KEYS;
    char * name_space=NULL;
    err=grib_count_in_file(NULL,fp,&count);
    if(err)
    {
        cout<<grib_get_error_message(err);
        return -1;
    }
    grib_handle * gh[count];

    for(int i=0;i<count;i++)
    {
        gh[i]=grib_handle_new_from_file(0,fp,&err);
        if(err)
        {
            cout<<grib_get_error_message(err);
            return -1;
        }
        cout<<"*********************"<<i+1<<endl;

        grib_keys_iterator * it=grib_keys_iterator_new(gh[i],key_iterator_filter_flags,name_space);
        while(grib_keys_iterator_next(it))
           {
            const char * name=grib_keys_iterator_get_name(it);
            len=10240;
            bzero(buffer,len);
            if(!strcmp(name,"typeOfFirstFixedSurface"))
            {
                long type;
                GRIB_CHECK(grib_get_long(gh[i],name,&type),name);
                cout<<name<<"="<<type<<endl;
            }
            else
            {
            GRIB_CHECK(grib_get_string(gh[i],name,buffer,&len),name);
            cout<<name<<"="<<buffer<<endl;
            }
        }
        grib_handle_delete(gh[i]);

    }
//    //unsigned long key_iterator_filter_flags = GRIB_KEYS_ITERATOR_SKIP_READ_ONLY |GRIB_KEYS_ITERATOR_SKIP_COMPUTED;


      rewind(fp);
      return 0;
}
int grib::ListGribInfoSimple()
{
    char buffer[102];
    size_t len=102;
    int err;
    int count;
    const char * name_space[]={"parameterName" ,"typeOfFirstFixedSurface","parameterUnits","shortName","level","pressureUnits",\
                         "latitudeOfFirstGridPointInDegrees","latitudeOfLastGridPointInDegrees","longitudeOfFirstGridPointInDegrees",\
                         "longitudeOfLastGridPointInDegrees",\
                         "Ni","Nj","getNumberOfValues","year","month","day","hour","minute","second","maximum","minimum","average","missingValue",NULL};

    err=grib_count_in_file(NULL,fp,&count);
    if(err)
    {
        cout<<grib_get_error_message(err);
        return -1;
    }
    grib_handle * gh[count];

    for(int i=0;i<count;i++)
    {
        gh[i]=grib_handle_new_from_file(0,fp,&err);
        if(err)
        {
            cout<<grib_get_error_message(err);
            return -1;
        }
        cout<<"*********************"<<i+1<<endl;
        int j=0;
        while(name_space[j])
           {

            len=102;
            bzero(buffer,len);
            if(!strcmp(name_space[j],"typeOfFirstFixedSurface")||!strcmp(name_space[j],"level"))
            {
                long type;
                GRIB_CHECK(grib_get_long(gh[i],name_space[j],&type),name_space[j]);
                cout<<name_space[j]<<"="<<type<<"    ";
            }
            else
            {
            GRIB_CHECK(grib_get_string(gh[i],name_space[j],buffer,&len),name_space[j]);
            cout<<name_space[j]<<"="<<buffer<<"    ";
            }

            j++;
        }
        cout<<endl;
        grib_handle_delete(gh[i]);

    }
//    //unsigned long key_iterator_filter_flags = GRIB_KEYS_ITERATOR_SKIP_READ_ONLY |GRIB_KEYS_ITERATOR_SKIP_COMPUTED;


      rewind(fp);
      return 0;
}
int grib::ReadVar(const char * name,const std::string & nameOfFirstFixedSurface,long level)
{
    char buffer[102];
    size_t len=102;
    int err;
    int count;
    size_t values_len;
    const char * name_space[]={"parameterName","shortName",NULL};
    err=grib_count_in_file(NULL,fp,&count);
    if(err)
    {
        cout<<grib_get_error_message(err);
        return -1;
    }
    grib_handle * gh;
    int i;
    for(i=0;i<count;i++)
    {
        gh=grib_handle_new_from_file(0,fp,&err);
        if(err)
        {
            cout<<grib_get_error_message(err);
            return -1;
        }
        for(int i=0;i<2;i++)
        {
            len=102;
            bzero(buffer,len);
            GRIB_CHECK(grib_get_string(gh,name_space[i],buffer,&len),name_space[i]);
            if(!strcmp(name,buffer))
            {
                long type;
                len=102;
                GRIB_CHECK(grib_get_string(gh,"nameOfFirstFixedSurface",buffer,&len),"nameOfFirstFixedSurface");
                if(strcmp(nameOfFirstFixedSurface.c_str(),buffer)==0)
                {
                    GRIB_CHECK(grib_get_long(gh,"level",&type),"level");
                    if(level==type||level==0)
                    {
                        err=1;
                        break;
                    }
                }
            }


        }
        if(err==1) break;

            grib_handle_delete(gh);
    }
    if(i==count)
        return -1;
    else
    {
        if(values!=NULL)
            free(values);
        /* get the size of the values array */
          GRIB_CHECK(grib_get_size(gh,"values",&values_len),0);
          values=(double *)malloc(values_len*sizeof(double));
          /* get data values */
          GRIB_CHECK(grib_get_double_array(gh,"values",values,&values_len),0);
    }
    h=gh;
   return 0;
}
void grib::showDetail()
{
    char buffer[1024];
    size_t len=1024;
    unsigned long key_iterator_filter_flags = GRIB_KEYS_ITERATOR_ALL_KEYS;
    char * name_space=NULL;
        grib_keys_iterator * it=grib_keys_iterator_new(h,key_iterator_filter_flags,name_space);
        while(grib_keys_iterator_next(it))
           {
            const char * name=grib_keys_iterator_get_name(it);
            len=1024;
            bzero(buffer,len);
            GRIB_CHECK(grib_get_string(h,name,buffer,&len),name);
            cout<<name<<"="<<buffer<<endl;
        }
}
void grib::show()
{
    char buffer[102];
    size_t len=102;
    const char * name_space[]={"parameterName" ,"typeOfFirstFixedSurface","parameterUnits","shortName","level","pressureUnits",\
                         "latitudeOfFirstGridPointInDegrees","latitudeOfLastGridPointInDegrees","longitudeOfFirstGridPointInDegrees",\
                         "longitudeOfLastGridPointInDegrees",\
                         "Ni","Nj","getNumberOfValues","year","month","day","hour","minute","second","maximum","minimum","average","missingValue",NULL};

        int j=0;
        while(name_space[j])
           {

            len=102;
            bzero(buffer,len);
            if(!strcmp(name_space[j],"typeOfFirstFixedSurface")||!strcmp(name_space[j],"level"))
            {
                long type;
                GRIB_CHECK(grib_get_long(h,name_space[j],&type),name_space[j]);
                cout<<name_space[j]<<"="<<type<<"    ";
            }
            else
            {
            GRIB_CHECK(grib_get_string(h,name_space[j],buffer,&len),name_space[j]);
            cout<<name_space[j]<<"="<<buffer<<"    ";
            }

            j++;
        }
        cout<<endl;

}
int grib::WriteVar(const char *name,char * note,double * values)
{
    int err;
    rewind(fp);
    grib_handle * h=grib_handle_new_from_file(0,fp,&err);
    char string_value[100];
    /* set centre as a long */
    strcat(string_value,note);
    size_t len=strlen(string_value);
    long long_value=len;  
    grib_set_double_array(h,"values",values,181*360);
    GRIB_CHECK(grib_write_message(h, name, "w"), 0);
    FILE * out=fopen(name,"r+");
    h=grib_handle_new_from_file(0,out,&err);
    GRIB_CHECK(grib_set_string(h,"parameterName","u",&len),0);
    grib_handle_delete(h);
    fclose(out);

    return 0;

}
