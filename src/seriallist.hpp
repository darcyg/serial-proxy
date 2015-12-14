#ifndef _SERIALLIST_HPP_
#define _SERIALLIST_HPP_

#include <string>
#include <vector>

template <class T>
class SerialList
{
public:
    typedef typename std::vector<T*>::iterator iterator;
    typedef typename std::vector<T*>::const_iterator const_iterator;

    iterator begin(void)
    {
        return m_Devices.begin();
    }

    const_iterator begin(void) const
    {
        return m_Devices.begin();
    }

    iterator end(void)
    {
        return m_Devices.end();
    }

    const_iterator end(void) const
    {
        return m_Devices.end();
    }

    ~SerialList(void)
    {
        iterator beg = begin();

        for (; beg != end(); ++beg)
            delete *beg;

        m_Devices.clear();
    }

    void Add(T* t)
    {
        m_Devices.push_back(t);
    }

    bool Remove(const std::string &path)
    {
        iterator beg = begin();

        for (; beg != end(); ) {
            if ((*beg)->getPort() == path) {

                delete *beg;
                m_Devices.erase(beg);

                return true;
            }
            ++beg;
        }
        return false;
    }

    iterator Remove(iterator iter)
    {
        delete *iter;
        return m_Devices.erase(iter);
    }

    bool Exists(const std::string &path)
    {
        iterator beg = begin();

        for (; beg != end(); ++beg) {
            if ((*beg)->getPort() == path)
                return true;
        }

        return false;
    }

    T* Get(const std::string &path)
    {
        iterator beg = begin();

        for (; beg != end(); ++beg) {
            if ((*beg)->getPort() == path)
                return *beg;
        }

        return NULL;
    }
private:
    std::vector<T*> m_Devices;
};

#endif
