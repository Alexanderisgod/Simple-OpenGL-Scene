// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

//
//使用纹理贴图，增强可视效果
//应用坐标变换，实现场景中不同物体重建
//采用双缓冲技术，实现场景实时绘制
//具有一定的鼠标、键盘交互功能

#pragma once
//#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib ")

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <time.h>
#include <vector>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\GLAUX.H>
#include <gl\glut.h>

using namespace std;

// TODO:  在此处引用程序需要的其他头文件
