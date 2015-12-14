#include "exception.hpp"

using namespace std;

InterruptException::InterruptException(const string &msg): m_Msg(msg)
{
}

InterruptException::~InterruptException(void) throw()
{
}

const char *InterruptException::what(void) const throw()
{
    return m_Msg.c_str();
}
