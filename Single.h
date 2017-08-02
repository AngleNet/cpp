#include <mutex>
class Single1{
private:
	Single1();
	Single1(const Single1& other);
	~Single1();
	static std::mutex* mtx;
	static Single1* instance;
public:
	static Single1* create();
};

class Single2{
private:
	Single2();
	Single2(const Single2& other);
	~Single2();
	static Single2* instance;
public:
	static Single2* create();
};
