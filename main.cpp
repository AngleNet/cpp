#include "Single.h"
#include <iostream>

int main(void){
	Single1* t1 = Single1::create();
	Single1* t2 = Single1::create();
	if(t1 == t2){
		std::cout<<"Single1 success"<<std::endl;
	}
	Single2* t3 = Single2::create();
	Single2* t4 = Single2::create();
	if(t3 == t4){
		std::cout<<"Single2 success"<<std::endl;
	}
	return 0;
}
