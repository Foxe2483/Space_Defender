#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <vector>
#include <stdio.h>
#include <algorithm>

const int WINDOW_W = 640;
const int WINDOW_H = 480;
const double WINDOW_SCALE = 1.2;
int WINDOW_X;
int WINDOW_Y;
int MOUSE_X;
int MOUSE_Y;

bool RenderText(
    SDL_Renderer *renderer, //where to render
    TTF_Font *font, //text font
    std::string stext, //text
    SDL_Color scolor, //text color
    int x, //position x of top-left corner in window
    int y //position y of top-left corner in window
)
{
    if (font == NULL)
    {
        std::cout<<stext<<std::endl;
        return true;
    }
    SDL_Surface *surface;
    SDL_Texture *texture;
    //SDL_Color *color = {scolor.r,scolor.g,scolor.b,scolor.a};
    const char *text = stext.c_str();
    surface = TTF_RenderText_Solid(font, text, scolor);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect *rect;
    rect->x = x;
    rect->y = y;
    rect->w = surface->w;
    rect->h = surface->h;
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    return false;
} //RenderText(renderer,TTF_OpenFont("arial.ttf", 18), "test", {29,255,255,255}, 20, 20);


void RenderImg
(
    SDL_Renderer *renderer,
    std::string spath,
    int x,
    int y,
    int w,
    int h
)
{
    SDL_Texture *texture = NULL;
    const char *path = spath.c_str();
    SDL_Surface *temp = IMG_Load(path);
    texture = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_Rect *rect;
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_FreeSurface(temp);
    SDL_DestroyTexture(texture);
}

bool SceneStart(SDL_Renderer *renderer, bool check)
{
    if(!check) {return check;}
    SDL_Texture *smile = NULL;
    SDL_Surface *temp = IMG_Load("pic.bmp");
    smile = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_RenderCopy(renderer, smile, NULL, NULL);
    SDL_RenderPresent(renderer);
    return check;
}
class Mob
{
public:
    int health = 1;
    double xpstn = 0;
    double ypstn = 0;
    int weight = 0;
    int height = 0;
    std::string texture;
    char brain = 'u';
    double xvec = 0;
    double yvec = 0;
    double speed = 0;
    int cooldown = 50;
    int lastcldw = 50;
    bool shown = false;
    bool fire = false;

    void set(int hp, double x, double y, int w, int h, std::string temptexture,char type, double xvector, double yvector, double spEEd, int shootcooldown)
    {
        health = hp;
        xpstn = x;
        ypstn = y;
        weight = w;
        height = h;
        texture = temptexture;
        brain = type;
        xvec = xvector;
        yvec = yvector;
        speed = spEEd;
        cooldown = shootcooldown;
        shown = true;
    };/*
    void render(SDL_Renderer *renderer)
    {
        RenderImg(renderer, texture, xpstn, ypstn, weight, height);
    }*/
    void renavigate(double xtarget, double ytarget)
    {
        double akatet = xtarget - xpstn;
        double bkatet = ytarget - ypstn;
        double gipot = sqrt((akatet * akatet) + (bkatet * bkatet));
        xvec = akatet * speed / gipot;
        yvec = bkatet * speed / gipot;
    };
    int gunreloaded()
    {
        if((lastcldw <= 0) && fire && (brain != 'b')){
            lastcldw = cooldown;
            return 1;
        }
        lastcldw--;
        return 0;
    };
    void move()
    {
        xpstn += xvec;
        ypstn += yvec;
    };
    void userinput(SDL_Event& e)
    {
        if(e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_w:
                yvec = -0;
                break;
            case SDLK_s:
                yvec = 0;
                break;
            case SDLK_a:
                xvec = -3;
                break;
            case SDLK_d:
                xvec = 3;
                break;
            }
        }
        else if(e.type == SDL_KEYUP)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_w: case SDLK_s:
                yvec = 0;
                break;
            case SDLK_a: case SDLK_d:
                xvec = 0;
                break;
            }
        }
        if(e.type == SDL_MOUSEBUTTONDOWN)
        {
            fire = true;
        }
        else if(e.type == SDL_MOUSEBUTTONUP)
        {
            fire = false;
        }
    };
};




int main(int argc, char **argv)
{
    bool RUNNING = true;
    bool start = true;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event e;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("%s\n", SDL_GetError());
        RUNNING = false;
    }
    TTF_Init();
    window = SDL_CreateWindow("Happy Title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W * WINDOW_SCALE, WINDOW_H * WINDOW_SCALE, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetScale(renderer,WINDOW_SCALE,WINDOW_SCALE);


    Mob player;
    player.set(3, WINDOW_W/2, WINDOW_H/2, 10, 10, "pic.bmp", 'p', 0, 0, 3, 21);
    Mob gag;
    std::vector<Mob> Mobs;
    Mobs.push_back(player);
    double level = 1;
    int score = 0, active_mobs = 0, active_enemy = 0;


    while(RUNNING)
    {
        //---
        SDL_GetWindowPosition(window, &WINDOW_X, &WINDOW_Y);
        SDL_GetGlobalMouseState(&MOUSE_X,&MOUSE_Y);
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
            {
                RUNNING = false;
            }
            if(e.type == SDL_KEYDOWN) { start = false;}
            Mobs[0].userinput(e);
        }
        if(SceneStart(renderer,start)){continue;}
        SDL_RenderClear(renderer);

        Mobs[0].move();
        //if(Mobs[0].gunreloaded())
        //{
        //    std::cout<<std::endl;

        //}
        //for(int i = 0; i != Mobs.size(); i++)
        //{
        //Mobs[0].render(renderer);
        //}

        int *a = Mobs[0].speed;
        std::cout<<a;
        RenderImg(renderer, Mobs[0].texture, Mobs[0].xpstn,Mobs[0].xpstn,Mobs[0].weight,Mobs[0].height);
        RenderText(renderer, TTF_OpenFont("arial.ttf", 18), "Level ", {200,220,255,175}, 1, 1);
        RenderText(renderer, TTF_OpenFont("arial.ttf", 18), "Score ", {200,220,255,175}, 1, 19);
        SDL_Delay(100);
        //---
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
