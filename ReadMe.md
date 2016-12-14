##项目说明
该项目用于 Visual Studio 编译项目时，自动增加编译版本号。
 
##使用方法
1.将该项目编译出的可执行文件：IncBuildVer.exe 放入系统目录（Windows或System32）下；

2.在你项目中的资源宏声明文件（resource.h）的开头添加：

    #define MAJOR_VER_NUM					1 // 主版本号
	#define MINOR_VER_NUM					0 // 子版本号
	#define REVISION_VER_NUM				0 // 修正版本号
	#define BUILD_VER_NUM					0 // 编译版本号

3.在项目属性“项目 - 属性 - 配置属性 - 生成事件 - 预先生成事件”的“命令行”添加如下：

    IncBuildVer $(Configuration) $(ProjectDir)
    
##修改纪录

2016-12-14：重构项目，更改自增编译版本号的实现方法，不需要向目标项目额外添加文件

2016-12-12：修复切换编译类型后版本号仍会增加的漏洞。

2016-12-10：完善程序功能，支持多种编码格式。

##License
[MIT](https://github.com/songbaoming/IncBuildVer/blob/master/LICENSE)
