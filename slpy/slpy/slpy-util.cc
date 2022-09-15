#include <sstream>
#include "slpy-util.hh"

const std::string slpy_message(Locn lo, std::string ms) {
    std::stringstream ss { };
    ss << lo.source_name << ":" << lo.row << ":" << lo.column << ":";
    ss << "\n\t" << ms << "\n";
    return ss.str();
}

SlpyError::SlpyError(Locn lo, std::string ms) :
    std::exception { },
    message { slpy_message (lo, ms) }
{ }

const char* SlpyError::what() {
    return message.c_str();
}
    
