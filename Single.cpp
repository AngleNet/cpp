#include "Single.h"

Single1::Single1(){}
Single1::Single1(const Single1& other){}
Single1::~Single1(){}

Single1* Single1::create(){
	if(instance == NULL){
		mtx->lock();
		if(instance == NULL){
			instance = new Single1();
		}
		mtx->unlock();
	}
	return instance;
}

Single1* Single1::instance = NULL;
std::mutex* Single1::mtx = new std::mutex();

Single2::Single2(){}
Single2::Single2(const Single2& other){}
Single2::~Single2(){}

Single2* Single2::create(){
	return instance;
}

Single2* Single2::instance = new Single2();
