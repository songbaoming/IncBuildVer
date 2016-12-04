// IncBuildVer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Shellapi.h"
#include "IncBuildVer.h"




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
	do {
		if (nArgc < 2) {
			break;
		}
		// 打开文件
		HANDLE hFile = CreateFile(ppArgv[1], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			DWORD dwRead;
			LARGE_INTEGER large;
			GetFileSizeEx(hFile, &large);
			auto pData = new char[large.QuadPart + 1];
			if (pData) {
				memset(pData, 0, large.QuadPart + 1);
				ReadFile(hFile, pData, large.QuadPart, &dwRead, NULL);
				auto pos = strstr(pData, "BUILD_VER_NUM");
				if (pos) {
					// 定位到编译版本号位置
					pos += strlen("BUILD_VER_NUM");
					while (!isdigit(*pos)) ++pos;
					// 获取当前编译版本号
					DWORD dwBuildVer = atoi(pos);
					LARGE_INTEGER large2;
					// 定位文件指针到编译版本号位置
					large2.QuadPart = pos - pData;
					SetFilePointerEx(hFile, large2, NULL, FILE_BEGIN);
					// 写入新的版本号
					sprintf_s(pData, large2.QuadPart, "%u", ++dwBuildVer);
					WriteFile(hFile, pData, strlen(pData), &dwRead, NULL);
					// 写入其余部分
					while (isdigit(*pos)) ++pos;
					WriteFile(hFile, pos, strlen(pos), &dwRead, NULL);
				}
			}
			CloseHandle(hFile);
		}
	} while (false);

	LocalFree(ppArgv);
    return 0;
}
