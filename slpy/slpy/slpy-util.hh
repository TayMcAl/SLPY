#ifndef __slpy_util_
#define __slpy_util_

class Locn {
public:
    std::string source_name;
    int row;
    int column;
    //
    Locn(std::string fn, int rw, int co)
        : source_name {fn}, row {rw}, column {co} { }
    Locn(void) : Locn {"",0,0} { }
};
    
const std::string slpy_message(Locn lo, std::string ms);

class SlpyError: public std::exception {
private:
    Locn location;
    const std::string message;
    
public:    
    SlpyError(Locn lo, std::string ms);
    const char* what();
};
        
#endif
