#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include "include/GL/freeglut.h"

using namespace std;

/// <summary>
/// 게임 엔티티의 타입 목록 : 없음, 플레이어, 적, 과일
/// </summary>
enum EntityTypes { none, player, enemy, fruit };


/// 전역 변수들은 여기 있으면 객체지향 어긋나지만 일단 작업

void GameOver();
void GameFinish();

int lastPressed = 0;
float timeLeft = 30;
int Width = 600, Height = 600;

float xMax = 600, yMax = 600;

float enemyRadius = 10;
float playerRadius = 12;
float playerRadiusIncrese = 4;
float fruitRadius = 10;

float enemySpeed = 0.3;
float enemySpeedIncrease = 1.5;
float playerSpeed = (xMax + yMax) / 100;

int enemySpawnInterval = 5000;
int fruitSpawnInterval = 6000;

/// <summary>
/// 모든 게임 엔티티의 원형
/// </summary>
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

/// <summary>
/// 과일 엔티티
/// </summary>
class Fruit : public Entity {
public:

	Fruit(vector<Entity*>* es) {
		//printf("created Fruit\n");
		type = EntityTypes::fruit;
		radius = fruitRadius;
		allEntities = es;
		isAlive = true;
	}

	virtual void Draw() {
		glTranslatef(x, y, 0.0);
		glLineWidth(radius);

		glColor3f(0, 0, 0);

		glBegin(GL_LINE_STRIP);
		glVertex3f(0, radius*1.33, 0.0);
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

/// <summary>
/// 적 엔티티
/// </summary>
class Enemy : public Entity {
public:
	float lastDirection = 0;
	int fruitAte = 0;
	Entity* target=NULL;

	Enemy(vector<Entity*>* es) {
		//printf("created enemy\n");
		allEntities = es;
		
		type = EntityTypes::enemy;
		radius = enemyRadius;
		speed = enemySpeed;
		isAlive = true;
	}

	virtual vector<Entity*> Touched() {
		vector<Entity*> v=Entity::Touched();

		for (auto x : v) {
			if (x->type == EntityTypes::fruit) {
				x->Kill();
				fruitAte++;
				speed *= enemySpeedIncrease;
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
			//printf("enemy angle : % f\n", lastDirection);
		}
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
		glLineWidth(radius*10);

	


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
		glVertex3f(-(radius*0.66), radius/3, 0.0);
		glVertex3f(radius*0.66, radius/3, 0.0);
		glVertex3f(0, radius, 0);
		glEnd();
	}
};

/// <summary>
/// 플레이어 엔티티
/// </summary>
class Pacman :public Entity {
public:
	float lastDirection = 0;
	int fruitAte = 0;
	int calledCnt = 0;
	bool isMouthOpened = true;

	Pacman(vector<Entity*>* es) {
		//printf("created pacman\n");
		radius = playerRadius;
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
				radius += playerRadiusIncrese;
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
				speed = playerSpeed ;
			}

			lastDirection = currentDirection;

			lastPressed = 0;
		}

		
	}

	virtual void Move() {
		float xOffset = 0;
		float yOffset = 0;


		if (lastDirection == 90) {
			yOffset = speed;
		}
		else if (lastDirection == 180) {
			xOffset = -speed;
		}
		else if (lastDirection == 270) {
			yOffset = -speed;
		}
		else if (lastDirection == 0) {
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

		if (calledCnt < 60) {
			calledCnt++;
		}
		else {
			calledCnt = 0;
			isMouthOpened = !isMouthOpened;
		}
		glTranslatef(x, y, 0.0);
		glRotatef(lastDirection, 0, 0, 1.0);
		glLineWidth(radius*5);
	

		glColor3f(1.0f, 0.7f, 0);

		double rad = radius;
		GLfloat twicePi = 2.0f * 3.14;
		int triangleAmount = 360;
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(0, 0); // center of circle
		for (int i = 0; i <= triangleAmount; i++) {

			float min = isMouthOpened ? 10 : 30;
			float max = isMouthOpened ? 350 : 300;
			if (i > min && i < max) {
				glVertex2f(
					(radius * cos(i * twicePi / triangleAmount)),
					(radius * sin(i * twicePi / triangleAmount))
				);
			}
			/*if (isMouthOpened) {
				if (i > 30 && i < 300) {
					glVertex2f(
						(radius * cos(i * twicePi / triangleAmount)),
						(radius * sin(i * twicePi / triangleAmount))
					);
				}
			}
			else {
				glVertex2f(
					(radius * cos(i * twicePi / triangleAmount)),
					(radius * sin(i * twicePi / triangleAmount))
				);
			}*/

			
		
		}
		glEnd();
		
	}

};



Pacman* currentPlayer;
vector<Entity*> entities;


void GameOver() {
	int ate = currentPlayer->fruitAte;
	printf("You Lose...\nCollected Fruit: %d\nTime Left: %f\n", ate,timeLeft);
	glutLeaveMainLoop();
}

void GameFinish() {
	int ate = currentPlayer->fruitAte;
	printf("You Win!!\nCollected Fruit: %d\n", ate);
	glutLeaveMainLoop();
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




void Render();
void Reshape(int w, int h);
void Timer(int id);

float angle1 = 0.0;
float angle2 = 0.0;

float xOffset = 0;
float yOffset = 0;

void SpawnFruits(int id) {
	SpawnFruitAtRandom();
	glutTimerFunc(fruitSpawnInterval, SpawnFruits, 2);
}

void SpawnEnemies(int id) {
	SpawnEnemyAtRandom();
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


int main(int argc, char** argv)
{
	srand(time(NULL));

	glutInit(&argc, argv);

	glutInitWindowSize(Width, Height);
	glutInitWindowPosition(0, 0);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	glutCreateWindow("PacMan");

	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
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

	player.x = xMax / 2;
	player.y = yMax / 2;
	entities.push_back(&player);
	currentPlayer = &player;

	glutMainLoop();
	return 0;
}

void Timer(int id)
{
	if (timeLeft > 0) {
		timeLeft -= 0.01;
		glutPostRedisplay();
		glutTimerFunc(10, Timer, 2);
	}
	else {
		GameFinish();
		glFinish();
	}


}
void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	Width = w;
	Height = h;
}

void Render()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, xMax, 0, yMax);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	for (auto x : entities) {
		if (x->isAlive) {
			x->Touched();
			x->Inputed();
			x->Move();
			glPushMatrix();
			{
				x->Draw();
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

	glutSwapBuffers();
}

