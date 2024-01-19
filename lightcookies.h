#ifndef cookies_h
#define cookies_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
using namespace std;

SDL_Texture *addTextures(SDL_Renderer *renderer, vector<vector<int>> fogcookies, SDL_Texture *&illuminateMe, SDL_Texture *&lightspot, int widthOfIlluminateMe, int heightOfIlluminateMe, int paddingx, int paddingy, int layer);

SDL_Texture *IlluminateTexture(SDL_Renderer *renderer, SDL_Texture *&mask, SDL_Texture *&diffuse, SDL_Texture *&result);

Uint32 getPixelOfSurface(SDL_Surface *surface, int x, int y);

SDL_Texture* animateWater(SDL_Renderer* renderer, SDL_Texture* wtex, SDL_Surface* wsurf, float acc);

#endif
