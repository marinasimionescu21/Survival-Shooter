#pragma once

#include "components/simple_scene.h"


#define WHITE	glm::vec3(1.f, 1.f, 1.f)
#define GREY	glm::vec3(0.3f, 0.3f, 0.3f)
#define RED		glm::vec3(1.f, 0.f, 0.f)
#define GREEN	glm::vec3(0.f, 1.f, 0.f)
#define YELLOW	glm::vec3(1.f, 1.f, 0.f)
#define BROWN	glm::vec3(0.7f, 0.6f, 0.4f)

#define NO_TRIANGLES 100
#define SCORE 10
#define ENEMY_SIZE 30

#define PLAYER_RADIUS 30

#define PLAYER_SPEED 150

class Bullet {
    public:
        float bulletAngle;
        float bulletTipX;
        float bulletTipY;
        float initialBulletTipX;
        float initialBulletTipY;
        float lastTranslateX;
        float lastTranslateY;
        float speed;

        Bullet(float angle, float lastTranslateX, float lastTranslateY, float tipX, float tipY) {
            this->lastTranslateX = lastTranslateX;
            this->lastTranslateY = lastTranslateY;
            this->bulletAngle = angle;
            this->bulletTipX = tipX;
            this->bulletTipY = tipY;
            lastTranslateX = 0;
            lastTranslateY = 0;
            initialBulletTipX = 0;
            initialBulletTipY = 0;
            speed = 1;
        }

        ~Bullet() {}

};

class Enemy {
    public:
        float enemyTranslateX;
        float enemyTranslateY;
        float enemyStartX;
        float enemyStartY;
        float enemyAngle;
        bool defeat;
        float score;
        float enemyScale;

        Enemy(float startX, float startY) {
            this->enemyStartX = startX;
            this->enemyStartY = startY;
            enemyTranslateX = 0;
            enemyTranslateY = 0;
            enemyAngle = 0;
            enemyScale = 1;
            defeat = false;
            score = SCORE;
        }

        ~Enemy() {}

};

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:

         struct LogicSpace
         {
             LogicSpace() : x(0), y(0), width(1), height(1) {}
             LogicSpace(float x, float y, float width, float height)
                 : x(x), y(y), width(width), height(height) {}
             float x;
             float y;
             float width;
             float height;
         };
     public:

        Tema1();
        ~Tema1();

        void Init() override;
        void drawBullet(float deltaTimeSeconds);
        void drawEnemy(float deltaTimeSeconds);
        void generateEnemy();
        float rotations;
        bool direction;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void ComputeBulletAngle();

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void shoot();
        void checkBulletTargetCollisions();
        void checkEnemyPlayerCollisions();
        void printScore();
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        bool CheckBallToPointColision(float PointX, float PointY, float CenterX, float CenterY);
        bool CheckBallToBallColision(float PointX, float PointY, float CenterX, float CenterY);
        void RenderEnemy(float deltaTimeSeconds);
        int Random(bool forOY);

     protected:
        glm::mat3 modelMatrix;
        float cx, cy;
        float translatePlayerX, translatePlayerY;
        float scalePlayerX, scalePlayerY;
        float angularStep;
        float degreePlayer;
        glm::vec3 center;
        GLenum polygonMode;
        bool gameOver = false;
        float length;

        glm::vec2 playerCoord;

        Mesh* player;
        Mesh* enemy;
        Mesh* wall1;

        bool start;
        bool finish;

        float speed;

        int lives;
        float mousePosX, mousePosY;
        LogicSpace logicSpace;
        float rotatie;

        float translateBulletX = 0;
        float translateBulletY = 0;
        float bulletAngle = 0;

        float coordBulletX = 0;
        float coordBulletY = 0;

        float distCollision = (float)0.25;

        float score = 0;

        float colisionShuriken;


        std::vector<Enemy> enemys;
        std::vector<Bullet> bullets;

        float coordWall1, coordWall2, coordWall3, coordWall4;

        // TODO(student): If you need any other class variables, define them here.

    };
}   // namespace m1
