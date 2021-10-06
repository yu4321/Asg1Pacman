#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include "include/GL/freeglut.h"

using namespace std;

enum EntityTypes { none, player, enemy, fruit };



void GameOver();
void GameFinish();

int lastPressed = 0;
float timeLeft = 30;

class Entity {
public:
	float x;
	float y;
	float speed;
	EntityTypes type;
	bool isAlive;

	virtual vector<Entity> Touched() {
		vector<Entity> v;
		return v;
	}

	virtual void Inputed() {

	}

	virtual void Move() {
		//x += speed;
		//y += speed;
	}

	virtual void Draw() {
		glLineWidth(3.0);
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 1.0, 0.0);
		glVertex3f(-1.0, 0.0, 0.0);

		glVertex3f(-1.0, 0.0, 0.0);
		glVertex3f(0.5, 0.0, 0.0);

		glVertex3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, -0.7, 0.0);

		glEnd();

		double rad = 0.5;
		glBegin(GL_LINE_STRIP);

		for (int i = 0; i < 360; i++) {
			double angle, x, y;
			angle = i * 3.141592 / 180;
			x = rad * cos(angle);
			y = rad * sin(angle);
			glVertex2f(x, y);
		}
		glEnd();
	}
};

class Pacman :public Entity {
public:
	float radius = 0.3;
	float lastDirection = 0;
	int fruitAte = 0;

	Pacman() {
		printf("created pacman");
		isAlive = true;
	}

	virtual vector<Entity> Touched() {
		printf("call overrided touched from pacman");
		vector<Entity> v;

		for (auto x : v) {
			if (x.type == EntityTypes::fruit) {
				fruitAte++;
				radius += 0.1;
			}
			else if (x.type == EntityTypes::enemy) {
				isAlive = false;
				GameOver();
				break;
			}
		}

		return v;
	}

	virtual void Inputed() {
		printf("call overrided Inputed from pacman");
		float currentDirection = 0;

		switch (lastPressed)
		{
		case GLUT_KEY_UP:
			break;
		case GLUT_KEY_DOWN:
			currentDirection = 180;
			break;
		case GLUT_KEY_LEFT:
			currentDirection = 90;
			break;
		case GLUT_KEY_RIGHT:
			currentDirection = 270;
			break;
		default:
			break;
		}

		if (currentDirection == lastDirection) {
			speed = 0.5;
		}

		lastDirection = currentDirection;
		/*	if (lastPressed == GLUT_KEY_UP) {
				lastDirection = 0;
			}*/
	}

	virtual void Move() {
		printf("call overrided Move from pacman");

		if (lastDirection == 0) {
			y += speed;
		}
		else if (lastDirection == 90) {
			x += speed;
		}
		else if (lastDirection == 180) {
			y -= speed;
		}
		else if (lastDirection == 270) {
			x -= speed;
		}
	}

	virtual void Draw() {
		printf("call overrided Draw from pacman");

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.0f, 0.0f, 1.0f);

		double rad = radius;

		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++)
		{
			double angle = i * 3.141592 / 180;
			double x = rad * cos(angle);
			double y = rad * sin(angle);
			glVertex2f(x, y);
		}
		glEnd();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0f, 0.0f, 0.0f);

		glBegin(GL_LINE);
		glVertex3f(-1.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();

		glBegin(GL_LINE);
		glVertex3f(1.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();

		glFinish();
	}

};

Pacman currentPlayer;
vector<Entity> entities;


void GameOver() {

}

void GameFinish() {

}


int Width = 400, Height = 400;

// 콜백 함수 선언
void Render();
void Reshape(int w, int h);
void Timer(int id);

// 사용자 정의 함수
void PrintGLMatrix(GLenum pname);

// 전역 변수
float angle1 = 0.0;
float angle2 = 0.0;

float xOffset = 0;
float yOffset = 0;




// 메인 함수
int main(int argc, char** argv)
{
	// Freeglut 초기화
	glutInit(&argc, argv);

	// 윈도우 크기 및 생성 위치 설정
	glutInitWindowSize(Width, Height);
	glutInitWindowPosition(0, 0);

	// 디스플레이 모드(칼라버퍼) 설정
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	// 윈도우 생성
	glutCreateWindow("PacMan");

	// 콜백 함수 등록
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	/*glutTimerFunc(100, Timer, 1);
	glutTimerFunc(10, Timer, 2);*/

	/*glutKeyboardFunc([](unsigned char c, int x, int y){
		switch (c)
		{
		case 'w':
			yOffset++;
			break;
		case 's':
			yOffset--;
			break;
		case 'a':
			xOffset--;
			break;
		case 'd':
			xOffset++;
			break;
		default:
			break;
		}
	});*/

	glutSpecialFunc([](int k, int x, int y) 
		{
			if (k == GLUT_KEY_UP || k == GLUT_KEY_LEFT || k == GLUT_KEY_RIGHT || k == GLUT_KEY_DOWN) {
				lastPressed = k;
			}
		}
	);

	Pacman player;
	entities.push_back(player);
	currentPlayer = player;
	// 메시지 처리 루푸 진입
	glutMainLoop();
	return 0;
}

void Timer(int id)
{
	if (id == 1)
	{
		angle1++;
		glutPostRedisplay();
		glutTimerFunc(100, Timer, 1);
	}

	if (id == 2)
	{
		angle2++;
		glutPostRedisplay();
		glutTimerFunc(10, Timer, 2);
	}
}
void Reshape(int w, int h)
{
	// View-port transformation
	glViewport(0, 0, w, h);
	Width = w;
	Height = h;
}

void DrawGrid()
{
	glLineWidth(3.0);
	glColor3f(0.0, 0.0, 0.0);

	glBegin(GL_LINES);
	glVertex3f(-5.0, 0.0, 0.0);
	glVertex3f(5.0, 0.0, 0.0);
	glVertex3f(0.0, -5.0, 0.0);
	glVertex3f(0.0, 5.0, 0.0);
	glEnd();

	glLineWidth(1.0);
	for (int i = -5; i < 6; ++i)
	{
		glBegin(GL_LINES);
		glVertex3f(i, -5.0, 0.0);
		glVertex3f(i, 5.0, 0.0);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(-5.0, i, 0.0);
		glVertex3f(5.0, i, 0.0);
		glEnd();
	}
}

void DrawObject(float r, float g, float b)
{
	glLineWidth(3.0);
	glColor3f(r, g, b);
	glBegin(GL_LINES);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(-1.0, 0.0, 0.0);

	glVertex3f(-1.0, 0.0, 0.0);
	glVertex3f(0.5, 0.0, 0.0);

	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, -0.7, 0.0);

	glEnd();

	double rad = 0.5;
	glBegin(GL_LINE_STRIP);

	for (int i = 0; i < 360; i++) {
		double angle, x, y;
		angle = i * 3.141592 / 180;
		x = rad * cos(angle);
		y = rad * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();


}

void Render()
{
	// 칼라 버퍼 초기화
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 관측 공간 지정
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-5.0, 5.0, -5.0, 5.0);
	//PrintGLMatrix(GL_PROJECTION_MATRIX);

	// 관측 변환 설정
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//PrintGLMatrix(GL_MODELVIEW_MATRIX);

	// 좌표축 그리기
	//DrawGrid();

	for (auto x : entities) {
		if (x.isAlive) {
			x.Touched();
			x.Inputed();
			x.Move();
			glPushMatrix();
			{
				glTranslatef(xOffset, yOffset, 0.0);
				x.Draw();
				//DrawObject(1.0, 0.0, 0.0);
			}
			glPopMatrix();
		}
	}

	//// Transformation example
	//glPushMatrix();
	//{
	//	glTranslatef(xOffset, yOffset, 0.0);
	//	//glRotatef(angle1, 0.0, 0.0, 1.0);
	//	DrawObject(1.0, 0.0, 0.0);
	//}
	//glPopMatrix();

	//glPushMatrix();
	//{
	//	glTranslatef(2.0, 0.0, 0.0);
	//	//glRotatef(angle2, 0.0, 0.0, 1.0);
	//	DrawObject(0.0, 0.0, 1.0);
	//}
	//glPopMatrix();

	// 렌더링 완료
	glutSwapBuffers();
}

void PrintGLMatrix(GLenum pname)
{
	float m[16];
	glGetFloatv(pname, m);

	/*printf("|%7.1f%7.1f%7.1f%7.1f|\n", m[0], m[4], m[8], m[12]);
	printf("|%7.1f%7.1f%7.1f%7.1f|\n", m[1], m[5], m[9], m[13]);
	printf("|%7.1f%7.1f%7.1f%7.1f|\n", m[2], m[6], m[10], m[14]);
	printf("|%7.1f%7.1f%7.1f%7.1f|\n\n", m[3], m[7], m[11], m[15]);*/
}
