#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

using namespace sku;

class Video {
    public:
        Video(std::string title, int winw, int winh, int texw, int texh){
            SDL_Init(SDL_INIT_VIDEO);
            w = SDL_CreateWindow(title, 0, 0, winw, winh, SDL_WINDOW_SHOWN);
            rdr = SDL_CreateWindow(w, -1, SDL_RENDERER_ACCELERATED);
            tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, texw, texh);
        }

        ~Video(){
            SDL_DestroyTexture(tex);
            SDL_DestroyRenderer(rdr);
            SDL_DestroyWindow(w);
            SDL_Quit();
        }

        void update(void const* buffer, int pitch){
            SDL_UpdateTexture(texture, nullptr, buffer, pitch);
            SDL_RenderClear(rdr);
            SDL_RenderCopy(rdr, tex, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }

        bool process_input(uint8_t* keys){
            bool quit = false;
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type){
                    case SDL_QUIT: quit = true; break;
                    case SDL_KEYDOWN:
                                   switch (event.key.keysym.sym){
                                       case SDLK_ESCAPE: quit = true; break;
                                       case SDLK_x: keys[0] = 1; break;
                                       case SDLK_1: keys[1] = 1; break;
                                       case SDLK_2: keys[2] = 1; break;
                                       case SDLK_3: keys[3] = 1; break;
                                       case SDLK_q: keys[4] = 1; break;
                                       case SDLK_w: keys[5] = 1; break;
                                       case SDLK_e: keys[6] = 1; break;
                                       case SDLK_a: keys[7] = 1; break;
                                       case SDLK_s: keys[8] = 1; break;
                                       case SDLK_d: keys[9] = 1; break;
                                       case SDLK_z: keys[0xA] = 1; break;
                                       case SDLK_c: keys[0xB] = 1; break;
                                       case SDLK_4: keys[0xC] = 1; break;
                                       case SDLK_r: keys[0xD] = 1; break;
                                       case SDLK_f: keys[0xE] = 1; break;
                                       case SDLK_v: keys[0xF] = 1; break;
                                   } break;
                    case SDL_KEYUP:
                                   switch (event.key.keysym.sym){
                                       case SDLK_x: keys[0] = 0; break;
                                       case SDLK_1: keys[1] = 0; break;
                                       case SDLK_2: keys[2] = 0; break;
                                       case SDLK_3: keys[3] = 0; break;
                                       case SDLK_q: keys[4] = 0; break;
                                       case SDLK_w:keys[5] = 0; break;
                                       case SDLK_e: keys[6] = 0; break;
                                       case SDLK_a: keys[7] = 0; break;
                                       case SDLK_s: keys[8] = 0; break;
                                       case SDLK_d: keys[9] = 0; break; 
                                       case SDLK_z: keys[0xA] = 0; break;
                                       case SDLK_c: keys[0xB] = 0; break;
                                       case SDLK_4: keys[0xC] = 0; break;
                                       case SDLK_r: keys[0xD] = 0; break;
                                       case SDLK_f: keys[0xE] = 0; break;
                                       case SDLK_v: keys[0xF] = 0; break;
                                   } break;
                }
            }
            return quit;
        }

    private:
        SDL_Window* w();
        SDL_Renderer* rdr();
        SDL_Texture* tex();

};