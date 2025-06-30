#include <iostream>

#include <Support/WinInclude.h>

using namespace std;

int main()
{

    IUnknown* p;
    // 함수 주소, 쿼리 인터페이스, 릴리즈
    p->Release();

    POINT pt;
    GetCursorPos(&pt);
    cout << "The cursor is x : " << pt.x << " y : " << pt.y;
}


