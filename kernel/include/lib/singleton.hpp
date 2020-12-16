#ifndef __SINGLETON_HPP
#define __SINGLETON_HPP

template <class T>
class Singleton {
public:
    static T* instance;

    ~Singleton() {
        instance = nullptr;
    }
};

template <class T> T* Singleton<T>::instance = nullptr;

#endif
