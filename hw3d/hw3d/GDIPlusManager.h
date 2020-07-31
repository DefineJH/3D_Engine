#pragma once
#include "ChiliWin.h"
//gdiplus는 초기화, 제거시에 해줘야하는 작업이 있음 RAII를 사용하여 초기화, 제거를 손쉽게 한다
class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;
};

