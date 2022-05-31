#pragma once

#ifdef CREATEDLL_EXPORTS
#define MATHDLL_DECLSPEC __declspec(dllexport)
#else
#define MATHDLL_DECLSPEC __declspec(dllimport)
#endif



// extern c -> 네임 맹글링을 사용하지 않겠다!
/* C++ 은 C와 달리 오버로딩을 지원한다
   동일한 함수의 이름이라도 매개변수에 따라 링커는 다르게 구분한다.
   따라서 같은 이름의 함수가 여러 개 존재할 때 어떤 함수를 호출할지 구분해서 링크하기 위해
   컴파일러가 이름을 변형하게 된다

   그런데 DLL을 사용하여 명시적 링킹(Explicit Linking)을 할 경우 호출할 함수의 이름을 알아야 하는데
   함수의 이름이 DLL로 생성되면서 맹글링 되면 호출하기가 어렵다.
   그래서 extern "C" 키워드를 붙여 네임 맹글링을 수행하지 않겠다고 정의한다

   고로 명시적 호출을 하려면 네임 맹글링을 하지 않아야(extern "C" 사용) 한다
*/


extern "C" MATHDLL_DECLSPEC int Add_Int(const int a, const int b);
extern "C" MATHDLL_DECLSPEC int Sum_Int(const int count, va_list args);
extern "C" MATHDLL_DECLSPEC float Add_Float(const float a, const float b);
extern "C" MATHDLL_DECLSPEC float Sum_Float(const float count, va_list args);
extern "C" MATHDLL_DECLSPEC float Sub(const float a, const float b);
extern "C" MATHDLL_DECLSPEC float Mul(const float a, const float b);
extern "C" MATHDLL_DECLSPEC float Div(const float a, const float b);