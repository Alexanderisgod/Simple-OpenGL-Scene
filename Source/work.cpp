// work.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"
#include "work.h"


GLint viewSizex = 200;
GLint viewSizey = 200;
GLint viewSizez = 200;

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH| GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 600);
	
	glutCreateWindow("汽车驾驶模拟");

	InitGL();
	
	glutSpecialFunc(SpecialKeys);  //注册功能键回调函数  
	glutKeyboardFunc(NormalKeys);
	glutMouseFunc(MouseFunc);
	glutDisplayFunc(Display);
	glutIdleFunc(idleFunc);
	glutReshapeFunc(reshape);
	glClearColor(0.6, 0.6, 1.0, 1);

	glEnable(GL_NORMALIZE);

	SetupMaterials();
	CreateTreeLists();

	glutMainLoop();
	return 0;
}

