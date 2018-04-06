#include<iostream>
#include"grib.h"
using namespace std;

int main()
{
  
       grib g("/home/wyang/NCL/fnl_20080606_00_00.grib2","r");
    //   g.ListGribInfo();
       g.ReadVar("u","Isobaric surface ",1000);//
       cout<<"OK"<<endl;
}
