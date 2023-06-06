#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void renderText(const std::string& text, const SDL_Rect& rect, const TTF_Font& font, const SDL_Renderer& pRenderer);
