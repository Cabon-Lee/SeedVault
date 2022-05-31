#include <iostream>
using namespace std;

class Plus 
{
public:
	Plus() 
	{
		num = 0;
	}

	int operator()() 
	{
		num++;
		return num;
	}

	int operator()(int n) 
	{
		num = ++n;
		return num;
	}

	friend ostream& operator<<(ostream&, const Plus&);

private:
	int num;

};

ostream& operator<<(ostream& os, const Plus& plus) {
	os << plus.num << endl;
	return os;
}

int main() {
	Plus plus;
	int num;
	num = plus(5);

	cout << num << endl;

	for (int i = 0; i < 5; i++)
		plus();

	cout << plus << endl;
	return 0;
}