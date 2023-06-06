#include "../include/Utils.hpp"

void renderText(const std::string& text, const SDL_Rect& rect, TTF_Font* font, SDL_Renderer* pRenderer) {
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(pRenderer, surface);
    SDL_FreeSurface(surface);

    SDL_RenderCopy(pRenderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
}
