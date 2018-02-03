////////////////////////////////////////////////////////////////////////////////
// 文件名：IncBuildVer.cpp
// 作者：Song Baoming,2016
// 用途：操作指定的 resource.h 和 *.rc 文件，自动增加编译版本号的程序入口函数定义
// License：https://github.com/songbaoming/IncBuildVer/blob/master/LICENSE
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

	// 获取命令行参数数量
	int nArgc;
	LPWSTR *ppArgv = CommandLineToArgvW(GetCommandLine(), &nArgc);

	// 判断命令行参数数量是否大于1
	if (nArgc > 1) {
		// 对编译版本号加一
		CVersionManager manager;
		manager.IncBuildVer(ppArgv[1]);
	}

	LocalFree(ppArgv);
	return 0;
}
