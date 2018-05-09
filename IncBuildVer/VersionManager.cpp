////////////////////////////////////////////////////////////////////////////////
// 文件名：VerFile.cpp
// 作者：Song Baoming,2016
// 用途：操作 resource.h 和 *.rc 文件，增加编译版本号类 CVerFile 的方法定义文件
// License：https://github.com/songbaoming/IncBuildVer/blob/master/LICENSE
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VersionManager.h"




enum {
	MAJOR,
	MINOR,
	REVISION,
	BUILD
};

#define VERSION_COUNT						4

static LPCTSTR g_pszSign[] = {
	TEXT("MAJOR_VER_NUM"),
	TEXT("MINOR_VER_NUM"),
	TEXT("REVISION_VER_NUM"),
	TEXT("BUILD_VER_NUM"),
	NULL
};


DWORD g_dwSignLen[VERSION_COUNT];


CVersionManager::CVersionManager()
	: m_dwOffset(0)
	, m_dwCode(0)
	, m_hFile(nullptr)
	, m_pContent(nullptr)
{
	memset(m_dwVerArr, 0, sizeof(m_dwVerArr));
	for (int i = 0; g_pszSign[i]; ++i)
		g_dwSignLen[i] = _tcslen(g_pszSign[i]);
}


CVersionManager::~CVersionManager()
{
	if (m_pContent)
		delete[] m_pContent;
	if (m_hFile && m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
}

// 将指定文件中编译版本号宏的值加一
bool CVersionManager::IncBuildVer(LPCTSTR lpszProjectPath)
{
	TCHAR szFilePath[MAX_PATH] = {};
	_stprintf_s(szFilePath, TEXT("%sresource.h"), lpszProjectPath);
	// 获取并更改 resource.h 中的版本信息
	if (!IncResourceVer(szFilePath))
		return false;
	// *.rc文件路径
	int nIndex = _tcslen(lpszProjectPath);
	for (--nIndex; nIndex > 0 && lpszProjectPath[nIndex - 1] != TEXT('\\'); --nIndex);
	if (nIndex <= 0)
		return false;
	_stprintf_s(szFilePath, TEXT("%s%s"), lpszProjectPath, lpszProjectPath + nIndex);
	szFilePath[_tcslen(szFilePath) - 1] = 0;
	_tcscat_s(szFilePath, TEXT(".rc"));
	// 更改 *.rc 中版本信息
	if (!IncRCFileVer(szFilePath))
		return false;

	return true;
}

DWORD CVersionManager::ParseVerPos(LPCTSTR lpszContent, LPCTSTR &lpszFirstValidPos, LPCTSTR *lppszVerPos)
{
	assert(lpszContent);

	enum {
		COMMENT_NONE,
		COMMENT_LINE,
		COMMENT_BLOCK
	};

	TCHAR chLast = 0x20;
	DWORD dwComment = 0, dwCount = 0;
	LPCTSTR pos = lpszContent, pszTemp = nullptr;
	for (; *pos && dwCount < VERSION_COUNT; ++pos) {
		switch (dwComment) {
		case COMMENT_LINE:
			if (*pos == TEXT('\n'))
				dwComment = COMMENT_NONE;
			chLast = 0x20;
			break;
		case COMMENT_BLOCK:
			if (*pos == TEXT('/') && chLast == TEXT('*'))
				dwComment = COMMENT_NONE;
			chLast = 0x20;
			break;
		default:
			if (*pos == TEXT('/')) {
				if (pos[1] == TEXT('/')) {
					++pos;
					dwComment = COMMENT_LINE;
					chLast = 0x20;
					break;
				}else if (pos[1] == TEXT('*')) {
					++pos;
					dwComment = COMMENT_BLOCK;
					chLast = 0x20;
					break;
				}
			}

			if (!lpszFirstValidPos && _istgraph(*pos))
				lpszFirstValidPos = pos;

			if (_istspace(chLast)) {
				for (int i = 0; g_pszSign[i]; ++i) {
					if (*pos == g_pszSign[i][0] && !_tcsncmp(pos, g_pszSign[i], g_dwSignLen[i]) &&
						_istspace(*(pos + g_dwSignLen[i]))) {
						pos += g_dwSignLen[i];
						while (!_istdigit(*pos)) ++pos;
						lppszVerPos[i] = pos;
						m_dwVerArr[i] = _ttoi(pos);
						++dwCount;
						break;
					}
				}
			}
			
			chLast = *pos;
			break;
		}
	}
	if (!lpszFirstValidPos)
		lpszFirstValidPos = pos;
	return dwCount;
}


bool CVersionManager::IncResourceVer(LPCTSTR lpszFilePath)
{
	bool bRes = true;
	LPCTSTR pszFirstValidPos = nullptr, pszVerPos[VERSION_COUNT] = {};

	// 读取文件内容
	if (!GetFileContent(lpszFilePath))
		return false;

	// 解析版本宏，获取版本号
	auto dwCount = ParseVerPos(m_pContent, pszFirstValidPos, pszVerPos);
	//TCHAR chLast = 0;
	//bool bComment = false;
	//for (auto pos = m_pContent; *pos; ++pos) {
	//	if (bComment) {

	//	}
	//}
	//auto pszBegin = m_pContent;
	//for (int i = 0; g_pszSign[i]; ++i){
	//	pszVerPos[i] = m_pContent;
	//	int nLen = _tcslen(g_pszSign[i]);
	//	while ((pszVerPos[i] = _tcsstr(pszVerPos[i] + 1, g_pszSign[i])) &&
	//		!_istspace(*(pszVerPos[i] - 1)) && !_istspace(*(pszVerPos[i] + nLen)));
	//	if (!pszVerPos[i]) {
	//		bMissVersion = true;
	//		continue;
	//	}
	//	m_dwVersion[i] = _ttoi(pszVerPos[i] + nLen);
	//}
	// 跳转到编译版本号的位置
	//while (*pszBegin && !_istdigit(*pszBegin)) ++pszBegin;
	//if (!*pszBegin)
	//	return false;
	//auto pszEnd = pszBegin;
	//while (*pszEnd && _istdigit(*pszEnd)) ++pszEnd;
	// 根据旧版本号计算新版本号，并转换为字符串
	//DWORD dwBuildVer = _ttoi(pszBegin);

	TCHAR szText[100];
	if (dwCount != VERSION_COUNT) {
		// 使用第一个有效字符以前的字符串重定位文件指针
		if (!SetFilePtrWithString(m_pContent, pszFirstValidPos - m_pContent))
			return false;

		for (int i = 0; i < VERSION_COUNT; ++i) {
			if (!pszVerPos[i]) {
				if (i == BUILD)
					++m_dwVerArr[BUILD];
				_stprintf_s(szText, TEXT("#define %s\t\t\t\t\t%u\r\n"), g_pszSign[i], m_dwVerArr[i]);
				if (!WriteContent(szText, _tcslen(szText)))
					return false;
			}
		}
		if (pszVerPos[BUILD]) {
			assert(pszVerPos[BUILD] > pszFirstValidPos);
			if (!WriteContent(pszFirstValidPos, pszVerPos[BUILD] - pszFirstValidPos))
				return false;
			// 写入新版本号
			_stprintf_s(szText, TEXT("%u"), ++m_dwVerArr[BUILD]);
			if (!WriteContent(szText, _tcslen(szText)))
				return false;
			while (_istdigit(*(pszVerPos[BUILD]))) ++(pszVerPos[BUILD]);
			// 判断版本号长度是否一致，只有不一致时才需要重新写入版本号后的内容
			if (!WriteContent(pszVerPos[BUILD], _tcslen(pszVerPos[BUILD])))
				return false;
		}else {
			if (!WriteContent(pszFirstValidPos, _tcslen(pszFirstValidPos)))
				return false;
		}
		SetEndOfFile(m_hFile);
	} else {
		// 使用编译版本号以前的字符串重定位文件指针
		if (!SetFilePtrWithString(m_pContent, pszVerPos[BUILD] - m_pContent))
			return false;
		// 写入新版本号
		_stprintf_s(szText, TEXT("%u"), ++m_dwVerArr[BUILD]);
		if (!WriteContent(szText, _tcslen(szText)))
			return false;
		int nLen = 0;
		while (_istdigit(*(pszVerPos[BUILD]))) {
			++(pszVerPos[BUILD]);
			++nLen;
		}
		// 判断版本号长度是否一致，只有不一致时才需要重新写入版本号后的内容
		if (nLen != _tcslen(szText)) {
			bRes = WriteContent(pszVerPos[BUILD], _tcslen(pszVerPos[BUILD]));
			SetEndOfFile(m_hFile);
		}
	}
	
	
	return bRes;
}

bool CVersionManager::IncRCFileVer(LPCTSTR lpszFilePath)
{
	static LPCTSTR pszSign[] = {
		TEXT("FILEVERSION"),
		TEXT("PRODUCTVERSION"),
		TEXT("\"FileVersion\","),
		TEXT("\"ProductVersion\","),
		NULL
	};
	bool bRes = false;

	// 读取文件内容
	if (!GetFileContent(lpszFilePath))
		return false;

	auto pszBegin = m_pContent;
	auto pszEnd = pszBegin;
	TCHAR szText[100];
	for (int i = 0; pszSign[i]; ++i){

		pszEnd = _tcsstr(pszBegin, pszSign[i]);
		if (!pszEnd)
			return false;
		pszEnd += _tcslen(pszSign[i]);
		while (*pszEnd && !_istgraph(*pszEnd)) ++pszEnd;
		if (!*pszEnd)
			return false;

		if (i == 0)
			bRes = SetFilePtrWithString(pszBegin, pszEnd - pszBegin);
		else
			bRes = WriteContent(pszBegin, pszEnd - pszBegin);
		if (!bRes)
			return false;
		
		if (!i){
			_stprintf_s(szText, TEXT("%u,%u,%u,%u"), m_dwVerArr[MAJOR], m_dwVerArr[MINOR]
				, m_dwVerArr[REVISION], m_dwVerArr[BUILD]);
		}else if(i == 2){
			_stprintf_s(szText, TEXT("\"%u.%u.%u.%u\""), m_dwVerArr[MAJOR], m_dwVerArr[MINOR]
				, m_dwVerArr[REVISION], m_dwVerArr[BUILD]);
		}
		if (!WriteContent(szText, _tcslen(szText)))
			return false;

		while (*pszEnd && _istprint(*pszEnd)) ++pszEnd;
		if (!*pszEnd)
			return false;
		pszBegin = pszEnd;
	}
	// 写入剩余内容
	bRes = WriteContent(pszEnd, _tcslen(pszEnd));
	SetEndOfFile(m_hFile);

	return bRes;
}

// 读取指定文件内容，并转换为 Unicode 编码
bool CVersionManager::GetFileContent(LPCTSTR lpszFilePath)
{
	// 打开文件
	if (m_hFile)
		CloseHandle(m_hFile);
	m_hFile = CreateFile(lpszFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, 0, nullptr);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return false;
	LARGE_INTEGER large;
	GetFileSizeEx(m_hFile, &large);
	if (!large.QuadPart)
		return false;
	// 创建文件映射并映射到内存页
	HANDLE hMapping = CreateFileMapping(m_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (!hMapping)
		return false;
	auto pData = (char*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hMapping);
	if (!pData)
		return false;
	// 获取文件编码
	GetFileContentCode(pData, large.QuadPart);
	// 根据文件编码将文件内容转换成 unicode
	if (m_pContent)
		delete[] m_pContent;
	m_pContent = FileContentToUnicode(pData + m_dwOffset, large.QuadPart - m_dwOffset);

	UnmapViewOfFile(pData);
	return m_pContent != nullptr;
}

// 获取该文件内容的编码格式
bool CVersionManager::GetFileContentCode(LPCSTR pData, LONGLONG llLen)
{
	if (llLen > 3) {
		DWORD dwBOM = MAKELONG(MAKEWORD(pData[0], pData[1]), MAKEWORD(pData[2], 0));
		if (dwBOM == 0xBFBBEF) { // utf-8
			m_dwCode = CP_UTF8;
			m_dwOffset = 3;
		}
		else if (LOWORD(dwBOM) == 0xFFFE) { // utf-16 big endian
			m_dwCode = CP_UTF16B;
			m_dwOffset = 2;
		}
		else if (LOWORD(dwBOM) == 0xFEFF) { // utf-16
			m_dwCode = CP_UTF16;
			m_dwOffset = 2;
		}
	}
	if(!m_dwCode) {
		if (IsCodeUtf8(pData, llLen))
			m_dwCode = CP_UTF8;
		else
			m_dwCode = IsCodeUtf16(pData, llLen);
	}
	
	return true;
}

// 判断给定的内容是否为utf-8编码
bool CVersionManager::IsCodeUtf8(LPCSTR pString, LONGLONG llLen)
{
	bool bRet = false;
	auto pData = (PBYTE)pString;
	for (LONGLONG i = 0; i < llLen; ++i) {
		if (pData[i] > 0x80) {
			int nCount = 1;
			while ((pData[i] << nCount) & 0x80) ++nCount;
			if (nCount < 2 || nCount > 6 || i + nCount > llLen)
				return false;
			for (int j = 1; j < nCount; ++j) {
				if ((pData[i + j] & 0xc0) != 0x80)
					return false;
			}
			return true;
		}
	}
	return bRet;
}

// 判断给定内容是否为utf-16格式的编码，如果是则返回字符编码，否则返回零
int CVersionManager::IsCodeUtf16(LPCSTR pString, LONGLONG llLen)
{
	if (llLen % 2)
		return 0;
	for (LONGLONG i = 0; i + 1 < llLen; ++i) {
		if (!pString[i])
			return CP_UTF16B;
		else if (!pString[i + 1])
			return CP_UTF16;
	}
	return 0;
}

LPTSTR CVersionManager::FileContentToUnicode(LPCSTR lpszSrc, LONGLONG llLen)
{
	LPTSTR pContent = nullptr;
	if (m_dwCode == CP_UTF16) {
		pContent = new TCHAR[llLen / sizeof(TCHAR) + 1];
		if (pContent) {
			memset(pContent, 0, llLen + sizeof(TCHAR));
			memcpy_s(pContent, llLen + sizeof(TCHAR), lpszSrc, llLen);
		}
	}
	else if (m_dwCode == CP_UTF16B) {
		pContent = new TCHAR[llLen / sizeof(TCHAR)+1];
		if (pContent) {
			memset(pContent, 0, llLen + sizeof(TCHAR));
			char *pDst = (char*)pContent;
			const char *pSrc = lpszSrc;
			for (LONGLONG i = 0; i + 1 < llLen; i += 2) {
				pDst[i] = pSrc[i + 1];
				pDst[i + 1] = pSrc[i];
			}
		}
	}
	else {
		DWORD dwLen = MultiByteToWideChar(m_dwCode, 0, lpszSrc, llLen, nullptr, 0);
		if (dwLen) {
			pContent = new TCHAR[dwLen + 1];
			if (pContent){
				MultiByteToWideChar(m_dwCode, 0, lpszSrc, llLen, pContent, dwLen);
				pContent[dwLen] = 0;
			}
		}
	}
	return pContent;
}

// 从给定的版本号之前的字符串，并根据原文件编码方式和文件内容的偏移量，
// 计算并设置文件指针的位置，跳过编译版本号之前不会被更改的内容，
// 用于后续写入，并减少 I/O 提高效率
bool CVersionManager::SetFilePtrWithString(LPCTSTR lpszProBuildVer, DWORD dwLen)
{
	LARGE_INTEGER large = { m_dwOffset };

	if (m_dwCode != CP_UTF16 && m_dwCode != CP_UTF16B) {
		if(dwLen)
		dwLen = WideCharToMultiByte(m_dwCode, 0, lpszProBuildVer, dwLen, nullptr, 0, nullptr, nullptr);
		large.QuadPart += dwLen;
	}
	else
		large.QuadPart += dwLen * sizeof(TCHAR);
	
	return TRUE == SetFilePointerEx(m_hFile, large, nullptr, FILE_BEGIN);
}

// 将给定的内容，转换为原文件编码格式后，写入文件
bool CVersionManager::WriteContent(LPCTSTR lpszContent, DWORD dwLen)
{
	DWORD dwWriten;

	if (m_dwCode == CP_UTF16) {
		return WriteFile(m_hFile, lpszContent, dwLen * sizeof(TCHAR), &dwWriten, nullptr);
	}
	else if (m_dwCode == CP_UTF16B) {
		DWORD dwBytes = dwLen * sizeof(TCHAR);
		auto pDst = new char[dwBytes];
		if (pDst) {
			auto pSrc = (const char*)lpszContent;
			for (DWORD i = 0; i + 1 < dwBytes; i += 2) {
				pDst[i] = pSrc[i + 1];
				pDst[i + 1] = pSrc[i];
			}
			auto bRes = WriteFile(m_hFile, pDst, dwBytes, &dwWriten, nullptr);
			delete[] pDst;
			return bRes;
		}
	}
	else {
		int nLen = WideCharToMultiByte(m_dwCode, 0, lpszContent, dwLen, nullptr, 0, nullptr, nullptr);
		if (!nLen)
			return false;
		auto pBuff = new char[nLen];
		if (pBuff) {
			WideCharToMultiByte(m_dwCode, 0, lpszContent, dwLen, pBuff, nLen, nullptr, nullptr);
			auto bRes = WriteFile(m_hFile, pBuff, nLen, &dwWriten, nullptr);
			delete[] pBuff;
			return bRes;
		}
	}
	
	return false;
}
