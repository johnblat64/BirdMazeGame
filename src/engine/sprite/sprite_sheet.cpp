#include <src/engine/sprite/sprite_sheet.h>
#include <external/stb/stb_image.h>
#include <stdio.h>
#include <src/engine/util/util_error_handling.h>
#include <src/engine/global.h>


SpriteSheet SpriteSheetCreateFromFile(const char *filename, const char *sprite_sheet_name, int rows, int cols)
{
    SpriteSheet sprite_sheet;

    int req_format = STBI_rgb_alpha;
    int image_width, image_height, image_channels;
    unsigned char *tileset_image_data = stbi_load(filename, &image_width, &image_height,
                                                  &image_channels, req_format);
    if (tileset_image_data == NULL)
    {
        fprintf(stdout, "image data is null after attempt to load");
        exit(EXIT_FAILURE);
    }

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

}