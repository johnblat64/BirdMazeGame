#include <src/engine/sprite/sprite_sheet.h>
#include <external/stb/stb_image.h>
#include <stdio.h>
#include <src/engine/util/util_error_handling.h>
#include <src/engine/global.h>
#include <math.h>


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
SpriteRender(SpriteSheet sprite_sheet, int sprite_sheet_row, int sprite_sheet_col, float pos_x, float pos_y)
{
    SDL_Rect src_rect = {
            sprite_sheet_col * (int)sprite_sheet.cell_width(),
            sprite_sheet_row * (int)sprite_sheet.cell_height(),
            (int)sprite_sheet.cell_width(),
            (int)sprite_sheet.cell_height()
    };

    SDL_Rect dst_rect = {
            (int)round(pos_x) - (int)sprite_sheet.cell_width()/2,
            (int)round(pos_y) - (int)sprite_sheet.cell_height()/2,
            (int)sprite_sheet.cell_width(),
            (int)sprite_sheet.cell_height()
    };


    SDL_RenderCopy(Global::renderer, sprite_sheet.texture, &src_rect, &dst_rect);
}