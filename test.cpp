
#include "data_loader.hpp"
#include <iostream>
#include <string>
#include <chrono>

int main(){

	std::chrono::steady_clock sClock;
	auto start = sClock.now();
    char c;
    scanf("%c",&c);
	auto end = sClock.now();
	auto time_span = static_cast<std::chrono::duration<double>>(end - start);
	std::cout << time_span.count() << std::endl;


return 0;
}