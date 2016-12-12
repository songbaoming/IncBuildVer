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
	do{
		// 判断参数数量是否大于等于3
		if (nArgc < 3)
			break;
		// 修改程序路径为配置文件路径
		auto p = _tcsrchr(ppArgv[0], TEXT('.'));
		if (!p)
			break;
		_tcscpy_s(p, 5, TEXT(".ini"));
		// 读取上次编译的程序类型
		TCHAR szType[10];
		GetPrivateProfileString(TEXT("LASTBUILD"), TEXT("TYPE"), TEXT("Debug"), szType, 10, ppArgv[0]);
		
		if (_tcsicmp(szType, ppArgv[1])){
			// 本次编译类型和上次不同，写入本次编译类型
			WritePrivateProfileString(TEXT("LASTBUILD"), TEXT("TYPE"), ppArgv[1], ppArgv[0]);
			break;
		}
		
		// 对编译版本号加一
		CVerFile file;
		file.IncBuildVer(ppArgv[2]);
	} while (false);

	LocalFree(ppArgv);
	return 0;
}
