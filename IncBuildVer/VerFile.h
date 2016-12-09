////////////////////////////////////////////////////////////////////////////////
// 文件名：VerFile.h
// 作者：Song Baoming,2016
// 用途：操作 Version.h 文件，增加编译版本号类 CVerFile 的头文件
// License：https://github.com/songbaoming/IncBuildVer/blob/master/LICENSE
////////////////////////////////////////////////////////////////////////////////

#pragma once

#define	CP_UTF16				1200
#define	CP_UTF16B				1201



class CVerFile
{
public:
	CVerFile();
	~CVerFile();

	// 功能：将指定文件中编译版本号宏的值加一
	// 参数：LPCTSTR lpszFilePath：目标文件的完整路径
	// 返回值：成功时返回 true，否则返回 false
	bool IncBuildVer(LPCTSTR lpszFilePath);

protected:
	DWORD m_dwOffset; // 有效文件内容相对文件开头的偏移字节数
	DWORD m_dwCode; // 文件的字符编码格式
	HANDLE m_hFile; // 目标文件句柄
	LPTSTR m_pContent; // 转换为 Unicode 编码格式后的文件内容的指针

	// 读取指定文件内容，并转换为 Unicode 编码
	bool GetFileContent(LPCTSTR lpszFilePath);

	// 获取该文件内容的编码格式
	bool GetFileCode(LPCSTR pData, LONGLONG llLen);
	// 判断给定的内容是否为utf-8编码
	bool IsCodeUtf8(LPCSTR pString, LONGLONG llLen);
	// 判断给定内容是否为utf-16格式的编码，如果是则返回字符编码，否则返回零
	int IsCodeUtf16(LPCSTR pString, LONGLONG llLen);

	// 从给定的版本号之前的字符串，并根据原文件编码方式和文件内容的偏移量，
	// 计算并设置文件指针的位置，跳过编译版本号之前不会被更改的内容，
	// 用于后续写入，并减少 I/O 提高效率
	bool SetFilePtrWithString(LPCTSTR lpszProBuildVer, DWORD dwLen);
	// 将给定的内容，转换为原文件编码格式后，写入文件
	bool WriteContent(LPCTSTR lpszContent);
};

