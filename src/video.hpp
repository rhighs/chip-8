#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

class Video {
    public:
        Video(std::string title, int winw, int winh, int texw, int texh);
        ~Video();
        void update(void const* buffer, int pitch);
        bool process_input(uint8_t* keys);
    private:
        SDL_Window *w;
        SDL_Renderer *rdr;
        SDL_Texture *tex;
};
