#ifndef MYGRIBAPICPLUSPLUS_H
#define MYGRIBAPICPLUSPLUS_H
#include<grib_api.h>
#include<string>
class grib
{
private:
    FILE * fp;
    grib_handle * h;
    double * values;

public:
    grib(const char * file , const char * flag);
    virtual ~grib()
    {

        if(h!=NULL)
            grib_handle_delete(h);
        if(fp!=NULL)
            fclose(fp);
        if(values!=NULL)
            free(values);
    }
    int ListGribInfo();
    int ListGribInfoSimple();
    int ReadVar(const char * name,const std::string &  nameOfFirstFixedSurface,long level=0);
    int WriteVar(const char * name,char * note, double * );
    void showDetail();
    void show();
};

#endif // MYGRIBAPICPLUSPLUS_H
