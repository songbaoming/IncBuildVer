//////////////////////////////////////////////////////////////
// 文件名：Version.h
// 作者：Song Baoming,2016
// 用途：定义程序版本号的宏，用于编译时自动增加编译版本号
// License：https://github.com/songbaoming/IncBuildVer/blob/master/LICENSE
//////////////////////////////////////////////////////////////

#pragma once

// 版本号
#define MARJOR_VER_NUM					1 // 主版本号
#define MINOR_VER_NUM					0 // 次版本号
#define REVISION_VER_NUM				0 // 修正版本号
#define BUILD_VER_NUM					4 // 编译版本号


// 版本号字符串
#define TOSTR0(arg) #arg
#define TOSTR(arg) TOSTR0(arg)
#define VERSION_STR TOSTR(MARJOR_VER_NUM) "." TOSTR(MINOR_VER_NUM) "." TOSTR(REVISION_VER_NUM) "." TOSTR(BUILD_VER_NUM)
