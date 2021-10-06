/*
2015110758 류영석 컴퓨터 그래픽스 과제 1
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <time.h>
#include<ctime>
#include <algorithm>
#include "include/GL/freeglut.h"

using namespace std;

/// 아래 두 함수는 클래스 내에서도 사용되므로 미리 선언
void GameOver();
void GameFinish();

/// 아래 변수들도 상동. 원래는 전역변수로 쓰던가 생성때 주입하던가 해야함

/// <summary>
/// 마지막으로 눌린 키보드 값. 초기값 0
/// </summary>
int lastPressed = 0;
/// <summary>
/// 실행 시간 체크(남은 시간과 다름)
/// </summary>
clock_t check;

#pragma region Customizable Variables

float timeLeft = 30;
int Width = 600, Height = 600;
float xMax = 600, yMax = 600;

float enemyRadius = 10;
float playerRadius = 12;
float playerRadiusIncrese = 4;
float fruitRadius = 10;

float enemySpeed = 0.5;
float enemySpeedIncrease = 1.5;
float playerSpeed = (xMax + yMax) / 100;

int enemySpawnInterval = 6000;
int fruitSpawnInterval = 3000;

#pragma endregion

#pragma region Class And Enums


/// <summary>
/// 게임 엔티티의 타입 목록 : 없음, 플레이어, 적, 과일
/// </summary>
enum EntityTypes { none, player, enemy, fruit };

/// <summary>
/// 모든 게임 엔티티의 원형
/// </summary>
class Entity {
public:
	float x = 0;
	float y = 0;
	float speed = 0;
	float radius = 0;
	EntityTypes type;
	vector<Entity*>* allEntities;
	bool isAlive = false;

	/// <summary>
	/// 자신과 다른 타입을 가진 엔티티와의 접촉 여부 감지. 접촉한 엔티티를 모두 반환
	/// </summary>
	/// <returns></returns>
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

	virtual void Inputed() {}

	virtual void Move() {}

	/// <summary>
	/// 외부에서 객체 삭제
	/// </summary>
	virtual void Kill() {
		isAlive = false;
	}


	virtual void Draw() {}
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

	/// <summary>
	/// 동그라미 위에 줄기
	/// </summary>
	virtual void Draw() {
		glTranslatef(x, y, 0.0);
		glLineWidth(radius);

		glColor3f(0, 0, 0);

		glBegin(GL_LINE_STRIP);
		glVertex3f(0, radius * 1.33, 0.0);
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
	Entity* target = NULL;

	Enemy(vector<Entity*>* es) {
		//printf("created enemy\n");
		allEntities = es;

		type = EntityTypes::enemy;
		radius = enemyRadius;
		speed = enemySpeed;
		isAlive = true;
	}

	/// <summary>
	/// 접촉 엔티티 목록을 가져와서 과일이면 제거 후 자신 스피드 상승. 플레이어면 처리 종료(게임 오버는 플레이어측 Touched에서 처리)
	/// </summary>
	/// <returns></returns>
	virtual vector<Entity*> Touched() {
		vector<Entity*> v = Entity::Touched();

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

	/// <summary>
	/// 플레이어와의 위치 계산 및 그 방향으로 회전
	/// </summary>
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

	/// <summary>
	/// 플레이어로 가는 일반화된 벡터를 얻은 뒤 스피드에 더해서 Draw때 그려지도록
	/// </summary>
	virtual void Move() {

		float dx = target->x - x;
		float dy = target->y - y;

		float hyp = sqrt((dx * dx) + (dy * dy));

		dx /= hyp;
		dy /= hyp;

		x += dx * speed;
		y += dy * speed;
	}

	/// <summary>
	/// x y로 Translate 및 Rotate 후 원+화살표 모양 Draw. 또한 과일을 먹을 수록 색이 붉게 변함
	/// </summary>
	virtual void Draw() {
		glTranslatef(x, y, 0.0);
		glRotatef(90, 0, 0, 1.0);
		glRotatef(lastDirection, 0, 0, 1.0);
		glLineWidth(radius * 10);




		glColor3f(0.2 * fruitAte, 0, 0.5);

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
		glVertex3f(-(radius * 0.66), radius / 3, 0.0);
		glVertex3f(radius * 0.66, radius / 3, 0.0);
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

	/// <summary>
	/// 접촉 엔티티 중 과일이 있으면 지름++, 적이 있으면 게임 종료
	/// </summary>
	/// <returns></returns>
	virtual vector<Entity*> Touched() {
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

	/// <summary>
	/// 키보드 1회 입력은 방향전환, 2회 입력은 해당 방향 전진. 초기값은 위쪽
	/// </summary>
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
				speed = playerSpeed;
			}

			lastDirection = currentDirection;

			lastPressed = 0;
		}


	}

	/// <summary>
	/// 회전한 방향으로 x y 더하기. 플레이어는 화면 밖에 나갈 수 없음
	/// </summary>
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

	/// <summary>
	/// 60번 불릴때마다 팩맨의 입을 여닫는 애니메이션 구현
	/// </summary>
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
		glLineWidth(radius * 5);


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

		}
		glEnd();

	}

};

#pragma endregion

#pragma region Function and Variables
// 여기부터 main까지는 상단 클래스 등을 필요로 하는 함수/변수들

Pacman* currentPlayer;
vector<Entity*> entities;

/// <summary>
/// 게임 실패
/// </summary>
void GameOver() {
	int ate = currentPlayer->fruitAte;
	auto survived = clock() - check;
	printf("You Lose...\nCollected Fruit: %d\nTime Left: %f\nSurvived Time : %f\n", ate, timeLeft, (float)survived / 1000);
	glutLeaveMainLoop();
}

/// <summary>
/// 게임 완료
/// </summary>
void GameFinish() {
	int ate = currentPlayer->fruitAte;
	printf("You Win!!\nCollected Fruit: %d\n", ate);
	glutLeaveMainLoop();
}

/// <summary>
/// 무작위 위치에 과일 생성
/// </summary>
void SpawnFruitAtRandom() {
	float x = 0;
	float y = 0;
	float nx = 0;
	float ny = 0;
	while (true) {
		nx = rand() % (int)xMax;
		ny = rand() % (int)yMax;

		/// 플레이어와의 겹침 생성 방지 로직
		if (currentPlayer != NULL) {
			auto nEntity = new Entity;
			nEntity->allEntities = currentPlayer->allEntities;
			nEntity->x = nx;
			nEntity->y = ny;

			auto v = nEntity->Touched();

			if (!v.empty()) {
				continue;
			}
		}


		x = nx;
		y = ny;
		break;
	}


	x = nx;
	y = ny;

	auto nfruit = new Fruit(&entities);
	nfruit->x = x;
	nfruit->y = y;

	entities.push_back(nfruit);
}

/// <summary>
/// 무작위 위치에 적 생성
/// </summary>
void SpawnEnemyAtRandom() {

	float x = 0;
	float y = 0;
	float nx = 0;
	float ny = 0;

	while (true) {
		nx = rand() % (int)xMax;
		ny = rand() % (int)yMax;

		/// 플레이어와의 겹침 생성 방지 로직
		if (currentPlayer != NULL) {
			auto nEntity = new Entity;
			nEntity->allEntities = currentPlayer->allEntities;
			nEntity->x = nx;
			nEntity->y = ny;

			auto v = nEntity->Touched();

			if (!v.empty()) {
				continue;
			}
		}
		x = nx;
		y = ny;
		break;
	}


	x = nx;
	y = ny;

	auto nfruit = new Enemy(&entities);
	nfruit->x = x;
	nfruit->y = y;

	entities.push_back(nfruit);
}

/// <summary>
/// 과일 생성 타이머 콜백
/// </summary>
/// <param name="id"></param>
void SpawnFruits(int id) {
	SpawnFruitAtRandom();
	glutTimerFunc(fruitSpawnInterval, SpawnFruits, 2);
}

/// <summary>
/// 적 생성 타이머 콜백
/// </summary>
/// <param name="id"></param>
void SpawnEnemies(int id) {
	SpawnEnemyAtRandom();
	glutTimerFunc(enemySpawnInterval, SpawnEnemies, 2);
}

/// <summary>
/// 첫 적 3마리 화면 가장자리에서 생성
/// </summary>
void SpawnFirstEnemies() {
	auto nfruit1 = new Enemy(&entities);
	auto nfruit2 = new Enemy(&entities);
	auto nfruit3 = new Enemy(&entities);
	nfruit1->x = 0;
	nfruit1->y = yMax;

	nfruit2->x = xMax;
	nfruit2->y = yMax;

	nfruit3->x = xMax;
	nfruit3->y = 0;


	entities.push_back(nfruit1);
	entities.push_back(nfruit2);
	entities.push_back(nfruit3);
}

/// <summary>
/// 최초 맵 생성
/// </summary>
void MakeMaps() {
	for (int i = 0; i < 8; i++) {
		SpawnFruitAtRandom();
	}

	SpawnFirstEnemies();

	glutTimerFunc(fruitSpawnInterval, SpawnFruits, 2);
	glutTimerFunc(enemySpawnInterval, SpawnEnemies, 2);
}

/// <summary>
/// 게임 진행/화면 갱신 타이머 콜백
/// </summary>
/// <param name="id"></param>
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

	// 모든 엔티티 목록을 순회하며, 살아있는 엔티티의 경우에는 개별 행동 처리 및 그리기
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

	// 죽은 엔티티 전체 제거
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

#pragma endregion

/// <summary>
/// 메인 함수
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char** argv)
{
	srand(time(NULL));
	check = clock();

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



