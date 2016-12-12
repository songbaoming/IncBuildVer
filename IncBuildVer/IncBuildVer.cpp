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
		// 根据 version.h 路径获取该项目下的最后编译类型记录文件路径
		TCHAR szLogPath[MAX_PATH];
		_tcscpy_s(szLogPath, ppArgv[2]);
		auto p = _tcsrchr(szLogPath, TEXT('\\'));
		if (!p)
			break;
		p[1] = 0;
		_tcscat_s(szLogPath, TEXT("ibv_lbt.log"));
		// 读取最后编译的程序类型
		TCHAR szType[10];
		GetPrivateProfileString(TEXT("LASTBUILD"), TEXT("TYPE"), TEXT(""), szType, 10, szLogPath);
		
		if (_tcsicmp(szType, ppArgv[1])){
			// 本次编译类型和最后不同，写入本次编译类型
			WritePrivateProfileString(TEXT("LASTBUILD"), TEXT("TYPE"), ppArgv[1], szLogPath);
			if(szType[0])
				break; // 最后编译类型为空时，说明是第一次编译，需要对版本号加一。所以不为空时跳过加一操作
		}
		
		// 对编译版本号加一
		CVerFile file;
		file.IncBuildVer(ppArgv[2]);
	} while (false);

	LocalFree(ppArgv);
	return 0;
}
