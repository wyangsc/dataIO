#ifndef DATAIOERROR__
#define DATAIOERROR__
#include<iostream>
#define OPENFAIL 1
class dataIOError
{
private:
    int num;
public:
    dataIOError(){};
    dataIOError(int x):num(x){};
    void what();    
};
#endif
