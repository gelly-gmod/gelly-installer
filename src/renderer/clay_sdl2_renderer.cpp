#include "clay_sdl2_renderer.hpp"

#include <SDL_image.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace gelly::renderer {
namespace {
constexpr auto MAX_SURFACE_TEXT_CACHE = 16384;
std::array<SDL_Texture *, MAX_SURFACE_TEXT_CACHE> textureTextCache = {};

uint32_t HashClayString(const Clay_String &string) {
  return Clay__HashString(string, 0x0, 0xFF3321A).id;
}

SDL_Texture *GetTextureFromCache(const Clay_String &string) {
  const auto hash = HashClayString(string);
  return textureTextCache[hash % MAX_SURFACE_TEXT_CACHE];
}

void WriteTextureToCache(const Clay_String &string, SDL_Texture *surface) {
  const auto hash = HashClayString(string);
  textureTextCache[hash % MAX_SURFACE_TEXT_CACHE] = surface;
}
} // namespace
static std::array<SDL2_Font, MAX_FONTS> SDL2_fonts = {};

Clay_Dimensions SDL2_MeasureText(Clay_String *text,
                                 Clay_TextElementConfig *config) {
  TTF_Font *font = SDL2_fonts[config->fontId].font;
  char *chars = (char *)calloc(text->length + 1, 1);
  memcpy(chars, text->chars, text->length);
  int width = 0;
  int height = 0;
  if (TTF_SizeUTF8(font, chars, &width, &height) < 0) {
    fprintf(stderr, "Error: could not measure text: %s\n", TTF_GetError());
    exit(1);
  }
  free(chars);
  return (Clay_Dimensions){
      .width = static_cast<float>(width),
      .height = static_cast<float>(height),
  };
}

SDL_Rect currentClippingRectangle;

void Clay_SDL2_Render(SDL_Renderer *renderer,
                      Clay_RenderCommandArray renderCommands) {
  for (uint32_t i = 0; i < renderCommands.length; i++) {
    Clay_RenderCommand *renderCommand =
        Clay_RenderCommandArray_Get(&renderCommands, i);
    Clay_BoundingBox boundingBox = renderCommand->boundingBox;
    switch (renderCommand->commandType) {
    case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
      Clay_RectangleElementConfig *config =
          renderCommand->config.rectangleElementConfig;
      Clay_Color color = config->color;
      if (color.a < 255) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
      }

      SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
      SDL_FRect rect = (SDL_FRect){
          .x = boundingBox.x,
          .y = boundingBox.y,
          .w = boundingBox.width,
          .h = boundingBox.height,
      };
      SDL_RenderFillRectF(renderer, &rect);
      SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_BORDER: {
      Clay_BorderElementConfig *config =
          renderCommand->config.borderElementConfig;

      const auto renderBorder = [&](const Clay_Border &border, float x1,
                                    float y1, float x2, float y2) {
        const auto red = static_cast<Uint8>(border.color.r);
        const auto green = static_cast<Uint8>(border.color.g);
        const auto blue = static_cast<Uint8>(border.color.b);
        const auto alpha = static_cast<Uint8>(border.color.a);

        SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
        SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
      };

      if (config->left.width > 0) {
        renderBorder(config->left, boundingBox.x, boundingBox.y, boundingBox.x,
                     boundingBox.y + boundingBox.height);
      }

      if (config->right.width > 0) {
        renderBorder(config->right, boundingBox.x + boundingBox.width,
                     boundingBox.y, boundingBox.x + boundingBox.width,
                     boundingBox.y + boundingBox.height);
      }

      if (config->top.width > 0) {
        renderBorder(config->top, boundingBox.x, boundingBox.y,
                     boundingBox.x + boundingBox.width, boundingBox.y);
      }

      if (config->bottom.width > 0) {
        renderBorder(config->bottom, boundingBox.x,
                     boundingBox.y + boundingBox.height,
                     boundingBox.x + boundingBox.width,
                     boundingBox.y + boundingBox.height);
      }

      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
      Clay_ImageElementConfig *config =
          renderCommand->config.imageElementConfig;
      auto *texture = static_cast<SDL_Texture *>(config->imageData);
      if (!texture) {
        fprintf(stderr, "Error: image texture is null\n");
        exit(1);
      }

      SDL_FRect destination = (SDL_FRect){
          .x = boundingBox.x,
          .y = boundingBox.y,
          .w = boundingBox.width,
          .h = boundingBox.height,
      };

      SDL_RenderCopyF(renderer, texture, nullptr, &destination);
      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_TEXT: {
      Clay_TextElementConfig *config = renderCommand->config.textElementConfig;
      Clay_String text = renderCommand->text;
      char *cloned = (char *)calloc(text.length + 1, 1);
      memcpy(cloned, text.chars, text.length);
      TTF_Font *font = SDL2_fonts[config->fontId].font;
      SDL_Texture *cachedTexture = GetTextureFromCache(text);
      if (!cachedTexture) {
        SDL_Surface *surface =
            TTF_RenderUTF8_Blended(font, cloned,
                                   (SDL_Color){
                                       .r = (Uint8)config->textColor.r,
                                       .g = (Uint8)config->textColor.g,
                                       .b = (Uint8)config->textColor.b,
                                       .a = (Uint8)config->textColor.a,
                                   });
        cachedTexture = SDL_CreateTextureFromSurface(renderer, surface);

        WriteTextureToCache(text, cachedTexture);
        SDL_FreeSurface(surface);
      }

      SDL_FRect destination = (SDL_FRect){
          .x = boundingBox.x,
          .y = boundingBox.y,
          .w = boundingBox.width,
          .h = boundingBox.height,
      };
      SDL_RenderCopyF(renderer, cachedTexture, NULL, &destination);

      free(cloned);
      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
      currentClippingRectangle = (SDL_Rect){
          .x = boundingBox.x,
          .y = boundingBox.y,
          .w = boundingBox.width,
          .h = boundingBox.height,
      };
      SDL_RenderSetClipRect(renderer, &currentClippingRectangle);
      break;
    }
    case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
      SDL_RenderSetClipRect(renderer, NULL);
      break;
    }
    default: {
      fprintf(stderr, "Error: unhandled render command: %d\n",
              renderCommand->commandType);
      exit(1);
    }
    }
  }
}

bool RegisterFont(const char *fontPath, int fontSize, uint32_t fontId) {
  SDL2_Font font = {
      .font = TTF_OpenFont(fontPath, fontSize),
      .fontId = fontId,
  };

  SDL2_fonts[fontId] = font;
  return font.font != nullptr;
}

SDL_Texture *LoadTexture(SDL_Renderer *renderer, const char *imagePath) {
  return SDL_CreateTextureFromSurface(renderer, IMG_Load(imagePath));
}

} // namespace gelly::renderer