////////////////////////////////////////////////////////////////////////////////
// 文件名：IncBuildVer.cpp
// 作者：Song Baoming,2016
// 用途：操作指定的 Version.h 文件，自动增加编译版本号的程序入口函数定义
// License：https://github.com/songbaoming/IncBuildVer/blob/master/LICENSE
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shellapi.h"
#include "IncBuildVer.h"
#include "VerFile.h"




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 获取参数数量及 Version.h 的完整路径
	int nArgc;
	LPWSTR *ppArgv = CommandLineToArgvW(GetCommandLine(), &nArgc);
	if (nArgc > 1) {
		CVerFile file;
		file.IncBuildVer(ppArgv[1]);
	}

	LocalFree(ppArgv);
	return 0;
}
