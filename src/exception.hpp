#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

#include <string>
#include <exception>

class InterruptException : public std::exception {

public:
    InterruptException(const std::string &msg);
    ~InterruptException(void) throw();
    const char *what(void) const throw();

private:
    std::string m_Msg;
};

#endif
