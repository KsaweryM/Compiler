#ifndef DATA_H
#define DATA_H

class Data {
public:
    virtual ComplexCommand* assign(int value) = 0;
    virtual ComplexCommand* pushOnStack() = 0;
    Data() {
        
    }
};
#endif