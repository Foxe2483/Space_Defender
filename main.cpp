#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <vector>
#include <stdio.h>
#include <algorithm>

int WINDOW_W = 640;
int WINDOW_H = 480;
//64*48=3 072 /2= 1536
double WINDOW_SCALE = 1.2;
int WINDOW_X;
int WINDOW_Y;
int MOUSE_X;
int MOUSE_Y;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Event e;
int score = 0, active_enemy = 0;

class Ltexture
{
public:
    Ltexture();
    ~Ltexture();
    void load(std::string path);
    void lfree();
    void render(int x, int y, int w, int h);
    void loadtext(std::string text, SDL_Color color, int tsize);
private:
    SDL_Texture* mTexture;
    int tWidth;
    int tHeight;
};

Ltexture::Ltexture()
{
    mTexture = NULL;
    tWidth = 0;
    tHeight = 0;
}
Ltexture::~Ltexture()
{
    lfree();
}
void Ltexture::load(std::string path)
{
    lfree();
    SDL_Texture* newTexture = NULL;
    SDL_Surface* newSurface = IMG_Load(path.c_str());
    SDL_SetColorKey(newSurface,SDL_TRUE, SDL_MapRGB(newSurface->format, 0x03, 0xFF, 0xFF));
    newTexture = SDL_CreateTextureFromSurface(renderer, newSurface);
    tWidth = newSurface->w;
    tHeight = newSurface->h;
    SDL_FreeSurface(newSurface);
    mTexture = newTexture;
}
void Ltexture::loadtext(std::string text, SDL_Color color, int tsize)
{
    lfree();
    SDL_Surface* textSurface = TTF_RenderText_Solid(TTF_OpenFont("arial.ttf", tsize), text.c_str(), color);
    if(textSurface != NULL)
    {
        mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        tWidth = textSurface->w;
        tHeight = textSurface->h;
        SDL_FreeSurface(textSurface);
    }
}
void Ltexture::lfree()
{
    if(mTexture != NULL)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        tWidth = 0;
        tHeight = 0;
    }
}
void Ltexture::render(int x, int y, int w = -1, int h = -1)
{
    if(w == -1 && h == -1)
    {
        w = tWidth;
        h = tHeight;
    }
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderCopy(renderer, mTexture, NULL, &rect);
}
Ltexture omni;
Ltexture textLevel;
Ltexture textScore;
Ltexture Lbullet;
Ltexture Lheart;
Ltexture LTitle;
Ltexture Lenemy;
Ltexture Lplayer;
class Mob
{
public:
    Mob();
    double xposition;
    double yposition;
    int width;
    int height;
    int invulve = 5;
    void setting(int HP, int X, int Y, int W, int H, Ltexture* Image, char Type, double XVector, double YVector, double Speed, int cooldown);
    void renavigate(int xtarget, int ytarget);
    bool moving();
    bool render();
    void gettingdamage(int damage = 1);
    int gethelth();
    char gettype();
    bool firing();
    void kill(int id);
    bool colliding(int id);
    void mfree();
private:
    int health;
    Ltexture* sprite;
    char brain;
    double xvector;
    double yvector;
    double speed;
    int guncooldown;
    int remaincooldown;
    bool shooting;
};

std::vector<Mob> Mobs;

Mob::Mob()
{
    health = 1;
    xposition = 0;
    yposition = 0;
    width = 0;
    height = 0;
    brain ='u';
    xvector = 0;
    yvector = 0;
    speed = 0;
    guncooldown = 0;
    remaincooldown = 0;
    shooting = false;
}
void Mob::setting(int HP, int X, int Y, int W, int H, Ltexture* Image, char Type, double XVector, double YVector, double Speed, int cooldown)
{
    health = HP;
    xposition = X;
    yposition = Y;
    width = W;
    height = H;
    sprite = Image;
    brain = Type;
    xvector = XVector;
    yvector = YVector;
    speed = Speed;
    guncooldown = cooldown;
    shooting = true;
}
void Mob::renavigate(int xtarget, int ytarget)
{
    if(brain != 'p')
    {
        double akatet = xtarget - xposition;
        double bkatet = ytarget - yposition;
        double gipot = sqrt((akatet * akatet) + (bkatet * bkatet));
        xvector = akatet * speed / gipot;
        yvector = bkatet * speed / gipot;
    }
    else
    {
        if(e.type == SDL_KEYDOWN && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_w: case SDLK_UP: yvector -= speed;
                break;
            case SDLK_s: case SDLK_DOWN: yvector += speed;
                break;
            case SDLK_a: case SDLK_LEFT: xvector -= speed;
                break;
            case SDLK_d: case SDLK_RIGHT: xvector += speed;
                break;
            }
        }
        else if(e.type == SDL_KEYUP && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_w: case SDLK_UP: yvector += speed;
                break;
            case SDLK_s: case SDLK_DOWN: yvector -= speed;
                break;
            case SDLK_a: case SDLK_LEFT: xvector += speed;
                break;
            case SDLK_d: case SDLK_RIGHT: xvector -= speed;
                break;
            }
        }
        if(e.type == SDL_MOUSEBUTTONDOWN)
        {
            shooting = true;

        }
        else if(e.type == SDL_MOUSEBUTTONUP)
        {
            shooting = false;
        }
    }
}
bool Mob::moving()
{
    xposition+=xvector;
    yposition+=yvector;
    bool brdr = false;
    if(xposition < 0 || xposition > WINDOW_W-width)
    {
        xposition-=xvector*1.1;
        brdr = true;
    }
    if(yposition < 0 || yposition > WINDOW_H-height)
    {
        yposition-=yvector*1.1;
        brdr = true;
    }
    if(brdr && brain == 'b')
    {
        return true;
    }
    else if(brdr && brain != 'p')
    {
        renavigate(Mobs[0].xposition, Mobs[0].yposition);
    }
    return false;
}
bool Mob::render()
{
    if(sprite == 0) {return false;}
    sprite->render(xposition, yposition, width, height);
    return true;
}
void Mob::gettingdamage(int damage)
{
    if(brain == 'p' && invulve <= 0)
    {
        health-=damage;
        invulve = 60;
    } else if (brain != 'p')
    {
        health-=damage;
    }
}
int Mob::gethelth()
{
    return health;
}
char Mob::gettype()
{
    return brain;
}
bool Mob::firing()
{
    if(invulve > -1) {invulve--;}
    if(sprite != NULL && remaincooldown <= 0 && brain != 'b' && shooting)
    {
        remaincooldown = guncooldown;
        Mob bullet;
        if(brain == 'p')
        {
            SDL_GetWindowPosition(window, &WINDOW_X, &WINDOW_Y);
            SDL_GetGlobalMouseState(&MOUSE_X,&MOUSE_Y);
            MOUSE_X = (MOUSE_X - WINDOW_X)/WINDOW_SCALE;
            MOUSE_Y = (MOUSE_Y - WINDOW_Y)/WINDOW_SCALE;
            double akatet = MOUSE_X - xposition;
            double bkatet = MOUSE_Y - yposition;
            double gipot = sqrt((akatet * akatet) + (bkatet * bkatet));
            bullet.setting(1,
                            xposition + width/2 + akatet*width/gipot,
                            yposition + height/2 + bkatet*height/gipot,
                            3, 3, &Lbullet, 'b', 1, 1, 6, 0);
            bullet.renavigate(MOUSE_X,MOUSE_Y);
        }
        else
        {
             double akatet = Mobs[0].xposition - xposition;
             double bkatet = Mobs[0].yposition - yposition;
             double gipot = sqrt((akatet * akatet) + (bkatet * bkatet));
             bullet.setting(1,
                            xposition + width/2 + akatet*width/gipot,
                            yposition + height/2 + bkatet*height/gipot,
                            3, 3, &Lbullet, 'b', 1, 1, 6, 0);
            bullet.renavigate(Mobs[0].xposition, Mobs[0].yposition);
        }
        Mobs.push_back(bullet);
        return true;
    }
    remaincooldown--;
    if(remaincooldown < -1000) {remaincooldown = -1;}
    return false;
}
void Mob::mfree()
{
    sprite->lfree();
}
void Mob::kill(int id)
{
    Mobs.erase(Mobs.begin() + id);
    if(brain != 'b') {score += 10; active_enemy--;}
}
bool Mob::colliding(int id)
{
    int tempint = Mobs.size();
    for(int i = 0; i < tempint; i++)
    {
        if(
           id != i &&
           brain != Mobs[i].gettype() &&
           xposition > Mobs[i].xposition &&
           xposition < (Mobs[i].xposition + Mobs[i].width) &&
           yposition > Mobs[i].yposition &&
           yposition < (Mobs[i].yposition + Mobs[i].height)
          )
          {
            gettingdamage();
            Mobs[i].gettingdamage();
            return true;
          }
    }
    return false;
}

int main(int argc, char **argv)
{
    bool RUNNING = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("%s\n", SDL_GetError());
        RUNNING = false;
    }
    TTF_Init();
    window = SDL_CreateWindow("Happy Title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W * WINDOW_SCALE, WINDOW_H * WINDOW_SCALE, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetScale(renderer,WINDOW_SCALE,WINDOW_SCALE);
    omni.load("pic.bmp");
    Lbullet.load("bullet.bmp");
    Lplayer.load("player.bmp");
    Lenemy.load("enemy.bmp");
    LTitle.load("Title.bmp");
    Lheart.load("heart.bmp");
    textLevel.loadtext("Level ",{200,220,255,175},18);
    textScore.loadtext("Score ",{200,220,255,175},18);

    newgame:
    Mobs.clear();
    Mobs.reserve(1536);
    bool start = true;
    Mob player;
    player.setting(5, WINDOW_W/2, WINDOW_H/2, 10, 10, &Lplayer, 'p', 0, 0, 3, 30);
    Mobs.push_back(player);
    score = 0;
    active_enemy = 0;
    LTitle.render(0,0,WINDOW_W,WINDOW_H);
    SDL_RenderPresent(renderer);
    int spawntimer = 5;

    while(RUNNING)
    {
        if(Mobs[0].gettype() != 'p')
        {
            goto newgame;
        }
        //---
        SDL_GetWindowSizeInPixels(window, &WINDOW_W, &WINDOW_H);
        WINDOW_W/=WINDOW_SCALE;WINDOW_H/=WINDOW_SCALE;
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
            {
                RUNNING = false;
            }
            if(e.type == SDL_KEYDOWN) { start = false;}
            Mobs[0].renavigate(0,0);
        }
        if(start){SDL_Delay(2); continue;}
        SDL_RenderClear(renderer);

        spawntimer--;
        if(spawntimer == 0)
        {
            Mob enemy;
            enemy.setting(3, rand()%(WINDOW_W-40)+20, 2, 20, 15, &Lenemy, 'e', 0, 1, 1, 160);
            Mobs.push_back(enemy);
            active_enemy++;
            spawntimer = 120 - (score/10);
        }
        int tempint = Mobs.size();
        for(int i = 0; i < tempint; i++)
        {
            Mobs[i].render();
            Mobs[i].colliding(i);
            if(Mobs[i].moving() || Mobs[i].gethelth() <= 0)

            {
                Mobs[i].kill(i);
                continue;
            }
            Mobs[i].firing();
        }
        textLevel.render(1,1);
        textScore.render(1,19);
        for(int i = 0; i != Mobs[0].gethelth(); i++)
        {
            Lheart.render(1+(26*i), WINDOW_H-26, 24, 24);
        }
        SDL_Delay(21);
        //---
        SDL_RenderPresent(renderer);
    }
    Mobs.clear();
    omni.lfree();
    textLevel.lfree();
    textScore.lfree();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
