#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <time.h>
#include <algorithm>
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
	float radius = 0;
	EntityTypes type;
	vector<Entity*>* allEntities;
	bool isAlive=false;

	virtual vector<Entity*> Touched() {
		vector<Entity*> v;

		for (auto e : *allEntities) {
			if (e->type == type) {
				continue;
			}

			if (e->isAlive == false || isAlive == false) {
				continue;
			}

			float dx = x - e->x;
			float dy = y - e->y;

			float length = sqrt((dx * dx) + (dy * dy));

			if (length <= (radius + e->radius)) {
				v.push_back(e);
			}
		}
		return v;
	}

	virtual void Inputed() {

	}

	virtual void Move() {
		//x += speed;
		//y += speed;
	}

	virtual void Kill() {
		isAlive = false;
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

	Fruit(vector<Entity*>* es) {
		printf("created Fruit\n");
		type = EntityTypes::fruit;
		radius = 0.3;
		allEntities = es;
		isAlive = true;
	}
	/*virtual vector<Entity> Touched() {
		printf("call overrided touched from pacman\n");
		vector<Entity> v = Entity::Touched();

		for (auto x : v) {
			if (x.type != EntityTypes::fruit) {
				isAlive = false;
				break;
			}
		}

		return v;
	}*/

	virtual void Draw() {
		glTranslatef(x, y, 0.0);
		glLineWidth(3.0);

		glColor3f(0, 0, 0);

		glBegin(GL_LINE_STRIP);
		glVertex3f(0, 0.4, 0.0);
		glVertex3f(0, 0, 0.0);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);

		double rad = radius;
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

class Enemy : public Entity {
public:
	float lastDirection = 0;
	int fruitAte = 0;
	Entity* target=NULL;

	Enemy(vector<Entity*>* es) {
		printf("created enemy\n");
		allEntities = es;
		
		type = EntityTypes::enemy;
		radius = 0.3;
		speed = 0.01;
		isAlive = true;
	}

	virtual vector<Entity*> Touched() {
		vector<Entity*> v=Entity::Touched();

		for (auto x : v) {
			if (x->type == EntityTypes::fruit) {
				x->Kill();
				fruitAte++;
				speed *= 1.3;
			}
			else if (x->type == EntityTypes::player) {
				break;
			}
		}

		return v;
	}

	virtual void Inputed() {
		
		for (auto xx : *allEntities) {
			if (xx->type == EntityTypes::player) {
				target = xx;
			}
		}

		if (target != NULL) {
			float dx = x - target->x;
			float dy = y - target->y;

			float ang = atan2(dy, dx);
			lastDirection = ang * 180 / 3.14;
			printf("enemy angle : % f\n", lastDirection);
		}

		//find player and turn to that
	}

	virtual void Move() {
		
		float dx = target->x - x;
		float dy = target->y - y;

		float hyp= sqrt((dx * dx) + (dy * dy));

		dx /= hyp;
		dy /= hyp;

		x += dx * speed;
		y += dy * speed;
	}

	virtual void Draw() {
		glTranslatef(x, y, 0.0);
		glRotatef(90, 0, 0, 1.0);
		glRotatef(lastDirection, 0, 0, 1.0);
		glLineWidth(3.0);

	


		glColor3f(0.2*fruitAte, 0, 0.5);

		double rad = radius;
		glBegin(GL_POLYGON);

		for (int i = 0; i < 360; i++) {
			double angle, x, y;
			angle = i * 3.141592 / 180;
			x = rad * cos(angle);
			y = rad * sin(angle);
			glVertex2f(x, y);
		}
		glEnd();

		glColor3f(0.0f, 1.0f, 0.0f);

		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(-0.2, 0.1, 0.0);
		glVertex3f(0.2, 0.1, 0.0);
		glVertex3f(0, 0.3, 0);
		glEnd();
	}
};

class Pacman :public Entity {
public:
	float lastDirection = 0;
	int fruitAte = 0;

	Pacman(vector<Entity*>* es) {
		printf("created pacman\n");
		radius = 0.5;
		allEntities = es;
		type = EntityTypes::player;
		isAlive = true;
	}

	virtual vector<Entity*> Touched(){
		//printf("call overrided touched from pacman\n");
		vector<Entity*> v = Entity::Touched();

		for (auto x : v) {
			if (x->type == EntityTypes::fruit) {
				fruitAte++;
				x->Kill();
				radius += 0.1;
			}
			else if (x->type == EntityTypes::enemy) {
				isAlive = false;
				GameOver();
				break;
			}
		}

		return v;
	}

	virtual void Inputed() {
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

		if (nx == 0 && ny == 0) {
			continue;
		}

		x = nx;
		y = ny;

		break;
	}

	auto nfruit = new Fruit(&entities);
	nfruit->x = x;
	nfruit->y = y;

	entities.push_back(nfruit);
}

void SpawnEnemyAtRandom() {
	float x = 0;
	float y = 0;
	while (true) {
		float nx = rand() % (int)xMax;
		float ny = rand() % (int)yMax;

		if (nx == 0 && ny == 0) {
			continue;
		}

		x = nx;
		y = ny;

		break;
	}

	auto nfruit = new Enemy(&entities);
	nfruit->x = x;
	nfruit->y = y;

	entities.push_back(nfruit);
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



	MakeMaps();

	Pacman player(&entities);
	//Fruit testFruit;
	//testFruit.x = xMax / 2;
	//testFruit.y = yMax / 2;
	player.x = xMax / 2;
	player.y = yMax / 2;
	//entities.push_back(&testFruit);
	entities.push_back(&player);
	currentPlayer = &player;

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

	for (auto iter = entities.begin(); iter != entities.end(); ) {
		if (!(*iter)->isAlive) {
			iter = entities.erase(iter);
		}
		else {
			iter++;
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
