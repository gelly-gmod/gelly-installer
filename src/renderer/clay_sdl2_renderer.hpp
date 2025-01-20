#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <clay.h>

namespace gelly::renderer {
struct SDL2_Font {
  uint32_t fontId;
  TTF_Font *font;
};

static constexpr auto MAX_FONTS = 12;

Clay_Dimensions SDL2_MeasureText(Clay_String *text,
                                 Clay_TextElementConfig *config);

void Clay_SDL2_Render(SDL_Renderer *renderer,
                      Clay_RenderCommandArray renderCommands);

bool RegisterFont(const char *fontPath, int fontSize, uint32_t fontId);

SDL_Texture *LoadTexture(SDL_Renderer *renderer, const char *path);

} // namespace gelly::renderer