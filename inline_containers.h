#ifndef __INLINE_CONTAINER_H__
#define __INLINE_CONTAINER_H__

// Andrei Alexandrescu implementation of "inline containers"
#include <vector>
#include <list>
#include <deque>
     
template < typename T, class container = std::vector<T> > 
class inline_container : public container
{
public:
    inline_container()      
    {
    }
    inline_container(const inline_container<T> &v) :
        container(v.begin(), v.end())
    {
    }
    explicit inline_container(const T &a) 
        : container(1, a)
    {           
    }
    inline_container &operator()(const T &a) 
    {
        this->push_back(a);
        return *this;
    }
};
     
template <typename T>
inline inline_container<T> make_vector(const T &a) 
{
    return inline_container<T>(a);
}
     
template <typename T>
inline inline_container< T, std::list<T> > make_list(const T &a)
{
    return inline_container< T, std::list<T> >(a);
}
     
template <typename T>
inline inline_container< T, std::deque<T> > make_deque(const T &a)
{
    return inline_container< T, std::deque<T> >(a);
}

#if 0
template <class container> 
inline const container::value_type min(const container &a) 
{
    return *std::min_element(a.begin(), a.end());
}
     
template <class container> 
inline const container::value_type max(const container &a) 
{
    return *std::max_element(a.begin(), a.end());
}
#endif

#endif //__INLINE_CONTAINER_H__
