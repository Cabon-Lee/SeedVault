#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <cmath>

void abssort(float* x, unsigned n) 
{// Lambda expression begins
	std::sort
	(x, x + n, [](float a, float b) 
		{
			return (std::abs(a) < std::abs(b));
		} // end of lambda expression
	);
}

template <typename T, typename U>
auto add(T t, U u) -> decltype(t + u) {
	return t + u;
}

struct abc 
{
	int a;
};
void main()
{
	int a = 3;
	abc aa;
	decltype(aa) b;  

	std::cout << typeid(aa).name() << std::endl;
	std::cout << typeid(b).name() << std::endl;

	typeid(aa);
	return;
}