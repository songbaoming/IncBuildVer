////////////////////////////////////////////////////////////////////////////////
// �ļ�����IncBuildVer.cpp
// ���ߣ�Song Baoming,2016
// ��;������ָ���� resource.h �� *.rc �ļ����Զ����ӱ���汾�ŵĳ�����ں�������
// License��https://github.com/songbaoming/IncBuildVer/blob/master/LICENSE
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shellapi.h"
#include "IncBuildVer.h"
#include "VersionManager.h"




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// ��ȡ�����в�������
	int nArgc;
	LPWSTR *ppArgv = CommandLineToArgvW(GetCommandLine(), &nArgc);

	// �ж������в��������Ƿ����1
	if (nArgc > 1) {
		// �Ա���汾�ż�һ
		CVersionManager manager;
		manager.IncBuildVer(ppArgv[1]);
	}

	LocalFree(ppArgv);
	return 0;
}
