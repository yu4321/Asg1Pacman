#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <time.h>
#include "include/GL/freeglut.h"

using namespace std;

enum EntityTypes { none, player, enemy, fruit };



void GameOver();
void GameFinish();

int lastPressed = 0;
float timeLeft = 30;
int Width = 600, Height = 600;

float xMax = 10, yMax = 10;

int enemySpawnInterval = 5000;
int fruitSpawnInterval = 6000;

class Entity {
public:
	float x=0;
	float y=0;
	float speed=0;
	EntityTypes type;
	bool isAlive=false;

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

class Fruit : public Entity {
public:

	Fruit() {
		printf("created Fruit\n");
		isAlive = true;
	}
	virtual vector<Entity> Touched() {
		printf("call overrided touched from pacman\n");
		vector<Entity> v;

		for (auto x : v) {
			if (x.type != EntityTypes::fruit) {
				isAlive = false;
				break;
			}
		}

		return v;
	}

	virtual void Draw() {
		glTranslatef(x, y, 0.0);
		glLineWidth(3.0);

		glColor3f(0, 0, 0);

		glBegin(GL_LINE_STRIP);
		glVertex3f(0, 0.4, 0.0);
		glVertex3f(0, 0, 0.0);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);

		double rad = 0.3;
		glBegin(GL_POLYGON);

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
	float radius = 0.5;
	float lastDirection = 0;
	int fruitAte = 0;

	Pacman() {
		printf("created pacman\n");
		isAlive = true;
	}



	virtual vector<Entity> Touched(){
		//printf("call overrided touched from pacman\n");
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
		//printf("call overrided Inputed from pacman\n");
		float currentDirection = 0;

		if (lastPressed != 0) {
			switch (lastPressed)
			{
			case GLUT_KEY_UP:
				currentDirection = 90;
				break;
			case GLUT_KEY_DOWN:
				currentDirection = 270;
				break;
			case GLUT_KEY_LEFT:
				currentDirection = 180;
				break;
			case GLUT_KEY_RIGHT:
				currentDirection = 0;
				break;
			default:
				break;
			}

			if (currentDirection == lastDirection) {
				speed = (xMax+yMax)/30 ;
			}

			lastDirection = currentDirection;
			/*	if (lastPressed == GLUT_KEY_UP) {
					lastDirection = 0;
				}*/
			lastPressed = 0;
		}

		
	}

	virtual void Move() {
		float xOffset = 0;
		float yOffset = 0;


		if (lastDirection == 90) {
			//y += speed;
			yOffset = speed;
		}
		else if (lastDirection == 180) {
			//x -= speed;
			xOffset = -speed;
		}
		else if (lastDirection == 270) {
			//y -= speed;
			yOffset = -speed;
		}
		else if (lastDirection == 0) {
			//x += speed;
			xOffset = speed;
		}

		float nx = x + xOffset;
		float ny = y + yOffset;

		if (nx >= 0 and nx <= xMax) {
			x = nx;
		}

		if (ny >= 0 and ny <= yMax) {
			y = ny;
		}

		speed = 0;
	}

	virtual void Draw() {
		glTranslatef(x, y, 0.0);
		glRotatef(lastDirection, 0, 0, 1.0);
		glLineWidth(3.0);
	

		glColor3f(1.0f, 0.7f, 0);

		double rad = radius;
		GLfloat twicePi = 2.0f * 3.14;
		int triangleAmount = 360;
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(0, 0); // center of circle
		for (int i = 0; i <= triangleAmount; i++) {
			if (i > 30 && i<300) {
				glVertex2f(
					(radius * cos(i * twicePi / triangleAmount)),
					(radius * sin(i * twicePi / triangleAmount))
				);
			}
		
		}
		glEnd();
		
	}

};



Pacman* currentPlayer;
vector<Entity*> entities;


void GameOver() {

}

void GameFinish() {

}

void SpawnFruitAtRandom() {
	float x = 0;
	float y = 0;
	while (true) {
		float nx = rand() % (int)xMax;
		float ny = rand() % (int)yMax;

		x = nx;
		y = ny;

		break;
	}

	auto nfruit = new Fruit;
	nfruit->x = x;
	nfruit->y = y;

	entities.push_back(nfruit);
}

void SpawnEnemyAtRandom() {

}




// 콜백 함수 선언
void Render();
void Reshape(int w, int h);
void Timer(int id);

void SpawnFruit();

// 사용자 정의 함수
void PrintGLMatrix(GLenum pname);

// 전역 변수
float angle1 = 0.0;
float angle2 = 0.0;

float xOffset = 0;
float yOffset = 0;

void SpawnFruits(int id) {
	SpawnFruitAtRandom();
	glutTimerFunc(fruitSpawnInterval, SpawnFruits, 2);
}

void SpawnEnemies(int id) {
	SpawnFruitAtRandom();
	glutTimerFunc(enemySpawnInterval, SpawnEnemies, 2);
}


void MakeMaps() {
	for (int i = 0; i < 8; i++) {
		SpawnFruitAtRandom();
	}

	for (int i = 0; i < 3; i++) {
		SpawnEnemyAtRandom();
	}

	glutTimerFunc(fruitSpawnInterval, SpawnFruits, 2);
	glutTimerFunc(enemySpawnInterval, SpawnEnemies, 2);
}


// 메인 함수
int main(int argc, char** argv)
{
	srand(time(NULL));
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
	//glutTimerFunc(100, Timer, 1);
	glutTimerFunc(10, Timer, 2);


	glutSpecialFunc([](int k, int x, int y) 
		{
			if (k == GLUT_KEY_UP || k == GLUT_KEY_LEFT || k == GLUT_KEY_RIGHT || k == GLUT_KEY_DOWN) {
				lastPressed = k;
			}
		}
	);

	Pacman player;
	//Fruit testFruit;
	//testFruit.x = xMax / 2;
	//testFruit.y = yMax / 2;
	player.x = xMax / 2;
	player.y = yMax / 2;
	//entities.push_back(&testFruit);
	entities.push_back(&player);
	currentPlayer = &player;

	MakeMaps();
	// 메시지 처리 루푸 진입
	glutMainLoop();
	return 0;
}

void Timer(int id)
{
	glutPostRedisplay();
	glutTimerFunc(10, Timer, 2);
}
void Reshape(int w, int h)
{
	// View-port transformation
	glViewport(0, 0, w, h);
	Width = w;
	Height = h;
}

void Render()
{
	// 칼라 버퍼 초기화
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 관측 공간 지정
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(-xMax, xMax, -yMax, yMax);
	gluOrtho2D(0, xMax, 0, yMax);
	//gluOrtho2D(0,Width,0,Height);
	//PrintGLMatrix(GL_PROJECTION_MATRIX);

	// 관측 변환 설정
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//PrintGLMatrix(GL_MODELVIEW_MATRIX);

	// 좌표축 그리기
	//DrawGrid();

	for (auto x : entities) {
		if (x->isAlive) {
			x->Touched();
			x->Inputed();
			x->Move();
			glPushMatrix();
			{
				//glTranslatef(x->x, x->y, 0.0);
				//glTranslatef(xOffset, yOffset, 0.0);
				x->Draw();
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
