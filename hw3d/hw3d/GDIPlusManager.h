#pragma once
#include "ChiliWin.h"
//gdiplus�� �ʱ�ȭ, ���Žÿ� ������ϴ� �۾��� ���� RAII�� ����Ͽ� �ʱ�ȭ, ���Ÿ� �ս��� �Ѵ�
class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;
};

