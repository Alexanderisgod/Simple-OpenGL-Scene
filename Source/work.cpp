// work.cpp : �������̨Ӧ�ó������ڵ㡣

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
	
	glutCreateWindow("������ʻģ��");

	InitGL();
	
	glutSpecialFunc(SpecialKeys);  //ע�Ṧ�ܼ��ص�����  
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

