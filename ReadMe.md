## 项目用途
该项目用于 Visual Studio 编译项目时，自动增加编译版本号。
 
## 使用方法
1.将该项目编译出的可执行文件：IncBuildVer.exe 放入系统目录（Windows或System32）下；

2.在项目属性“菜单栏 - 项目 - 属性”中切换到要使用的编译类型（Debug/Release），然后在右侧“配置属性 - 生成事件 - 预先生成事件”的“命令行”添加如下：

    IncBuildVer $(ProjectDir)
    
## 使用说明

1.程序会自动在你项目中的资源宏声明文件（resource.h）的开头添加：

    #define MAJOR_VER_NUM					0
	#define MINOR_VER_NUM					0
	#define REVISION_VER_NUM				0
	#define BUILD_VER_NUM					1

2.如果想在代码中或在界面上使用当前版本号，只需包含“resource.h”头文件，然后直接使用上述宏；

3.手动编辑版本号时，只需要修改头文件“resource.h”中对应宏即可，程序会在这些宏对应值的基础上更新BUILD_VER_NUM及*.rc中的版本字段。

## 修改纪录

2018-02-03：增加自动添加版本宏的功能，不需要用户再手动添加。

2016-12-25：精简逻辑，去掉编译类型的判断，由用户决定在哪种编译类型下使用。

2016-12-14：重构项目，更改自增编译版本号的实现方法。不需要向目标项目额外添加文件，也不用在vs中编辑对话框等资源后重新编辑*.rc文件。

2016-12-12：修复切换编译类型后版本号仍会增加的漏洞。

2016-12-10：完善程序功能，支持多种编码格式。

## License
[MIT](https://github.com/songbaoming/IncBuildVer/blob/master/LICENSE)
