
void clear_buffer(Game_Offscreen_Buffer *buffer, int color) 
{
    uint32_t *pixels = (uint32_t*) buffer->memory;
    for(int y = 0; y < buffer->height; y++) {
        for(int x = 0; x < buffer->width; x++) {
            *pixels++ = color;
        }
    }
}

#include <math.h>

void draw_rect(Game_Offscreen_Buffer *buffer, 
               vec2 min, vec2 max,
               int color)
{
    int min_x = roundf(min.x);
    int min_y = roundf(min.y);
    int max_x = roundf(max.x);
    int max_y = roundf(max.y);
    if(min_x < 0) {
        min_x = 0;
    }
    if(min_y < 0) {
        min_y = 0;
    }
    if(max_x > buffer->width) {
        max_x = buffer->width;
    }
    if(max_y > buffer->height) {
        max_y = buffer->height;
    }
    
    for(int y = min_y; y < max_y; y++) {
        uint32_t *pixels = (uint32_t*) buffer->memory + min_x + y*buffer->width;
        for(int x = min_x; x < max_x; x++) {
            *pixels++ = color;
        }
    }
}
