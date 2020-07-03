#include <stdint.h>
#include <windows.h>

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

struct Off_Screen_Buffer {
    BITMAPINFO info;
    int width;
    int height;
    int bytes_per_pixel;
    void *memory;
};

bool running = true;

#include "platform_common.h"
#include "game.cpp"

void create_bitmap_buffer(Off_Screen_Buffer *bitmap, int width, int height)
{
    bitmap->width = width;
    bitmap->height = height;
    bitmap->bytes_per_pixel = 4;
    
    bitmap->info.bmiHeader.biSize = sizeof(bitmap->info.bmiHeader);
    bitmap->info.bmiHeader.biWidth = bitmap->width;
    bitmap->info.bmiHeader.biHeight = bitmap->height;
    bitmap->info.bmiHeader.biPlanes = 1;
    bitmap->info.bmiHeader.biBitCount = 32;
    bitmap->info.bmiHeader.biCompression = BI_RGB;
    bitmap->info.bmiHeader.biSizeImage = 0;
    
    int buffer_memory_size = bitmap->width * bitmap->height * bitmap->bytes_per_pixel;
    bitmap->memory = VirtualAlloc(0, buffer_memory_size, MEM_COMMIT, PAGE_READWRITE);
}

void display_bitmap(Off_Screen_Buffer *bitmap, HWND window)
{
    HDC device_context = GetDC(window);
    StretchDIBits(device_context,
                  0, 0,
                  bitmap->width, bitmap->height,
                  0, 0,
                  bitmap->width, bitmap->height,
                  bitmap->memory,
                  &bitmap->info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = {};
    switch(msg) {
        case WM_DESTROY:
        case WM_CLOSE:
        {
            running = false;
        } break;
        
        default:
        {
            result = DefWindowProc(window, msg, w_param, l_param);
        } break;
    }
    
    return result;
}

int WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmdline, int command)
{
    WNDCLASS window_class = {};
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    // window_class.hIcon;
    // window_class.hCursor;
    window_class.lpszClassName = "Pong Game Window";
    
    RegisterClass(&window_class);
    
    HWND window = CreateWindow(window_class.lpszClassName,
                               "Pong",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               WINDOW_WIDTH + 30, WINDOW_HEIGHT + 50,
                               0, 0, instance, 0);
    
    Off_Screen_Buffer bitmap = {};
    create_bitmap_buffer(&bitmap, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    Input input = {};
    
    float refresh_rate = 30.0;
    float dt = 1.0 / refresh_rate;
    
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    
    LARGE_INTEGER last_counter;
    QueryPerformanceCounter(&last_counter);
    while(running) {
        
        for(int i = 0; i < BUTTON_COUNT; i++) {
            input.buttons[i].changed = false;
        }
        
        MSG message = {};
        while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
            switch(message.message) {
                case WM_KEYUP:
                case WM_KEYDOWN:
                {
                    int vk_code = message.wParam;
                    bool is_down = (message.lParam >> 31) == 0;
                    
#define process_button(b, vk)\
                    case vk:\
                    {\
                        input.buttons[b].changed = input.buttons[b].is_down != is_down;\
                        input.buttons[b].is_down = is_down;\
                    } break;
                    switch(vk_code) {
                        process_button(BUTTON_UP, VK_UP);
                        process_button(BUTTON_DOWN, VK_DOWN);
                        process_button(BUTTON_W, 'W');
                        process_button(BUTTON_S, 'S');
                        process_button(BUTTON_LEFT, VK_LEFT);
                        process_button(BUTTON_RIGHT, VK_RIGHT);
                        process_button(BUTTON_ESC, VK_ESCAPE);
                    }
                } break;
                
                default:
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                } break;
            }
        }
        
        Game_Offscreen_Buffer game_buffer = {};
        game_buffer.width = bitmap.width;
        game_buffer.height = bitmap.height;
        game_buffer.bytes_per_pixel = bitmap.bytes_per_pixel;
        game_buffer.memory = bitmap.memory;
        simulate_game(&game_buffer, &input, dt);
        
        display_bitmap(&bitmap, window);
        
        LARGE_INTEGER current_counter;
        QueryPerformanceCounter(&current_counter);
        dt = (current_counter.QuadPart - last_counter.QuadPart) /(float)frequency.QuadPart;
        
        last_counter = current_counter;
        
        if(input.buttons[BUTTON_ESC].is_down) {
            running = false;
        }
    }
    
    return 0;
}