#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */



Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    cout << "=====================================" << endl;
    cout << "Game Started!" << endl << "You have 3 lives" << endl;
    cout << "Have fun" << endl;
    cout << "=====================================" << endl << endl;

    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));

    camera->Update();
    GetCameraInput()->SetActive(false);


    logicSpace.x = 0;       
    logicSpace.y = 0;       
    logicSpace.width = 4;   
    logicSpace.height = 4;  

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;


    speed = 300.0f;

    // Initialize tx and ty (the translation steps)
    translatePlayerX = 80;
    translatePlayerY = 360;

    coordBulletX = 130;
    coordBulletY = 200;

    // Initialize sx and sy (the scale factors)
    scalePlayerX = 1;
    scalePlayerY = 1;

    lives = 3;

    coordWall1 = -30;
    coordWall2 = 1175;
    coordWall3 = 386;
    coordWall4 = -252;

    colisionShuriken = false;


    // Initialize angularStep
    angularStep = 0;

    start = false;
    finish = false;

    //creez player-ul, inamicii, proiectilele
    {
        player = object2D::CreatePlayer("player", glm::vec3(80, 360, 0));
        player->setMeshCenter(glm::vec3(80, 360, 0));
        AddMeshToList(player);

        enemy = object2D::CreateEnemy("enemy", glm::vec3(80, 150, 0), ENEMY_SIZE, RED);
        AddMeshToList(enemy);

        Mesh* bullet = object2D::CreateRectangle("bullet", glm::vec3(80, 360, 0), 10, 11, WHITE);
        AddMeshToList(bullet);
    }
     
    //creez obstacolele
    {
        Mesh* obst1 = object2D::CreateSquare("obst1", glm::vec3(300, 622, 0), 75, GREEN, true);
        AddMeshToList(obst1);

        Mesh* obst2 = object2D::CreateRectangle("obst2", glm::vec3(800, 200, 0), 200, 50, GREEN);
        AddMeshToList(obst2);

        Mesh* obst3 = object2D::CreateRectangle("obst3", glm::vec3(925, 275, 0), 50, 300, GREEN);
        AddMeshToList(obst3);

        Mesh* obst4 = object2D::CreateRectangle("obst4", glm::vec3(111, 300, 0), 50, 175, GREEN);
        AddMeshToList(obst4);

        Mesh* obst6 = object2D::CreateRectangle("obst6", glm::vec3(875, 672, 0), 50, 300, GREEN);
        AddMeshToList(obst6);

        Mesh* obst7 = object2D::CreateRectangle("obst7", glm::vec3(400, 134, 0), 225, 200, GREEN);
        AddMeshToList(obst7);
        
        Mesh* obst8 = object2D::CreateRectangle("obst8", glm::vec3(500, 450, 0), 130, 140, GREEN);
        AddMeshToList(obst8);

        Mesh* gun = object2D::CreateCloud("gun", glm::vec3(80, 375, 0), 25, PINK);
        AddMeshToList(gun);
      
    }

    //creez peretii
    {
        Mesh* wall1 = object2D::CreateRectangle("wall1", glm::vec3(640, 700, 0), 5, 1240, GREY);
        wall1->setMeshCenter(glm::vec3(640, 700, 0));
        AddMeshToList(wall1);

        Mesh* wall2 = object2D::CreateRectangle("wall2", glm::vec3(1258, 360, 0), 677, 5, GREY);
        AddMeshToList(wall2);

        Mesh* wall3 = object2D::CreateRectangle("wall3", glm::vec3(640, 20, 0), 5, 1240, GREY);
        AddMeshToList(wall3);

        Mesh* wall4 = object2D::CreateRectangle("wall4", glm::vec3(22, 360, 0), 677, 5, GREY);
        AddMeshToList(wall4);
    }
}

// aplic transformari proiectilelor in functie de pozitia playerului
void Tema1::drawBullet(float deltaTimeSeconds) {
    glm::ivec2 resolution = window->GetResolution();

    for (int i = 0; i < bullets.size(); i++) {
        float x = player->getMeshCenter().x;
        float y = player->getMeshCenter().y;
        bullets[i].bulletTipX += bullets[i].initialBulletTipX * deltaTimeSeconds * bullets[i].speed;
        bullets[i].bulletTipY += bullets[i].initialBulletTipY * deltaTimeSeconds * bullets[i].speed;
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(bullets[i].bulletTipX, bullets[i].bulletTipY);
        modelMatrix *= transform2D::Translate(x, y);
        modelMatrix *= transform2D::Rotate(bullets[i].bulletAngle);
        modelMatrix *= transform2D::Translate(-x, -y);
        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
        //daca proiectilul iese din scena, este sters
        if (bullets[i].bulletTipX >= resolution.x || bullets[i].bulletTipY >= resolution.y) {
            bullets.erase(bullets.begin() + i);
            i--;
        }
    }

}

//aplic transformari inamicului
void Tema1::drawEnemy(float deltaTimeSeconds) {
    glm::ivec2 resolution = window->GetResolution();
    for (int i = 0; i < enemys.size(); i++) {
        float enemyCenterX = ENEMY_SIZE / 2;
        float enemyCenterY = ENEMY_SIZE / 2;
        enemys[i].enemyAngle -= deltaTimeSeconds * 5;
        enemys[i].enemyTranslateX -= deltaTimeSeconds * 400;
        modelMatrix = glm::mat3(1);

        modelMatrix = transform2D::Translate(enemys[i].enemyStartX, enemys[i].enemyStartY);
        modelMatrix *= transform2D::Translate(enemys[i].enemyTranslateX, enemys[i].enemyTranslateY);
        modelMatrix *= transform2D::Translate(enemyCenterX, enemyCenterY);
        modelMatrix *= transform2D::Translate(-enemyCenterX, -enemyCenterY);
        RenderMesh2D(meshes["enemy"], shaders["VertexColor"], modelMatrix);
        if (enemys[i].enemyStartX + enemys[i].enemyTranslateX + ENEMY_SIZE <= 0 || enemys[i].defeat) {
            enemys.erase(enemys.begin() + i);
            i--;
        }
    }
}

void Tema1::generateEnemy() {
    glm::ivec2 resolution = window->GetResolution();
    int chance = rand() % 1000 + 1;
    if (chance <= 8) {
        float enemyPosX = resolution.x + ENEMY_SIZE;
        float enemyPosY = rand() % (resolution.y - ENEMY_SIZE + 1);
        Enemy e(enemyPosX, enemyPosY);
        enemys.push_back(e);
    }
}

void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

// verific daca exista coliziune intre proiectil si inamic
void Tema1::checkBulletTargetCollisions() {
    for (int i = 0; i < bullets.size(); i++) {
        float alpha = bullets[i].bulletAngle;
        float ax = bullets[i].bulletTipX + cos(alpha);
        float ay = bullets[i].bulletTipY + sin(alpha);

        for (int j = 0; j < enemys.size(); j++) {
            float rs = ENEMY_SIZE;
            float sx = enemys[j].enemyStartX + enemys[j].enemyTranslateX;
            float sy = enemys[j].enemyStartY + enemys[j].enemyTranslateY+ rs;
            float distance = sqrt((bullets[i].bulletTipX - sx) * (bullets[i].bulletTipX - sx) + (bullets[i].bulletTipY - sy) * (bullets[i].bulletTipY - sy));
            if (rs >= distance && !enemys[i].defeat) {
                enemys[j].defeat = true;
                enemys.erase(enemys.begin() + j);
                bullets.erase(bullets.begin() + i);
                score += 10;
                
                printScore();
            }
        }
    }
}

// verific daca exista coliziune intre player si inamic
void Tema1::checkEnemyPlayerCollisions() {

    float bx = 0;
    float by = translatePlayerY + PLAYER_RADIUS;
    float rb = PLAYER_RADIUS;
   

    for (int i = 0; i < enemys.size(); i++) {
        float rs = ENEMY_SIZE / 2;
        float sx = enemys[i].enemyStartX + enemys[i].enemyTranslateX + rs;
        float sy = enemys[i].enemyStartY + rs;

        float distance = sqrt((bx - sx) * (bx - sx) + (bx - sy) * (by - sy));
        if ((rs + rb) * 3 >= distance) {
            lives--;

            enemys.erase(enemys.begin() + i);
            i--;
            printScore();

            if (lives == 0) {
                gameOver = 1;
                cout << "\t\t\t=======================" << endl;
                cout << endl;
                cout << "\t\t\t=      GAME OVER      =" << endl;
                cout << endl;
                cout << "\t\t\t=======================" << endl;
            }
        }/* else if ((enemys[i].enemyStartX + enemys[i].enemyTranslateX) - (player->getMeshCenter().x + translatePlayerX) < 0.5) {
            cout << "x = " << (enemys[i].enemyStartY + enemys[i].enemyTranslateY);
            cout << "y = " << (player->getMeshCenter().y + translatePlayerY);
            lives--;
            enemys.erase(enemys.begin() + i);
            i--;
            printScore();

            if (lives == 0) {
                gameOver = 1;
                cout << "GAME OVER";
            }
        }*/
    }

}

void Tema1::printScore() {
    for (int j = 0; j < lives; j++) {
        cout << "*";
    }
    for (int j = 0; j < 13 - lives; j++) {
        cout << " ";
    }
    cout << score << endl;
}


void Tema1::Update(float deltaTimeSeconds)
{
    float cx = 50.0f;
    float cy = 50.0f;

    modelMatrix = glm::mat3(1);

    //player-ul nu poate trece prin pereti
    if (translatePlayerX < coordWall1) {
        translatePlayerX = coordWall1;
    }


    if (translatePlayerX > coordWall2) {
        translatePlayerX = coordWall2;
    }

    if (translatePlayerY > coordWall3) {
        translatePlayerY = coordWall3;
    }
    
    if (translatePlayerY < coordWall4) {
        translatePlayerY = coordWall4;
    }


    //aplic transformari player-ului
    modelMatrix *= transform2D::Translate(translatePlayerX, translatePlayerY);
    modelMatrix *= transform2D::Scale(0.8f, 0.8f);
    modelMatrix *= transform2D::Translate(80, 360);
    modelMatrix *= transform2D::Rotate(angularStep);
    modelMatrix *= transform2D::Translate(-80, -360);
    RenderMesh2D(meshes["player"], shaders["VertexColor"], modelMatrix);


    //atat timp cat jocul nu s-a incheiat, creeaza inamicii, proiectilele, si verifica daca 
    //exista coliziuni
    if (!gameOver) {
        generateEnemy();
        drawEnemy(deltaTimeSeconds);
        drawBullet(deltaTimeSeconds);
        checkBulletTargetCollisions();
        checkEnemyPlayerCollisions();
    }

    

    // creeaza obstacolele
    modelMatrix = glm::mat3(1);

     {
        
        RenderMesh2D(meshes["obst1"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["obst2"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["obst3"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["obst4"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["obst6"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["obst7"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["obst8"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["gun"], shaders["VertexColor"], modelMatrix);
    }

    {
        
        RenderMesh2D(meshes["wall1"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["wall2"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["wall3"], shaders["VertexColor"], modelMatrix);

        RenderMesh2D(meshes["wall4"], shaders["VertexColor"], modelMatrix);

    }

}

void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

//afla unghiul pentru proiectil
void Tema1::ComputeBulletAngle() {
    glm::ivec2 resolution = window->GetResolution();
    glm::ivec2 cursorPos = window->GetCursorPosition();
    double coordX = cursorPos.x - translatePlayerX - player->getMeshCenter().x;
    double coordY = resolution.y - cursorPos.y - translatePlayerY - player->getMeshCenter().y;
    bulletAngle = atan2(coordY, coordX);
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{

    // deplaseaza player-ul atat timp cat nu se afla in interiorul unui obstacol
    if (window->KeyHold(GLFW_KEY_W)) {
        if ((player->getMeshCenter().x + translatePlayerX < 280 || player->getMeshCenter().x + translatePlayerX > 385 || (player->getMeshCenter().x + translatePlayerX > 280 && player->getMeshCenter().x + translatePlayerX < 385 && player->getMeshCenter().y + translatePlayerY < 650))
            && (player->getMeshCenter().x + translatePlayerX < 738 || player->getMeshCenter().x + translatePlayerX > 1045 || (player->getMeshCenter().x + translatePlayerX > 738 && player->getMeshCenter().x + translatePlayerX < 1045 && player->getMeshCenter().y + translatePlayerY < 676))
            && (player->getMeshCenter().x + translatePlayerX < 423 || player->getMeshCenter().x + translatePlayerX > 580 || (player->getMeshCenter().x + translatePlayerX > 423 && player->getMeshCenter().x + translatePlayerX < 580 && (player->getMeshCenter().y + translatePlayerY < 417 || player->getMeshCenter().y + translatePlayerY > 575)))
            && (player->getMeshCenter().x + translatePlayerX < 30 || player->getMeshCenter().x + translatePlayerX > 202 || (player->getMeshCenter().x + translatePlayerX > 30 && player->getMeshCenter().x + translatePlayerX < 202 && (player->getMeshCenter().y + translatePlayerY < 310 || player->getMeshCenter().y + translatePlayerY > 360)))
            && (player->getMeshCenter().x + translatePlayerX < 777 || player->getMeshCenter().x + translatePlayerX > 832 || (player->getMeshCenter().x + translatePlayerX > 777 && player->getMeshCenter().x + translatePlayerX < 832 && (player->getMeshCenter().y + translatePlayerY < 138 || player->getMeshCenter().y + translatePlayerY > 335)))
            && (player->getMeshCenter().x + translatePlayerX < 832 || player->getMeshCenter().x + translatePlayerX > 1032 || (player->getMeshCenter().x + translatePlayerX > 832 && player->getMeshCenter().x + translatePlayerX < 1032 && (player->getMeshCenter().y + translatePlayerY < 286 || player->getMeshCenter().y + translatePlayerY > 335)))
            && (player->getMeshCenter().x + translatePlayerX < 284 || player->getMeshCenter().x + translatePlayerX > 510 || (player->getMeshCenter().x + translatePlayerX > 284 && player->getMeshCenter().x + translatePlayerX < 510 && (player->getMeshCenter().y + translatePlayerY < 90 || player->getMeshCenter().y + translatePlayerY > 280)))) {
                translatePlayerY += deltaTime * PLAYER_SPEED;
       }
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        if ((player->getMeshCenter().x + translatePlayerX < 300 || player->getMeshCenter().x + translatePlayerX > 405 || (player->getMeshCenter().x + translatePlayerX > 300 && player->getMeshCenter().x + translatePlayerX < 405 && player->getMeshCenter().y + translatePlayerY < 670))
            && (player->getMeshCenter().x + translatePlayerX < 758 || player->getMeshCenter().x + translatePlayerX > 1065 || (player->getMeshCenter().x + translatePlayerX > 758 && player->getMeshCenter().x + translatePlayerX < 1065 && player->getMeshCenter().y + translatePlayerY < 696))
            && (player->getMeshCenter().x + translatePlayerX < 443 || player->getMeshCenter().x + translatePlayerX > 600 || (player->getMeshCenter().x + translatePlayerX > 443 && player->getMeshCenter().x + translatePlayerX < 600 && (player->getMeshCenter().y + translatePlayerY < 437 || player->getMeshCenter().y + translatePlayerY > 595)))
            && (player->getMeshCenter().x + translatePlayerX < 50 || player->getMeshCenter().x + translatePlayerX > 222 || (player->getMeshCenter().x + translatePlayerX > 50 && player->getMeshCenter().x + translatePlayerX < 222 && (player->getMeshCenter().y + translatePlayerY < 330 || player->getMeshCenter().y + translatePlayerY > 380)))
            && (player->getMeshCenter().x + translatePlayerX < 797 || player->getMeshCenter().x + translatePlayerX > 852 || (player->getMeshCenter().x + translatePlayerX > 797 && player->getMeshCenter().x + translatePlayerX < 852 && (player->getMeshCenter().y + translatePlayerY < 158 || player->getMeshCenter().y + translatePlayerY > 355)))
            && (player->getMeshCenter().x + translatePlayerX < 852 || player->getMeshCenter().x + translatePlayerX > 1052 || (player->getMeshCenter().x + translatePlayerX > 852 && player->getMeshCenter().x + translatePlayerX < 1052 && (player->getMeshCenter().y + translatePlayerY < 306 || player->getMeshCenter().y + translatePlayerY > 355)))
            && (player->getMeshCenter().x + translatePlayerX < 304 || player->getMeshCenter().x + translatePlayerX > 530 || (player->getMeshCenter().x + translatePlayerX > 304 && player->getMeshCenter().x + translatePlayerX < 530 && (player->getMeshCenter().y + translatePlayerY < 110 || player->getMeshCenter().y + translatePlayerY > 300)))) {
                translatePlayerX -= deltaTime * PLAYER_SPEED;
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        if ((player->getMeshCenter().x + translatePlayerX < 290 || player->getMeshCenter().x + translatePlayerX > 395 || (player->getMeshCenter().x + translatePlayerX > 290 && player->getMeshCenter().x + translatePlayerX < 395 && player->getMeshCenter().y + translatePlayerY < 690))
            && (player->getMeshCenter().x + translatePlayerX < 788 || player->getMeshCenter().x + translatePlayerX > 1055 || (player->getMeshCenter().x + translatePlayerX > 788 && player->getMeshCenter().x + translatePlayerX < 1055 && player->getMeshCenter().y + translatePlayerY < 686))
            && (player->getMeshCenter().x + translatePlayerX < 433 || player->getMeshCenter().x + translatePlayerX > 590 || (player->getMeshCenter().x + translatePlayerX > 433 && player->getMeshCenter().x + translatePlayerX < 590 && (player->getMeshCenter().y + translatePlayerY < 427 || player->getMeshCenter().y + translatePlayerY > 585)))
            && (player->getMeshCenter().x + translatePlayerX < 60 || player->getMeshCenter().x + translatePlayerX > 232 || (player->getMeshCenter().x + translatePlayerX > 60 && player->getMeshCenter().x + translatePlayerX < 232 && (player->getMeshCenter().y + translatePlayerY < 340 || player->getMeshCenter().y + translatePlayerY > 420)))
            && (player->getMeshCenter().x + translatePlayerX < 787 || player->getMeshCenter().x + translatePlayerX > 842 || (player->getMeshCenter().x + translatePlayerX > 787 && player->getMeshCenter().x + translatePlayerX < 842 && (player->getMeshCenter().y + translatePlayerY < 148 || player->getMeshCenter().y + translatePlayerY > 345)))
            && (player->getMeshCenter().x + translatePlayerX < 842 || player->getMeshCenter().x + translatePlayerX > 1042 || (player->getMeshCenter().x + translatePlayerX > 872 && player->getMeshCenter().x + translatePlayerX < 1042 && (player->getMeshCenter().y + translatePlayerY < 296 || player->getMeshCenter().y + translatePlayerY > 395)))
            && (player->getMeshCenter().x + translatePlayerX < 294 || player->getMeshCenter().x + translatePlayerX > 520 || (player->getMeshCenter().x + translatePlayerX > 294 && player->getMeshCenter().x + translatePlayerX < 520 && (player->getMeshCenter().y + translatePlayerY < 100 || player->getMeshCenter().y + translatePlayerY > 320)))) {
                translatePlayerY -= deltaTime * PLAYER_SPEED;
        }
        
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        if ((player->getMeshCenter().x + translatePlayerX < 290 || player->getMeshCenter().x + translatePlayerX > 395 || (player->getMeshCenter().x + translatePlayerX > 290 && player->getMeshCenter().x + translatePlayerX < 395 && player->getMeshCenter().y + translatePlayerY < 660))
            && (player->getMeshCenter().x + translatePlayerX < 748 || player->getMeshCenter().x + translatePlayerX > 1055 || (player->getMeshCenter().x + translatePlayerX > 748 && player->getMeshCenter().x + translatePlayerX < 1055 && player->getMeshCenter().y + translatePlayerY < 686))
            && (player->getMeshCenter().x + translatePlayerX < 433 || player->getMeshCenter().x + translatePlayerX > 590 || (player->getMeshCenter().x + translatePlayerX > 433 && player->getMeshCenter().x + translatePlayerX < 590 && (player->getMeshCenter().y + translatePlayerY < 427 || player->getMeshCenter().y + translatePlayerY > 585)))
            && (player->getMeshCenter().x + translatePlayerX < 40 || player->getMeshCenter().x + translatePlayerX > 212 || (player->getMeshCenter().x + translatePlayerX > 40 && player->getMeshCenter().x + translatePlayerX < 212 && (player->getMeshCenter().y + translatePlayerY < 320 || player->getMeshCenter().y + translatePlayerY > 370)))
            && (player->getMeshCenter().x + translatePlayerX < 787 || player->getMeshCenter().x + translatePlayerX > 842 || (player->getMeshCenter().x + translatePlayerX > 787 && player->getMeshCenter().x + translatePlayerX < 842 && (player->getMeshCenter().y + translatePlayerY < 148 || player->getMeshCenter().y + translatePlayerY > 345)))
            && (player->getMeshCenter().x + translatePlayerX < 842 || player->getMeshCenter().x + translatePlayerX > 1042 || (player->getMeshCenter().x + translatePlayerX > 842 && player->getMeshCenter().x + translatePlayerX < 1042 && (player->getMeshCenter().y + translatePlayerY < 296 || player->getMeshCenter().y + translatePlayerY > 345)))
            && (player->getMeshCenter().x + translatePlayerX < 294 || player->getMeshCenter().x + translatePlayerX > 520 || (player->getMeshCenter().x + translatePlayerX > 294 && player->getMeshCenter().x + translatePlayerX < 520 && (player->getMeshCenter().y + translatePlayerY < 100 || player->getMeshCenter().y + translatePlayerY > 290)))) {
                translatePlayerX += deltaTime * PLAYER_SPEED;
        }
    }
    ComputeBulletAngle();
    
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


//player-ul urmareste cursorul
void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    glm::ivec2 resolution = window->GetResolution();
    glm::ivec2 cursorPos = window->GetCursorPosition();
    double coordX = cursorPos.x - translatePlayerX - player->getMeshCenter().x;
    double coordY = resolution.y - cursorPos.y - translatePlayerY - player->getMeshCenter().y;
    angularStep = atan2(coordY, coordX);

}

//functie care ajuta la tragerea proiectilelor
void Tema1::shoot() {
    float ty = sin(bulletAngle);
    float tx = cos(bulletAngle);
    Bullet b(bulletAngle, translatePlayerX, translatePlayerY, 0, 0);
    b.initialBulletTipX = tx;
    b.initialBulletTipY = ty;
    b.speed = speed;
    bullets.push_back(b);

}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        shoot();
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
