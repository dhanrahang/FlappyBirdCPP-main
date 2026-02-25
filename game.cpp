#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
using namespace std;

#define WIDTH 800
#define HEIGHT 600
#define FD 50
#define GC 100
#define MARGIN 220
struct StartMenu
{
    SDL_FRect logoSrc;
    SDL_FRect logoDst;

    SDL_FRect playSrc;
    SDL_FRect playDst;

    SDL_FRect textSrc;
    SDL_FRect textDst;

    bool active;
    StartMenu() : logoSrc({150, 195, 92, 30}),
                  logoDst({260, 250, 276, 90}),
                  playSrc({264, 135, 321 - 264, 168 - 135}),
                  playDst({320, 450, 57 * 3, 33 * 2}),
                  textSrc({16,28,72,44}),
                  textDst({380,390,72 , 44}),
                  active(true) {};
};

struct Score {
    int currSCore ;
    int HighSCore ;

};

struct PipeUp
{

    SDL_FRect srcUP;
    SDL_FRect dstUP;
    SDL_FRect dstDN ;
    SDL_Texture * tex ;
    SDL_Renderer * ren ;
    
    float * x ;
    float * y;
    Uint64 *  timer ;

   
    PipeUp() : srcUP({151 + 1, 0 + 3, 28-1-1, 164})  {};
};

bool isActionInput(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        return event.key.key == SDLK_UP || event.key.key == SDLK_SPACE;
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        return event.button.button == SDL_BUTTON_LEFT;
    }
    if (event.type == SDL_EVENT_FINGER_DOWN)
    {
        return true;
    }
    return false;
}

void birdAimation(SDL_Renderer *ren, SDL_Texture *tex, SDL_FRect dst, Uint64 &timer, int &frame, int seq[])
{
    int sprite = seq[frame];
    SDL_FRect src = {381.0f, static_cast<float>(187 + sprite * 26), 17.0f, 12.0f};
    SDL_RenderTexture(ren, tex, &src, &dst);
    if (SDL_GetTicks() - timer >= FD)
    {
        timer = SDL_GetTicks();
        frame++;
        if (frame > 3)
            frame = 0;
    };
}

void drawPipe(PipeUp* pipe, bool &gameOver, int pipeNum, const SDL_FRect* birdst = nullptr) {
    float y = pipe->y[pipeNum];
    float x = pipe->x[pipeNum] ; 
    
    // 1. Setup Pipe Destinations
    pipe->dstUP = {x, y, 26 * 2, 164 * 2};
    pipe->dstDN = {x, y + (164 * 2) + 140, 26 * 2, 164 * 2};

    // 2. Draw the Pipes
    // Upper Pipe
    SDL_RenderTexture(pipe->ren, pipe->tex, &pipe->srcUP, &pipe->dstUP);
    // Lower Pipe (Rotated 180 degrees)
    SDL_RenderTextureRotated(pipe->ren, pipe->tex, &pipe->srcUP, &pipe->dstDN, 180.0, NULL, SDL_FLIP_NONE);

    // 3. Movement Logic (Delta Time is better, but keeping your logic for consistency)
    Uint32 timenow = SDL_GetTicks();
    if (timenow - pipe->timer[pipeNum] >= 20 && !gameOver) { 
        pipe->x[pipeNum] -= 5; 
        pipe->timer[pipeNum] = timenow; 
    }

    
    if (birdst && !gameOver) {
        // Create a shrunken hitbox for "forgiveness"
        // We shrink the bird's collision box by 15% on each side
        SDL_FRect birdHitbox;
        float horizontalPadding = birdst->w * 0.15f;
        float verticalPadding = birdst->h * 0.15f;

        birdHitbox.x = birdst->x + horizontalPadding;
        birdHitbox.y = birdst->y + verticalPadding;
        birdHitbox.w = birdst->w - (horizontalPadding * 2);
        birdHitbox.h = birdst->h - (verticalPadding * 2);

       
        if (SDL_HasRectIntersectionFloat(&birdHitbox, &pipe->dstUP) || 
            SDL_HasRectIntersectionFloat(&birdHitbox, &pipe->dstDN)) {
            
            gameOver = true;
            
        }
    }

    
    if (x + (26 * 2) < 0) {

        pipe->x[pipeNum] = WIDTH;
        // Keep pipe opening in a playable vertical range.
        pipe->y[pipeNum] = -220.0f + static_cast<float>(rand() % 140);
    }
}


void GameOver(SDL_Renderer * ren , SDL_Texture * tex  ) {
    SDL_FRect src = {152,170 , 96 , 26 } ;
    SDL_FRect dst = {250,150 , 96*3 , 26*3} ;

    SDL_FRect srcBoard ={433-173 ,260-65,113 ,57 };
    SDL_FRect dstBoard ={230,250,113*3,57*3 };

    SDL_RenderTexture(ren , tex , &src , &dst) ;
    SDL_RenderTexture(ren , tex , &srcBoard , &dstBoard) ;

};




int main()
{


    srand((unsigned int)time(NULL));
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        cout << "SDL_Init error: " << SDL_GetError() << endl;
        return 1;
    }

    // WIndow
    SDL_Window *win = SDL_CreateWindow("FLAPPY BIRD", WIDTH, HEIGHT, 0);
    if (!win)
    {
        cout << "Window create error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    // Renderer
    SDL_Renderer *ren = SDL_CreateRenderer(win, NULL);
    if (!ren)
    {
        cout << "Renderer create error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // sprite load kr re
    const char *path = SDL_GetBasePath();
    std::string fullPath = std::string(path) + "img/flappy.png";
    
    SDL_Surface *sprite = IMG_Load(fullPath.c_str());
    if (!sprite)
    {
        cout << "IMG LOAD ERROR (required file): " << fullPath << " -> " << SDL_GetError() << endl;
        if (sprite) SDL_DestroySurface(sprite);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    std::string textPath = std::string(path) + "img/text.png" ;
    SDL_Surface *text = IMG_Load(textPath.c_str());

    SDL_Texture *spriteTex = SDL_CreateTextureFromSurface(ren, sprite);
    SDL_Texture *txt = nullptr;
    bool ownsTxtTexture = false;
    if (text)
    {
        txt = SDL_CreateTextureFromSurface(ren, text);
        SDL_DestroySurface(text);
        ownsTxtTexture = true;
    }
    else
    {
        cout << "Warning: optional file missing: " << textPath << ". Using flappy texture for menu text." << endl;
        txt = spriteTex;
    }
    SDL_DestroySurface(sprite);
    if (!spriteTex || !txt)
    {
        cout << "Texture LOAD ERROR : " << SDL_GetError() << endl;
        if (txt && ownsTxtTexture) SDL_DestroyTexture(txt);
        if (spriteTex) SDL_DestroyTexture(spriteTex);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    SDL_SetTextureScaleMode(spriteTex, SDL_SCALEMODE_NEAREST);
    SDL_FRect backgroundSrc = {10, 0, 128, 256};
    SDL_FRect backgroundDst1 = {0, 0, WIDTH / 2, HEIGHT};
    SDL_FRect backgroundDst2 = {WIDTH / 2, 0, WIDTH / 2, HEIGHT};

    // StartUp Menu
    

    StartMenu *sm = new StartMenu();

    SDL_AudioSpec spec{}, specFly{};
    Uint8 *buffer = nullptr, *bufferFly = nullptr;
    Uint32 len = 0, lenFly = 0;
    std::string mpath = std::string(path) + "audio/start.wav";
    if (!SDL_LoadWAV(mpath.c_str(), &spec, &buffer, &len))
    {
        std::cout << SDL_GetError();
    };

    std::string fpath = std::string(path) + "audio/whoosh.wav";
    if (!SDL_LoadWAV(fpath.c_str(), &specFly, &bufferFly, &lenFly))
    {
        std::cout << SDL_GetError();
    };

    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!stream)
    {
        std::cout << SDL_GetError();
    }
    SDL_AudioStream *streamfly = nullptr;
    if (bufferFly)
    {
        streamfly = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &specFly, NULL, NULL);
        if (!streamfly)
        {
            cout << "Audio Stream Error (fly): " << SDL_GetError() << endl;
        }
        else
        {
            SDL_ResumeAudioStreamDevice(streamfly);
        }
    }

    if (streamfly)
    {
        SDL_ResumeAudioStreamDevice(streamfly);
    }
    if (stream && buffer && len > 0)
    {
        SDL_ResumeAudioStreamDevice(stream);
        SDL_PutAudioStreamData(stream, buffer, len);
    }

    // PIPES
    float y[4] = {-200  , -180 , -160 , -190};
    float x[4]={
        WIDTH-26*2
        ,WIDTH-26*2+MARGIN,
        WIDTH-26*2+MARGIN*2,
        WIDTH-26*2+MARGIN*3
    };
    Uint64 timer[4] = {SDL_GetTicks(),SDL_GetTicks(),SDL_GetTicks(),SDL_GetTicks()};
    PipeUp *pipe = new PipeUp();
    pipe->ren = ren ;
    pipe->tex = spriteTex ;
    pipe-> y = y ;
    pipe->x = x ;
    pipe->timer = timer;

   // Ground 

   SDL_FRect gsrc = {215 , 10 , 168 , 56} ;
   SDL_FRect gdst ;
   gdst.h = 56*2 ;
   gdst.w = 168*5;
   gdst.x = 0 ;
   gdst.y = HEIGHT-56*2 ;

    // BIRD

    SDL_FRect dst;
    dst.x = 200.0;
    dst.y = 250.0;
    dst.h = 12 * 4;
    dst.w = 17 * 4;

    int sequence[4] = {0, 1, 2, 1};

    // Velocity and gravity Gravity
    // PHYSICS CONSTANTS
    const float GRAVITY = 1000.0f;        // pixels per second squared
    const float JUMP_VELOCITY = -200.0f;  // pixels per second (negative = upward)
    const float MAX_FALL_SPEED = 600.0f;  // terminal velocity
    
    // Physics state
    float velocityY = 0.0f;  // Current vertical velocity in pixels/second
    Uint64 lastPhysicsTime = SDL_GetTicks();
    
    // Timers // BOOLS
    Uint64 birdtimer = SDL_GetTicks();
    int frame = 0;
    bool running = true;
    bool gameOver = false ;
    auto resetRound = [&]() {
        dst.x = 200.0f;
        dst.y = 250.0f;
        velocityY = 0.0f;
        gameOver = false;
        lastPhysicsTime = SDL_GetTicks();
        for (int i = 0; i < 4; ++i)
        {
            x[i] = WIDTH - 26 * 2 + MARGIN * i;
            y[i] = -220.0f + static_cast<float>(rand() % 140);
            timer[i] = SDL_GetTicks();
        }
    };

    while (running)
    {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastPhysicsTime) / 1000.0f;  // Convert to seconds
        lastPhysicsTime = currentTime;
        if (deltaTime > 0.05f) deltaTime = 0.05f;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {

            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }

            if (isActionInput(event))
            {
               if (sm->active)
                {
                    sm->active = false;
                    resetRound();
                }

               if (!gameOver)  // Only jump when game is active
                {
                    // Play jump sound
                    if (streamfly && bufferFly)
                    {
                        SDL_ClearAudioStream(streamfly);
                        SDL_PutAudioStreamData(streamfly, bufferFly, lenFly);
                    }
                    
                    // Apply jump velocity (negative because Y increases downward)
                    velocityY = JUMP_VELOCITY;
                }
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_RETURN)
            {
                sm->active = false;
                resetRound();
                if (stream)
                {
                    SDL_ClearAudioStream(stream);
                }
                std::cout << "ENTER _ CLICKED";
            }

             if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_R)
            {
                sm->active = false;
                resetRound();
            }
        }
        // PHYSICS UPDATE
        if (!sm->active && !gameOver)
        {
            // Apply gravity: v = v + a*t
            velocityY += GRAVITY * deltaTime;
            
            // Clamp to max fall speed
            if (velocityY > MAX_FALL_SPEED)
            {
                velocityY = MAX_FALL_SPEED;
            }
            
            // Update position: p = p + v*t
            dst.y += velocityY * deltaTime;
            
            // Ground collision
            if (dst.y > HEIGHT  - dst.h - 56*2)
            {
                dst.y = HEIGHT - dst.h -56*2;
                velocityY = 0.0f;
                gameOver = true;

            }
            
            // Ceiling collision
            if (dst.y < 0.0f)
            {
                dst.y = 0.0f;
                velocityY = 0.0f;
            }
        }

       
        // Draw Background
        SDL_RenderClear(ren);

        SDL_RenderTexture(ren, spriteTex, &backgroundSrc, &backgroundDst1);
        SDL_RenderTexture(ren, spriteTex, &backgroundSrc, &backgroundDst2);
       
        
        if(!sm->active){
        drawPipe(pipe , gameOver, 0 , &dst);
        drawPipe(pipe , gameOver, 1 , &dst);
        drawPipe(pipe , gameOver, 2 , &dst);
        drawPipe(pipe , gameOver, 3, &dst);
        }

        SDL_RenderTexture(ren, spriteTex, &gsrc, &gdst);

         if (sm->active)
        {
            SDL_RenderTexture(ren, spriteTex, &sm->logoSrc, &sm->logoDst);
            SDL_RenderTexture(ren, spriteTex, &sm->playSrc, &sm->playDst);

            SDL_RenderTexture(ren , txt  , &sm->textSrc , &sm->textDst );
        }

        birdAimation(ren, spriteTex, dst, birdtimer, frame, sequence);
        
        if(gameOver){
            GameOver(ren , spriteTex) ;
        }

        SDL_RenderPresent(ren);
    }

    delete pipe;
    delete sm;
    if (streamfly)
    {
        SDL_DestroyAudioStream(streamfly);
    }
    if (stream)
    {
        SDL_DestroyAudioStream(stream);
    }
    if (bufferFly)
    {
        SDL_free(bufferFly);
    }
    if (buffer)
    {
        SDL_free(buffer);
    }
    if (txt && ownsTxtTexture)
    {
        SDL_DestroyTexture(txt);
    }
    
    SDL_DestroyTexture(spriteTex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
