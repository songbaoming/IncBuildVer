#pragma once

// °æ±¾ºÅ
#define MARJOR_VER_NUM					1 // Ö÷°æ±¾ºÅ
#define MINOR_VER_NUM					0 // ´Î°æ±¾ºÅ
#define REVISION_VER_NUM				0 // ÐÞÕý°æ±¾ºÅ
#define BUILD_VER_NUM					4 // ±àÒë°æ±¾ºÅ


// °æ±¾ºÅ×Ö·û´®
#define TOSTR0(arg) #arg
#define TOSTR(arg) TOSTR0(arg)
#define VERSION_STR TOSTR(MARJOR_VER_NUM) "." TOSTR(MINOR_VER_NUM) "." TOSTR(REVISION_VER_NUM) "." TOSTR(BUILD_VER_NUM)
