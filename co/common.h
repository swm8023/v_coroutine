#pragma once
#include <iostream>
#include <sstream>
#include <fstream>


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

class Logger : public Singleton<Logger> {
	friend class Singleton<Logger>;
public:
	static const int BUFF_SZ = 4096;

	template<typename... Args>
	void Log(Args const&... args) {
		std::ostringstream temp_os;
		(temp_os << ... << AddSpaceHelper(args)) << std::endl;
		std::cout << temp_os.str();
	}


protected:
	std::ofstream fstream_;

};

#define Log(...)   Logger::Instance().Log(##__VA_ARGS__)

}
}