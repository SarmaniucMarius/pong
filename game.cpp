#include "game.h"

#include "vec2.cpp"
#include "renderer.cpp"

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down) && (input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down) && (input->buttons[b].changed)

vec2 window_dim = {WINDOW_WIDTH, WINDOW_HEIGHT};
vec2 window_center = window_dim*0.5;

vec2 ball_pos = window_center-10;
vec2 ball_dim = {20, 20};
vec2 ball_dp = {3.0f, 0.0f};

vec2 paddle_dim = {40, 200};
vec2 paddle1_pos = {50+20, window_center.y - paddle_dim.y/2};
float paddle1_dp;
vec2 paddle2_pos = {window_dim.x-paddle1_pos.x-paddle_dim.x, paddle1_pos.y};
float paddle2_dp;

int player1_score, player2_score;

bool aabb_vs_aabb(vec2 a_min, vec2 a_max, vec2 b_min, vec2 b_max)
{
    return a_max.x >= b_min.x && b_max.x >= a_min.x &&
        a_max.y >= b_min.y && b_max.y >= a_min.y;
}

void simulate_player(vec2 *paddle_pos, float *paddle_dp, float paddle_ddp, float dt)
{
    paddle_ddp += -*paddle_dp;
    
    *paddle_dp += paddle_ddp*dt;
    paddle_pos->y += paddle_ddp*0.5*dt*dt + *(paddle_dp)*dt;
    
    if(paddle_pos->y + paddle_dim.y > window_dim.y - 50) {
        paddle_pos->y = window_dim.y - 50 - paddle_dim.y;
        *paddle_dp *= 0;
    } else if(paddle_pos->y < 50) {
        paddle_pos->y = 50;
        *paddle_dp *= 0;
    }
}

void simulate_game(Game_Offscreen_Buffer *buffer, Input *input, float dt)
{
    clear_buffer(buffer, 0xff7700);
    vec2 board_pos = {50, 50};
    draw_rect(buffer, board_pos, window_dim-board_pos, 0xffbb00);
    
    float paddle1_ddp = 0.0;
    if(is_down(BUTTON_W)){
        paddle1_ddp = 5000.0f;
    }
    if(is_down(BUTTON_S)){
        paddle1_ddp = -5000.0f;
    }
#if 0
    float paddle2_ddp = 0.0f;
    if(is_down(BUTTON_UP)){
        paddle2_ddp= 5000.0f;
    }
    if(is_down(BUTTON_DOWN)){
        paddle2_ddp= -5000.0f;
    }
#else
    float paddle2_ddp = 0.0f;
    vec2 ball_center = ball_pos + ball_dim*0.5;
    vec2 paddle2_center = paddle2_pos + paddle_dim*0.5;
    if(ball_center.y > paddle2_center.y) {
        paddle2_ddp = 5000.0f;
    }
    if(ball_center.y < paddle2_center.y) {
        paddle2_ddp = -5000.0f;
    }
#endif
    
    simulate_player(&paddle1_pos, &paddle1_dp, paddle1_ddp, dt);
    simulate_player(&paddle2_pos, &paddle2_dp, paddle2_ddp, dt);
    
    ball_pos += ball_dp;
    if(aabb_vs_aabb(ball_pos, ball_pos + ball_dim, 
                    paddle2_pos, paddle2_pos + paddle_dim)) {
        float ball_center_pos_y = ball_pos.y + ball_dim.y/2;
        float paddle2_center_pos_y = paddle2_pos.y + paddle_dim.y/2;
        ball_pos.x = paddle2_pos.x - ball_dim.x;
        ball_dp.x *= -1;
        ball_dp.y = (paddle2_center_pos_y - ball_center_pos_y)/10;
    } 
    else if(aabb_vs_aabb(ball_pos, ball_pos + ball_dim,
                         paddle1_pos, paddle1_pos + paddle_dim)) {
        float ball_center_pos_y = ball_pos.y + ball_dim.y/2;
        float paddle1_center_pos_y = paddle1_pos.y + paddle_dim.y/2;
        ball_pos.x = paddle1_pos.x + paddle_dim.x;
        ball_dp.x *= -1;
        ball_dp.y = (paddle1_center_pos_y - ball_center_pos_y)/10;
    }
    
    if(ball_pos.y <= 50) {
        ball_pos.y = 50;
        ball_dp.y *= -1;
    } else if(ball_pos.y >= window_dim.y - 50) {
        ball_pos.y = window_dim.y - 50;
        ball_dp.y *= -1;
    }
    
    if(ball_pos.x <= 50) {
        player2_score++;
        ball_pos = window_center-10;
        ball_dp = {-3.0, 0.0};
    } else if(ball_pos.x >= window_dim.x - 50) {
        player1_score++;
        ball_pos = window_center-10;
        ball_dp = {3.0, 0.0};
    }
    
    vec2 player_score_dim = {10, 10};
    vec2 player1_score_pos = {30, 30};
    for(int i = 0; i < player1_score; i++) {
        player1_score_pos.x += player_score_dim.x + 10;
        draw_rect(buffer, 
                  player1_score_pos, player1_score_pos+player_score_dim, 
                  0x000000);
    }
    
    vec2 player2_score_pos = {window_dim.x-40, 30};
    for(int i = 0; i < player2_score; i++) {
        player2_score_pos.x -= player_score_dim.x + 10;
        draw_rect(buffer, 
                  player2_score_pos, player2_score_pos+player_score_dim, 
                  0x000000);
    }
    
    draw_rect(buffer, ball_pos, ball_pos + ball_dim, 0x00ff33);
    draw_rect(buffer, paddle1_pos, paddle1_pos+paddle_dim, 0xff0000);
    draw_rect(buffer, paddle2_pos, paddle2_pos+paddle_dim, 0xff0000);
}