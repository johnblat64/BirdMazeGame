#include <src/engine/sprite/sprite_sheet.h>
#include <external/stb/stb_image.h>
#include <stdio.h>
#include <src/engine/util/util_error_handling.h>
#include <src/engine/global.h>
#include <math.h>
#include "src/engine/tile/tilemap.h"


SpriteSheet SpriteSheetCreateFromFile(const char *filename, const char *sprite_sheet_name, int rows, int cols)
{
    SpriteSheet sprite_sheet;


    sprite_sheet.n_rows = rows;
    sprite_sheet.n_cols = cols;

    int req_format = STBI_rgb_alpha;
    int image_width, image_height, image_channels;
    unsigned char *tileset_image_data = stbi_load(filename, &image_width, &image_height,
                                                  &image_channels, req_format);
    if (tileset_image_data == NULL)
    {
        fprintf(stdout, "image data is null after attempt to load\n");
        exit(EXIT_FAILURE);
    }

    sprite_sheet.texture_w = image_width;
    sprite_sheet.texture_h = image_height;

    int depth, pitch;
    SDL_Surface *image_surface;
    Uint32 pixel_format;

    depth = 32;
    pitch = 4 * image_width;
    pixel_format = SDL_PIXELFORMAT_RGBA32;

    image_surface = SDL_CreateRGBSurfaceWithFormatFrom(tileset_image_data, image_width, image_height, depth, pitch, pixel_format);

    SDLErrorHandleNull(image_surface);
    sprite_sheet.texture = SDL_CreateTextureFromSurface(Global::renderer, image_surface);
    SDLErrorHandleNull(sprite_sheet.texture);
    SDL_FreeSurface(image_surface);

    stbi_image_free(tileset_image_data);

    return sprite_sheet;
}


void
SpriteRender(SpriteSheet sprite_sheet, int sprite_sheet_row, int sprite_sheet_col, float scale, float tilemap_x,
             float tilemap_y, float parent_x, float parent_y, float local_sprite_x, float local_sprite_y,
             bool flip)
{
    SDL_Rect src_rect = {
            sprite_sheet_col * (int)sprite_sheet.cell_width(),
            sprite_sheet_row * (int)sprite_sheet.cell_height(),
            (int)sprite_sheet.cell_width(),
            (int)sprite_sheet.cell_height()
    };

    SDL_Rect dst_rect = {
            (int)round(tilemap_x + parent_x + local_sprite_x) - (int)round(sprite_sheet.cell_width() / 2 * scale),
            (int)round(tilemap_y + parent_y + local_sprite_y) - (int)round(sprite_sheet.cell_height() / 2 * scale),
            (int)round(sprite_sheet.cell_width() * scale),
            (int)round(sprite_sheet.cell_height() * scale)
    };

    SDL_RendererFlip flip_val = SDL_FLIP_NONE;
    if(flip)
    {
        flip_val = SDL_FLIP_HORIZONTAL;
    }


    SDL_RenderCopyEx(Global::renderer, sprite_sheet.texture, &src_rect, &dst_rect, 0, NULL, flip_val);
}


AnimatedSprite
AnimatedSpriteInit(SpriteSheet sprite_sheet, Uint32 start_frame, Uint32 end_frame, float FPS)
{
    AnimatedSprite animated_sprite;
    animated_sprite.sprite_sheet = sprite_sheet;
    animated_sprite.start_frame = start_frame;
    animated_sprite.end_frame = end_frame;
    animated_sprite.FPS = FPS;

    return animated_sprite;
}