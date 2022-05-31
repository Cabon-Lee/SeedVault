#pragma once

#ifdef CREATEDLL_EXPORTS
#define MATHDLL_DECLSPEC __declspec(dllexport)
#else
#define MATHDLL_DECLSPEC __declspec(dllimport)
#endif



// extern c -> ���� �ͱ۸��� ������� �ʰڴ�!
/* C++ �� C�� �޸� �����ε��� �����Ѵ�
   ������ �Լ��� �̸��̶� �Ű������� ���� ��Ŀ�� �ٸ��� �����Ѵ�.
   ���� ���� �̸��� �Լ��� ���� �� ������ �� � �Լ��� ȣ������ �����ؼ� ��ũ�ϱ� ����
   �����Ϸ��� �̸��� �����ϰ� �ȴ�

   �׷��� DLL�� ����Ͽ� ����� ��ŷ(Explicit Linking)�� �� ��� ȣ���� �Լ��� �̸��� �˾ƾ� �ϴµ�
   �Լ��� �̸��� DLL�� �����Ǹ鼭 �ͱ۸� �Ǹ� ȣ���ϱⰡ ��ƴ�.
   �׷��� extern "C" Ű���带 �ٿ� ���� �ͱ۸��� �������� �ʰڴٰ� �����Ѵ�

   ��� ����� ȣ���� �Ϸ��� ���� �ͱ۸��� ���� �ʾƾ�(extern "C" ���) �Ѵ�
*/


extern "C" MATHDLL_DECLSPEC int Add_Int(const int a, const int b);
extern "C" MATHDLL_DECLSPEC int Sum_Int(const int count, va_list args);
extern "C" MATHDLL_DECLSPEC float Add_Float(const float a, const float b);
extern "C" MATHDLL_DECLSPEC float Sum_Float(const float count, va_list args);
extern "C" MATHDLL_DECLSPEC float Sub(const float a, const float b);
extern "C" MATHDLL_DECLSPEC float Mul(const float a, const float b);
extern "C" MATHDLL_DECLSPEC float Div(const float a, const float b);