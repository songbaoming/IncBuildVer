////////////////////////////////////////////////////////////////////////////////
// 文件名：VerFile.cpp
// 作者：Song Baoming,2016
// 用途：操作 Version.h 文件，增加编译版本号类 CVerFile 的方法定义文件
// License：https://github.com/songbaoming/IncBuildVer/blob/master/LICENSE
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VerFile.h"


CVerFile::CVerFile()
	: m_dwOffset(0)
	, m_dwCode(0)
	, m_hFile(nullptr)
	, m_pContent(nullptr)
{
}


CVerFile::~CVerFile()
{
	if (m_pContent)
		delete[] m_pContent;
	if (m_hFile && m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
}

// 将指定文件中编译版本号宏的值加一
bool CVerFile::IncBuildVer(LPCTSTR lpszFilePath)
{
	static LPCTSTR pszSign = TEXT("BUILD_VER_NUM");
	// 读取文件内容
	if (!GetFileContent(lpszFilePath))
		return false;
	// 跳转到编译版本号的位置
	auto pszBegin = _tcsstr(m_pContent, pszSign);
	if (!pszBegin)
		return false;
	pszBegin += _tcslen(pszSign);
	while (*pszBegin && !_istdigit(*pszBegin)) ++pszBegin;
	if (!*pszBegin)
		return false;
	auto pszEnd = pszBegin;
	while (*pszEnd && _istdigit(*pszEnd)) ++pszEnd;
	// 根据就版本号计算新版本号，并转换为字符串
	DWORD dwBuildVer = _ttoi(pszBegin);
	TCHAR szNewVer[20];
	_stprintf_s(szNewVer, TEXT("%u"), ++dwBuildVer);

	// 使用版本号以前的字符串重定位文件指针
	if (!SetFilePtrWithString(m_pContent, pszBegin - m_pContent))
		return false;
	// 写入新版本号
	if (!WriteContent(szNewVer))
		return false;
	// 判断版本号长度是否一致，只有不一致时才需要重新写入版本号后的内容
	if (pszEnd - pszBegin != _tcslen(szNewVer))
		return WriteContent(pszEnd);

	return true;
}

// 读取指定文件内容，并转换为 Unicode 编码
bool CVerFile::GetFileContent(LPCTSTR lpszFilePath)
{
	// 打开文件
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
	GetFileCode(pData, large.QuadPart);
	// 根据文件编码将文件内容转换成 unicode
	if (m_dwCode == CP_UTF16) {
		m_pContent = new TCHAR[large.QuadPart / sizeof(TCHAR) + 1];
		if (m_pContent) {
			memset(m_pContent, 0, large.QuadPart + sizeof(TCHAR));
			memcpy_s(m_pContent, large.QuadPart + sizeof(TCHAR), pData + m_dwOffset, large.QuadPart - m_dwOffset);
		}
	}
	else if (m_dwCode == CP_UTF16B) {
		m_pContent = new TCHAR[large.QuadPart / sizeof(TCHAR) + 1];
		if (m_pContent) {
			memset(m_pContent, 0, large.QuadPart + sizeof(TCHAR));
			char *pDst = (char*)m_pContent;
			char *pSrc = pData + m_dwOffset;
			for (LONGLONG i = 0; i + 1 < large.QuadPart; i += 2) {
				pDst[i] = pSrc[i + 1];
				pDst[i + 1] = pSrc[i];
			}
		}
	}
	else {
		DWORD dwLen = MultiByteToWideChar(m_dwCode, 0, pData + m_dwOffset, -1, nullptr, 0);
		if (dwLen) {
			m_pContent = new TCHAR[dwLen];
			if (m_pContent)
				MultiByteToWideChar(m_dwCode, 0, pData + m_dwOffset, -1, m_pContent, dwLen);
		}
	}

	UnmapViewOfFile(pData);
	return m_pContent != nullptr;
}

// 获取该文件内容的编码格式
bool CVerFile::GetFileCode(LPCSTR pData, LONGLONG llLen)
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
bool CVerFile::IsCodeUtf8(LPCSTR pString, LONGLONG llLen)
{
	bool bRet = false;
	auto pData = (PBYTE)pString;
	for (LONGLONG i = 0; i < llLen; ++i) {
		if (pData[i] > 0x80) {
			int nCount = 1;
			while ((pData[i] << nCount) & 0x80) ++nCount;
			if (nCount > 6 || i + nCount > llLen)
				return false;
			for (int j = 1; j < nCount; ++j) {
				if ((pData[i + j] & 0xc0) != 0x80)
					return false;
			}
			//i += nCount;
			//bRet = true;
			return true;
		}
		//else
		//	++i;
	}
	return bRet;
}

// 判断给定内容是否为utf-16格式的编码，如果是则返回字符编码，否则返回零
int CVerFile::IsCodeUtf16(LPCSTR pString, LONGLONG llLen)
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

// 从给定的版本号之前的字符串，并根据原文件编码方式和文件内容的偏移量，
// 计算并设置文件指针的位置，跳过编译版本号之前不会被更改的内容，
// 用于后续写入，并减少 I/O 提高效率
bool CVerFile::SetFilePtrWithString(LPCTSTR lpszProBuildVer, DWORD dwLen)
{
	LARGE_INTEGER large = { m_dwOffset };

	if (m_dwCode != CP_UTF16 && m_dwCode != CP_UTF16B) {
		int nLen = WideCharToMultiByte(m_dwCode, 0, lpszProBuildVer, dwLen, nullptr, 0, nullptr, nullptr);
		if (!nLen)
			return false;
		large.QuadPart += nLen;
	}
	else
		large.QuadPart += dwLen * sizeof(TCHAR);
	
	return TRUE == SetFilePointerEx(m_hFile, large, nullptr, FILE_BEGIN);
}

// 将给定的内容，转换为原文件编码格式后，写入文件
bool CVerFile::WriteContent(LPCTSTR lpszContent)
{
	DWORD dwWriten;

	if (m_dwCode == CP_UTF16) {
		return WriteFile(m_hFile, lpszContent, _tcslen(lpszContent) * sizeof(TCHAR), &dwWriten, nullptr);
	}
	else if (m_dwCode == CP_UTF16B) {
		DWORD dwLen = _tcslen(lpszContent) * sizeof(TCHAR);
		auto pDst = new char[dwLen];
		if (pDst) {
			auto pSrc = (const char*)lpszContent;
			for (DWORD i = 0; i + 1 < dwLen; i += 2) {
				pDst[i] = pSrc[i + 1];
				pDst[i + 1] = pSrc[i];
			}
			auto bRes = WriteFile(m_hFile, pDst, dwLen, &dwWriten, nullptr);
			delete[] pDst;
			return bRes;
		}
	}
	else {
		int nLen = WideCharToMultiByte(m_dwCode, 0, lpszContent, -1, nullptr, 0, nullptr, nullptr);
		if (!nLen)
			return false;
		auto pBuff = new char[nLen];
		if (pBuff) {
			WideCharToMultiByte(m_dwCode, 0, lpszContent, -1, pBuff, nLen, nullptr, nullptr);
			auto bRes = WriteFile(m_hFile, pBuff, nLen - 1, &dwWriten, nullptr);
			delete[] pBuff;
			return bRes;
		}
	}
	
	return false;
}
