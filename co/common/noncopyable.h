#pragma once

namespace v {
namespace co {
class NonCopyable {
public:
    NonCopyable(const NonCopyable&) = delete;
    void operator=(const NonCopyable&) = delete;
    
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

}
}