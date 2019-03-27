#pragma once

#include "noncopyable.h"

namespace v {
namespace co {
template<typename T>
class Singleton : NonCopyable{
public:
    static T& Instance() {
        static T t;
        return t;
    }
    
protected:
    Singleton() {};
    ~Singleton() {};
};

}
}
