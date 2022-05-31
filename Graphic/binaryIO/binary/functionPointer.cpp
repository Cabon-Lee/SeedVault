#include <iostream>
using namespace std;

typedef void (*ptrfunc)(int);

void hello()     // ��ȯ���� �Ű������� ����
{
	std::cout << "Hello, world!" << std::endl;
}

int hello(int i)     // ��ȯ���� �Ű������� ����
{
	int out = i + 5;
	std::cout << "Hello, world!" << out << std::endl;

	return out;
}

void bonjour()    // ��ȯ���� �Ű������� ����
{
	std::cout << "bonjour le monde!" << std::endl;
}

void fnc_1() 
{
	printf("fnc �Լ�\n");
}

void fnc_2(int n) 
{
	printf("%d\n", n);
}

void fnc_print(void(*fnc)()) 
{
	fnc();
}

void fnc_print2(void(*fnc)(int), int n) 
{
	fnc(n);
}

int main()
{
	void (*fp)();   // ��ȯ���� �Ű������� ���� �Լ� ������ fp ����
	int (*fpint)(int i); 

	fp = hello;     // hello �Լ��� �޸� �ּҸ� �Լ� ������ fp�� ����
	fp();           // Hello, world!: �Լ� �����ͷ� hello �Լ� ȣ��

	fp = bonjour;   // bonjour �Լ��� �޸� �ּҸ� �Լ� ������ fp�� ����
	fp();           // bonjour le monde!: �Լ� �����ͷ� bonjour �Լ� ȣ��

	fpint = hello;
	fpint(5);

	ptrfunc pf;
	pf = fnc_2;
	pf(3);

	fnc_print(fnc_1);
	fnc_print2(fnc_2, 5);

	return 0;
}

//typedef bool (*Cmp)(int, int);
//bool Up(int x, int y) { return (x > y ? true : false); }
//bool Down(int x, int y) { return (x < y ? true : false); }
//
//void simple_sort(int* arr, int n, Cmp cmp) {
//	for (int i = 0; i < n - 1; i++) {
//		for (int j = i + 1; j < 5; j++) {
//			if (cmp(arr[i], arr[j])) {
//				arr[i] ^= arr[j] ^= arr[i] ^= arr[j];
//			}
//		}
//	}
//}
//
//void sort_print(int* arr, int n) {
//	for (int i = 0; i < 5; i++)
//		cout << arr[i] << " ";
//	cout << endl;
//}
//
//int main(void) {
//	int arr[5] = { 10, 5, 41, 100, 2 };
//
//	simple_sort(arr, 5, Down);
//	sort_print(arr, 5);
//
//	simple_sort(arr, 5, Up);
//	sort_print(arr, 5);
//
//
//	return 0;
//}