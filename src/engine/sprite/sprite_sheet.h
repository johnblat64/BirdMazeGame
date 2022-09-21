#include <external/SDL2/include/SDL.h>
#include <vector>
#include <map>

struct SpriteSheet
{
    SDL_Texture *texture;
    float texture_w;
    float texture_h;
    size_t n_rows;
    size_t n_cols;
};


struct SpriteSheetBitmask
{
    const char *sprite_sheet_name;
    std::vector<Uint8> bitmasks;
};

static std::map<std::string, SpriteSheet> sprite_sheets;


void sprite_sheet_create_from_file(const char *filename, const char *sprite_sheet_name, int rows, int cols);
SpriteSheet sprite_sheet_get_by_name(std::string sprite_sheet_name);