#pragma once

#include "stdafx.h"

#include <windows.h>  
#include <math.h>  
#include <stdlib.h>
#include <time.h>


static GLfloat dist = 0.0f;
GLfloat relX = 0.0f;//车辆相对x轴的偏移
GLfloat speed = 1.0f;
GLfloat carRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat cameraX = 0.0f;
static GLfloat cameraY = 0.0f;
static GLfloat cameraZ = 0.0f;
GLuint texture[3];

GLboolean run = false;//启动或停止
GLboolean reset = false;


#define TREE 6
#define STEM 1
#define LEAF 2
#define LEAF_MAT 3
#define TREE_MAT 4
#define STEMANDLEAVES 5

const double PI = 3.14159265;
int Rebuild = 1;
int Level = 6;
float treePos[3] = { 0, -2, 4 };

//生成随机数
inline float randf()
{
	return (rand() / (float)RAND_MAX);
}

//生成树木模型的显示列表
void FractalTree(int level)
{
	long savedseed;
	if (level == Level)
	{
		glPushMatrix();
		glRotatef(60 + randf() * 120, 0, 1, 0);
		glCallList(STEMANDLEAVES);
		glPopMatrix();
	}
	else
	{
		glCallList(STEM);

		glPushMatrix();
		glTranslatef(0, 1, 0);
		glScalef(0.7, 0.7, 0.7);

		savedseed = rand();
		glPushMatrix();
		glRotatef(0 + randf() * 90, 0, 1, 0);
		glRotatef(30 + randf() * 30, 0, 0, 1);
		FractalTree(level + 1);
		glPopMatrix();

		srand(savedseed);
		savedseed = rand();
		glPushMatrix();
		glRotatef(180 + randf() * 90, 0, 1, 0);
		glRotatef(30 + randf() * 30, 0, 0, 1);
		FractalTree(level + 1);
		glPopMatrix();

		srand(savedseed);
		savedseed = rand();
		glPushMatrix();
		glRotatef(90 + randf() * 90, 0, 1, 0);
		glRotatef(30 + randf() * 30, 0, 0, 1);
		FractalTree(level + 1);
		glPopMatrix();

		glPopMatrix();
	}
}

//生成树木的显示列表
void CreateTreeLists(void)
{
	GLUquadricObj *cylquad = gluNewQuadric();
	int i;
	//树干
	glNewList(STEM, GL_COMPILE);
	glCallList(TREE_MAT);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluCylinder(cylquad, 0.1, 0.08, 1, 10, 2);
	glPopMatrix();
	glEndList();
	//树叶
	glNewList(LEAF, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	glNormal3f(-0.1, 0, 0.25);
	glVertex3f(0, 0, 0);
	glVertex3f(0.5, 0.5, 0.2);
	glVertex3f(0, 1.0, 0);
	glNormal3f(0.2, 0, 0.5);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1.0, 0);
	glVertex3f(-0.5, 0.5, 0.2);
	glEnd();
	glEndList();

	glNewList(STEMANDLEAVES, GL_COMPILE);
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glCallList(STEM);
	glCallList(LEAF_MAT);
	for (i = 0; i < 2; i++)//3的话，内存占用太大
	{
		glTranslatef(0, 0.333, 0);
		glRotatef(90, 0, 1, 0);
		glPushMatrix();
		glRotatef(0, 0, 1, 0);
		glRotatef(50, 1, 0, 0);
		glCallList(LEAF);
		glPopMatrix();
		glPushMatrix();
		glRotatef(180, 0, 1, 0);
		glRotatef(60, 1, 0, 0);
		glCallList(LEAF);
		glPopMatrix();
	}
	glPopAttrib();
	glPopMatrix();
	glEndList();

	gluDeleteQuadric(cylquad);
}

//设置树木的纹理
void SetupMaterials(void)
{
	GLfloat tree_ambuse[] = { 0.4f, 0.25f, 0.1f, 1.0f };
	GLfloat tree_specular[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat tree_shininess[] = { 10 };

	GLfloat leaf_ambuse[] = { 0.0f, 0.8f, 0.0f, 1.0f };
	GLfloat leaf_specular[] = { 0.0f, 0.8f, 0.0f, 1.0f };
	GLfloat leaf_shininess[] = { 10 };

	glNewList(TREE_MAT, GL_COMPILE);
	glColor3f(.4f, .25f, .1f);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tree_ambuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, tree_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, tree_shininess);
	glEndList();

	glNewList(LEAF_MAT, GL_COMPILE);
	glColor3f(.0f, .8f, .0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, leaf_ambuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, leaf_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, leaf_shininess);
	glEndList();
}

//绘制车辆
void DrawCar() {
	glPushMatrix();

	//绘制汽车	
	if (run) {
		relX -= speed*sin(-yRot / 180 * PI);//车辆相对X轴的偏移
		dist = dist - speed + speed*cos(-yRot / 180 * PI);
	}

	//车辆撞上路边
	if (abs(relX) >= 28) {
		if (dist <= 512 - 40 || dist >= 512 + 20) {
			run = false;
			carRot = 180;
		}
	}

	//越界复原汽车，达到循环驾驶
	if (abs(relX) >= 60 || dist > 1000) {
		//功能测试
		if (relX >= 40) {
			yRot -= 90.0;
		}
		else if (relX <= -40) {
			yRot += 90.0;
		}
		if (carRot > 360)
			carRot -= 360;
		else if (carRot < 0)
			carRot += 360;
		relX = 0;
		dist = 0;
	}

	glTranslatef(relX, 0.0f, -dist);

	glTranslatef(0.0f, -25.0f, 70.0f);
	glRotatef(270, 0.0f, 1.0f, 0.0f);//保持汽车方位正常
	glRotatef(carRot, 1.0f, .0f, 0.0f);//汽车反转
	glRotatef(yRot, 0.0f, -1.0f, 0.0f);

	//缩放车辆
	glScalef(0.2, 0.2, 0.2);

	glBegin(GL_POLYGON);
	glColor3f(0.1, 0.1, 0.8);
	glVertex3f(-40.0f, 0.0f, 15.0f);
	glVertex3f(40.0f, 0.0f, 15.0f);
	glColor3f(1, 0.5, 0.8);
	glVertex3f(40.0f, -15.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.3, 0.2, 0.5);
	glVertex3f(40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, 15.0f);
	glVertex3f(40.0f, -15.0f, 15.0f);
	glVertex3f(40.0f, -15.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.3, 0.1, 0.3);
	glVertex3f(-40.0f, 0.0f, -15.0f);
	glVertex3f(-40.0f, 0.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.1, 0.1, 0.8);
	glVertex3f(-40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, -15.0f);
	glColor3f(1, 0.5, 0.8);
	glVertex3f(40.0f, -15.0f, -15.0f);
	glVertex3f(-40.0f, -15.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 1);
	glVertex3f(-40.0f, 0.0f, 15.0f);
	glVertex3f(-40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.8, 0.5, 0.2);
	glVertex3f(-40.0f, -15.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, -15.0f);
	glVertex3f(40.0f, -15.0f, -15.0f);
	glVertex3f(40.0f, -15.0f, 15.0f);
	glEnd();
	//车上半部  
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 1);
	glVertex3f(-20.0f, 0.0f, 15.0f);
	glVertex3f(-10.0f, 10.0f, 15.0f);
	glVertex3f(20.0f, 10.0f, 15.0f);
	glVertex3f(25.0f, 0.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 1);
	glVertex3f(-20.0f, 0.0f, -15.0f);
	glVertex3f(-10.0f, 10.0f, -15.0f);
	glVertex3f(20.0f, 10.0f, -15.0f);
	glVertex3f(25.0f, 0.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0, 1, 1);
	glVertex3f(-10.0f, 10.0f, 15.0f);
	glVertex3f(-10.0f, 10.0f, -15.0f);
	glVertex3f(20.0f, 10.0f, -15.0f);
	glVertex3f(20.0f, 10.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0.5, 0.8, 0.8);
	glVertex3f(-10.0f, 10.0f, 15.0f);
	glVertex3f(-20.0f, 0.0f, 15.0f);
	glVertex3f(-20.0f, 0.0f, -15.0f);
	glVertex3f(-10.0f, 10.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0, 0.5, 0.5);
	glVertex3f(20.0f, 10.0f, 15.0f);
	glVertex3f(20.0f, 10.0f, -15.0f);
	glVertex3f(25.0f, 0.0f, -15.0f);
	glVertex3f(25.0f, 0.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0, 0, 1);
	glVertex3f(-30.0f, -15.0f, 15.0f);
	glVertex3f(-30.0f, -15.0f, -15.0f);
	glVertex3f(30.0f, -15.0f, -15.0f);
	glVertex3f(30.0f, -15.0f, 15.0f);
	glEnd();
	//车轮  
	glColor3f(1, 0, 0);
	glTranslated(-20.0f, -15.0f, 15.0f);
	glutSolidTorus(2, 5, 5, 100);
	glTranslated(0.0f, 0.0f, -30.0f);
	glutSolidTorus(2, 5, 5, 100);
	glTranslated(45.0f, 0.0f, 0.0f);
	glutSolidTorus(2, 5, 5, 100);
	glTranslated(0.0f, 0.0f, 30.0f);
	glutSolidTorus(2, 5, 5, 100);

	glPopMatrix();
}

//街道绘制和贴图
void render() {
	glBindTexture(GL_TEXTURE_2D, texture[0]);               // Select Our Texture

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);

	//屏幕面
	//glTexCoord2f(.0f, .0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	//glTexCoord2f(1.0f, .0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	//glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
	//glTexCoord2f(.0f, 1.0f); glVertex3f(1.0f, -1.0f, 0.0f);

	//xoz面,道路
	GLfloat x = 100;
	//设置路面长度1024;
	for (int i = 0; i < 2; i++) {
		glTexCoord2f(.0f, .0f); glVertex3f(-30.0f, -30.0f, x - 256);
		glTexCoord2f(1.0f, .0f); glVertex3f(-30.0f, -30.0f, x);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, -30.0f, x);
		glTexCoord2f(.0f, 1.0f); glVertex3f(30.0f, -30.0f, x - 256);
		x = x - 256;
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[1]);               // Select Our Texture
	glBegin(GL_QUADS);
	x = 100;
	for (int i = 0; i < 4; i++) {
		//道路旁的
		glTexCoord2f(.0f, .0f); glVertex3f(30.0f, -31.0f, x - 128.0);
		glTexCoord2f(1.0f, .0f); glVertex3f(30.0f, -27.0f, x - 128.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, -27.0f, x);
		glTexCoord2f(.0f, 1.0f); glVertex3f(30.0f, -31.0f, x);

		glTexCoord2f(.0f, .0f); glVertex3f(-30.0f, -31.0f, x - 128.0);
		glTexCoord2f(1.0f, .0f); glVertex3f(-30.0f, -27.0f, x - 128.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-30.0f, -27.0f, x);
		glTexCoord2f(.0f, 1.0f); glVertex3f(-30.0f, -31.0f, x);
		x = x - 128;
	}
	glEnd();

	//草地
	glBegin(GL_QUADS);
	glColor3f(0.8f, 1.0f, 0.0f);
	glTexCoord2f(.0f, .0f); glVertex3f(30.0f, -27.0f, 100);
	glTexCoord2f(1.0f, .0f); glVertex3f(30.0f, -27.0f, -256 * 2 + 100);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(100.0f, -27.0f, -256 * 2 + 100);
	glTexCoord2f(.0f, 1.0f); glVertex3f(100.0f, -27.0f, 100);

	glTexCoord2f(.0f, .0f); glVertex3f(-30.0f, -27.0f, 100);
	glTexCoord2f(1.0f, .0f); glVertex3f(-30.0f, -27.0f, -256 * 2 + 100);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-100.0f, -27.0f, -256 * 2 + 100);
	glTexCoord2f(.0f, 1.0f); glVertex3f(-100.0f, -27.0f, 100);
	glEnd();


	//绘制树木, 批量
	for (int j = 0; j < 2; j++) {
		int k1 = 1, k2 = 60;
		if (j == 1) {
			k1 = -1;
			k2 = 40;
		}
		for (int i = 0; i < 3; i++) {
			glPushMatrix();
			glTranslatef(-35.0f*k1, -26.0f, k2 - 200 * i);

			glScalef(8, 8, 8);

			glCallList(TREE);
			glPopMatrix();
		}
	}

}

//键盘控制车辆加减速，左右转动，启停
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)	speed += 0.5;
	else if (key == GLUT_KEY_DOWN) speed -= 0.5f;
	else if (key == GLUT_KEY_LEFT) yRot -= 5.0f;
	else if (key == GLUT_KEY_RIGHT) yRot += 5.0f;
	if (yRot >= 365.0) yRot = -365.0;
	if (yRot < -1.0) yRot += 365.0;
	glutPostRedisplay();
}

//控制摄像头的转动
void NormalKeys(unsigned char key, int x, int y) {
	reset = false;
	//摄像机上下移动
	if (key == 'w' || key == 'W') {
		cameraY += 5;
	}
	else if (key == 's' || key == 'S') {
		cameraY -= 5;
	}
	else if (key == 'd' || key == 'D') {
		cameraX += 5;
	}
	else if (key == 'a' || key == 'A') {
		cameraX -= 5;
	}
	else if (key == 'm' || key == 'M') {
		cameraZ += 5;
	}
	else if (key == 'n' || key == 'N') {
		cameraZ -= 5;
	}
	//enter键复位
	else if (key == 'r' || key == 'R') {
		carRot = 0;
		yRot = 0;
		//恢复原状
		relX = 0; dist = 0;
		//摄像机复位
		cameraX = 0; cameraY = 0; cameraZ = 0;
	}
	else if (key == ' ') {
		run = !run;
		if (run)
			speed = 1.0;
	}

	glutPostRedisplay();
}

//鼠标左击控制追随车辆视角
void MouseFunc(int button, int state, int x, int y) {
	//左击鼠标
	if (state == GLUT_DOWN&&button == GLUT_LEFT_BUTTON) {
		//恢复到小车的后方
		reset = true;
	}
	glutPostRedisplay();
}

//加载纹理
AUX_RGBImageRec *LoadBMP(char *filename) {
	FILE* file = NULL;

	if (!filename) {
		return NULL;
	}
	fopen_s(&file, filename, "r");
	if (file) {
		fclose(file);
		return auxDIBImageLoadA(filename);
	}
	return NULL;
}

int LoadTexures() {
	int status = FALSE;
	AUX_RGBImageRec *TextureImage[3];
	memset(TextureImage, 0, sizeof(void *) * 3);
	char* bmpName[3] = { "ground.bmp","wand.bmp","sun.bmp" };

	for (int i = 0; i < 3; i++) {
		if (TextureImage[i] = LoadBMP(bmpName[i])) {
			status = TRUE;
			glGenTextures(1, &texture[i]);
			glBindTexture(GL_TEXTURE_2D, texture[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[i]->sizeX, TextureImage[i]->sizeY,
				0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[i]->data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		}
		if (TextureImage[i]) {
			if (TextureImage[i]->data) {
				free(TextureImage[i]->data);
			}
			free(TextureImage[i]);
		}
	}
	return status;
}

//改变窗口
void reshape(int widht, int height) {
	glViewport(0, 0, (GLsizei)widht, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, widht / height, 1, 800);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-10.0, -5.0, 120.0, 0.0, -30.0, -30.0, 0.0f, 1.0f, .0);
}

//初始化
int InitGL(GLvoid)                              // All Setup For OpenGL Goes Here
{
	if (!LoadTexures())                          // Jump To Texture Loading Routine ( NEW )
	{
		return FALSE;                           // If Texture Didn't Load Return FALSE ( NEW )
	}
	glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping ( NEW )
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // Really Nice Perspective Calculations

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 0.1f, 100.0f);

	//生成树叶
	glNewList(TREE, GL_COMPILE);
	FractalTree(0);
	glEndList();

	return TRUE;
}

//绘制
void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (!reset)
		gluLookAt(-10.0 + cameraX, -5.0 + cameraY, 120.0 - dist + cameraZ, 0.0, -30.0, -30.0 - dist, 0.0f, 1.0f, .0f);
	else {
		//-sin(yRot / 180 * PI)//视线转动角度
		gluLookAt(-10 + relX, -5.0f, 120.0 - dist, 0.0 + relX, -30.0, -30.0f - dist, 0.0f, 1.0f, 0.0f);
		cameraX = relX; cameraY = 0; cameraZ = 0;
	}

	//绘制太阳
	glPushMatrix();
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	gluQuadricTexture(qobj, GL_TRUE);
	glTranslatef(-60.0f, 120.0f, -600.0f);
	gluSphere(qobj, 10, 100, 100);

	glPopMatrix();
	//十字路口前面的街道
	render();

	//十字路口中间的街道
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 2; i++) {
		glTexCoord2f(1.0f, .0f); glVertex3f(100.0f, -30.0f, -512 + 100);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-100.0f, -30.0f, -512 + 100);
		glTexCoord2f(.0f, 1.0f); glVertex3f(-100.0f, -30.0f, -512 + 40);
		glTexCoord2f(.0f, .0f); glVertex3f(100.0f, -30.0f, -512 + 40);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[1]);               // Select Our Texture
	glBegin(GL_QUADS);
	//十字路口左侧的路边
	glTexCoord2f(.0f, .0f); glVertex3f(-100.0f, -31.0f, -512 + 100);
	glTexCoord2f(1.0f, .0f); glVertex3f(-30.0f, -31.0f, -512 + 100);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-30.0f, -27.0f, -512 + 100);
	glTexCoord2f(.0f, 1.0f); glVertex3f(-100.0f, -27.0f, -512 + 100);

	glTexCoord2f(.0f, .0f); glVertex3f(-100.0f, -31.0f, 40 - 512);
	glTexCoord2f(1.0f, .0f); glVertex3f(-30.0f, -31.0f, 40 - 512);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-30.0f, -27.0f, 40 - 512);
	glTexCoord2f(.0f, 1.0f); glVertex3f(-100.0f, -27.0f, 40 - 512);

	//十字路口右侧的路边
	glTexCoord2f(.0f, .0f); glVertex3f(100.0f, -31.0f, -512 + 100);
	glTexCoord2f(1.0f, .0f); glVertex3f(30.0f, -31.0f, -512 + 100);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, -27.0f, -512 + 100);
	glTexCoord2f(.0f, 1.0f); glVertex3f(100.0f, -27.0f, -512 + 100);

	glTexCoord2f(.0f, .0f); glVertex3f(100.0f, -31.0f, 40 - 512);
	glTexCoord2f(1.0f, .0f); glVertex3f(30.0f, -31.0f, 40 - 512);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, -27.0f, 40 - 512);
	glTexCoord2f(.0f, 1.0f); glVertex3f(100.0f, -27.0f, 40 - 512);
	glEnd();


	//十字路口后面的街道
	glPushMatrix();
	glTranslatef(0.0, 0.0, -512 - 60);
	render();
	glPopMatrix();

	DrawCar();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1.0, 1, 800);

	//每次画完后，模拟车运动
	if (run)
		dist += speed;

	glutSwapBuffers();
}

//循环绘制， 60frames
void idleFunc() {
	Sleep(17);
	Display();
}