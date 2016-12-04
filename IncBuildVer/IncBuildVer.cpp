// IncBuildVer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Shellapi.h"
#include "IncBuildVer.h"

#define MAX_LOADSTRING 100



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	TCHAR szPath[MAX_PATH];
	int nArgc;
	LPWSTR *ppArgv = CommandLineToArgvW(GetCommandLine(), &nArgc);
	do {
		if (nArgc < 2) {
			break;
		}
		_stprintf_s(szPath, TEXT("%sVersion.h"), ppArgv[1]);
		HANDLE hFile = CreateFile(szPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
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
					pos += strlen("BUILD_VER_NUM");

					while (!isdigit(*pos)) ++pos;
					DWORD dwBuildVer = atoi(pos);
					LARGE_INTEGER large2;
					large2.QuadPart = pos - pData;
					SetFilePointerEx(hFile, large2, NULL, FILE_BEGIN);
					sprintf_s(pData, large2.QuadPart, "%u", ++dwBuildVer);
					WriteFile(hFile, pData, strlen(pData), &dwRead, NULL);
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
