#pragma once
#include <iostream>
#include <sstream>
#include <fstream>


namespace v {
namespace co {

// noncopyable
class NonCopyable {
public:
	NonCopyable(const NonCopyable&) = delete;
	void operator=(const NonCopyable&) = delete;

protected:
	NonCopyable() = default;
	~NonCopyable() = default;
};

// singleton
template<typename T>
class Singleton : NonCopyable {
public:
	static T& Instance() {
		static T t;
		return t;
	}

protected:
	Singleton() {};
	~Singleton() {};
};


// logger
template<typename T>
class AddSpaceHelper {
public:
	AddSpaceHelper(T const& ref) : ref_(ref) {}
	friend std::ostream& operator << (std::ostream &os, AddSpaceHelper<T> t) {
		return os << t.ref_ << ' ';
	}

private:
	T const& ref_;
};

class CoLogger : public Singleton<CoLogger> {
	friend class Singleton<CoLogger>;
public:
	static const int BUFF_SZ = 4096;

	void Log() {
		std::cout << std::endl;
	}

	template<typename First, typename... Args>
	void Log(First const& first, Args const&... args) {
		std::cout << first << " ";
		Log(args...);
	}

protected:
	std::ofstream fstream_;

};

#define CoLog(...)   CoLogger::Instance().Log(__VA_ARGS__)

}
}