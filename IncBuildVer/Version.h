#pragma once


#define MARJOR_VER_NUM					1
#define MINOR_VER_NUM					0
#define REVISION_VER_NUM				0
#define BUILD_VER_NUM					1


#define TOSTR0(arg) #arg
#define TOSTR(arg) TOSTR0(arg)
#define VERSION_STR TOSTR(MARJOR_VER_NUM) "." TOSTR(MINOR_VER_NUM) "." TOSTR(REVISION_VER_NUM) "." TOSTR(BUILD_VER_NUM)
