#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <SDL2/SDL.h>

int world[20][20] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
}; // the world, 1 represents a wall

#define PI 3.141592

double xpos, ypos; // the x and y position of the player
double direction; // the directiont that the player is facing

int wallConstant; // a constant that we will be using to draw the walls

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* wallTexture;

double deltaTime;

double FOV = PI/2; // The field of view of the player, in radians

int WINDOW_WIDTH, WINDOW_HEIGHT;

double distance(double x1, double y1, double x2, double y2){
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

void renderClosestWall(double angle, int xImage){
    if(angle > 2*PI) angle-=2*PI;
    if(angle < 0) angle+=2*PI;
    if(angle == PI/2 || angle == 3*PI/2 || angle == 0 || angle == PI) return;
    double intX, currentY; // the vertical intersections
    double currentX, intY; // the horizontal intersections
    int incrementX, incrementY;
    if(angle < PI/2 || angle > 3*PI/2) incrementX = 1;
    else incrementX = -1;
    if(angle < PI) incrementY = 1;
    else incrementY = -1;
    if(incrementX == 1) intX = (int)xpos+incrementX;
    else intX = (int)xpos-0.001;
    if(incrementY == 1) intY = (int)ypos+incrementY;
    else intY = (int)ypos-0.001;
    double tanAngle = tan(angle);
    while(1){
        currentY = tanAngle*(intX-xpos)+ypos; // calculate the vertical intersection
        if(currentY < 0 || currentY >= 20 || intX < 0 || intX >= 20) break; // test if it's out of boundaries
        if(world[(int)intX][(int)currentY]) break; // we find it a vertical intersection!!
        intX+=incrementX;
    }
    while(1){
        currentX = (intY-ypos)/tanAngle+xpos; // calculate the horizontal intersection
        if(currentX < 0 || currentX >= 20 || intY < 0 || intY >= 20) break; // test if it's out of boundaries
        if(world[(int)currentX][(int)intY]) break; // we find it a horizontal intersection!!
        intY+=incrementY;
    }
    // We just need to draw the closest wall
    if(distance(xpos, ypos, intX, currentY) < distance(xpos, ypos, currentX, intY)){
        double wallHeight = wallConstant/(distance(xpos, ypos, intX, currentY)*cos(angle-direction));
        SDL_Rect srcRect = {(currentY-(int)currentY)*32, 0, 1, 32};
        SDL_Rect destRect = {xImage, WINDOW_HEIGHT/2-wallHeight/2, 1, wallHeight};
        SDL_RenderCopy(renderer, wallTexture, &srcRect, &destRect);
    }
    else{
        double wallHeight = wallConstant/(distance(xpos, ypos, currentX, intY)*cos(angle-direction));
        SDL_Rect srcRect = {(currentX-(int)currentX)*32, 0, 1, 32};
        SDL_Rect destRect = {xImage, WINDOW_HEIGHT/2-wallHeight/2, 1, wallHeight};
        SDL_RenderCopy(renderer, wallTexture, &srcRect, &destRect);
    }
}

void renderScene(){
    SDL_SetRenderDrawColor(renderer, 45, 45, 200, 255);
    SDL_RenderClear(renderer);
    int xImage = 0; // its the current column of the image
    if(direction < 0) direction+=2*PI;
    if(direction > 2*PI) direction-=2*PI;
    for(double i = direction-FOV/2; i < direction+FOV/2; i+=FOV/WINDOW_WIDTH){
        renderClosestWall(i, xImage);
        xImage++;
    }
    SDL_RenderPresent(renderer);
}

void init(){
    SDL_Init(SDL_INIT_VIDEO);
    WINDOW_WIDTH = 1024; WINDOW_HEIGHT = 576;
    wallConstant = 1*WINDOW_HEIGHT;
    window = SDL_CreateWindow("raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    xpos = 2; ypos = 2;
    direction = FOV/2;
    wallTexture = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("brick.bmp"));
}

void input(){
    Uint8* currentKeyState = SDL_GetKeyboardState(NULL);
    double VELOCITY = 10;
    // simple input and simple collision detection
    if(currentKeyState[SDL_SCANCODE_W]){
        xpos+=VELOCITY*deltaTime*cos(direction);
        if(world[(int)xpos][(int)ypos]){
            xpos-=VELOCITY*deltaTime*cos(direction);
        }
        ypos+=VELOCITY*deltaTime*sin(direction);
        if(world[(int)xpos][(int)ypos]){
            ypos-=VELOCITY*deltaTime*sin(direction);
        }
    }
    if(currentKeyState[SDL_SCANCODE_S]){
        xpos-=VELOCITY*deltaTime*cos(direction);
        if(world[(int)xpos][(int)ypos]){
            xpos+=VELOCITY*deltaTime*cos(direction);
        }
        ypos-=VELOCITY*deltaTime*sin(direction);
        if(world[(int)xpos][(int)ypos]){
            ypos+=VELOCITY*deltaTime*sin(direction);
        }
    }
    if(currentKeyState[SDL_SCANCODE_A]) direction-=5*deltaTime;
    if(currentKeyState[SDL_SCANCODE_D]) direction+=5*deltaTime;
}

void gameLoop(){
    int quit = 0;
    SDL_Event e;
    while(!quit){
        Uint32 firstTime = SDL_GetTicks();
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT) quit = 1;
        }
        input();
        renderScene();
        deltaTime = (double)(SDL_GetTicks()-firstTime)/1000;
    }
}

int main(int argc, char** argv){
    init();
    gameLoop();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
