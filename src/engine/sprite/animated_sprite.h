#include <stddef.h>
#include <map>
#include <vector>

struct Animation
{
    std::vector<unsigned int> frames; // indices into the sprite_sheet
    unsigned int curr_frame;
    float ms_per_frame;
    float accumulator;
    bool is_loop;
};

struct AnimatedSprite
{
    const char *sprite_sheet_name;
    std::string curr_animation_name;
    bool is_playing;
    std::map<std::string, Animation> animations;
};


AnimatedSprite AnimatedSprite_create(const char *sprite_sheet_name);
void AnimatedSprite_add_Animation(Animation animation);
Animation Animation_create(const char *animation_name, float FPS);
void Animation_add_frame(Animation *animation, unsigned int frame);



