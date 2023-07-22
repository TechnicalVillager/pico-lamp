#include "pico/stdlib.h"
#include "ws2812b.h"
#include "hardware/uart.h"
#include <string.h>

#define IS_RGBW false
#define NUM_PIXELS 7
#define WS2812_PIN 28
#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1

char message[20];
int message_length = 0;
uint32_t strip[NUM_PIXELS];

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

void set_pixel (uint8_t r, uint8_t g, uint8_t b, int pixels) {
    if (pixels < NUM_PIXELS) {
        strip [pixels] = urgb_u32(r, g, b);
    }
}

void update_strip () {
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel(strip [i]);
    }
}

void breathing_pattern (int length, char* color) {
    // BREATHE OUT
    for (int breath = 0; breath <= 250; breath++) {
        for (int i = 0; i < length; i++) {
            if (color == "RED") {
                set_pixel (breath, 0, 0, i);
            }
            if (color == "BLUE") {
                set_pixel (0, 0, breath, i);
            }
            if (color == "GREEN") {
                set_pixel (0, breath, 0, i);
            }
            if (color == "YELLOW") {
                set_pixel (breath, breath, 0, i);
            }
            if (color == "PINK") {
                set_pixel (breath, 0, breath, i);
            }
            if (color == "WHITE") {
                set_pixel (breath, breath, breath, i);
            }
        }
        update_strip ();
        sleep_ms (5);
    }
    
    sleep_ms (50);
    
    // BREATHE IN
    for (int breath = 250; breath >= 0; breath--) {
        for (int i = 0; i < length; i++) {
            if (color == "RED") {
                set_pixel (breath, 0, 0, i);
            }
            if (color == "BLUE") {
                set_pixel (0, 0, breath, i);
            }
            if (color == "GREEN") {
                set_pixel (0, breath, 0, i);
            }
            if (color == "YELLOW") {
                set_pixel (breath, breath, 0, i);
            }
            if (color == "PINK") {
                set_pixel (breath, 0, breath, i);
            }
            if (color == "WHITE") {
                set_pixel (breath, breath, breath, i);
            }
        }
        update_strip ();
        sleep_ms (5);
    }
    sleep_ms (50);
}

void snake_pattern (int length, char* color) {
    for (int i = 0; i < length; i++) {
        if (color == "RED") {
            set_pixel (250, 0, 0, i);
        }
        if (color == "BLUE") {
            set_pixel (0, 0, 250, i);
        }
        if (color == "GREEN") {
            set_pixel (0, 250, 0, i);
        }
        if (color == "YELLOW") {
            set_pixel (250, 250, 0, i);
        }
        if (color == "PINK") {
            set_pixel (250, 0, 250, i);
        }
        if (color == "WHITE") {
            set_pixel (250, 250, 250, i);
        }
        if (color == "OFF") {
            set_pixel (0, 0, 0, i);
        }
        update_strip ();
        sleep_ms (100);
    }
}

void loop_breathing_pattern (int length) {
    char *colors[] = {"RED", "GREEN", "BLUE", "YELLOW", "WHITE", "PINK"};
    int n = sizeof(colors) / sizeof(colors[0]);
    for (int i = 0; i < n; i++) {
        breathing_pattern (length, colors[i]);
    }
}

void loop_snake_pattern (int length) {
    char *colors[] = {"RED", "GREEN", "BLUE", "YELLOW", "WHITE", "PINK", "OFF"};
    int n = sizeof(colors) / sizeof(colors[0]);
    for (int i = 0; i < n; i++) {
        snake_pattern (length, colors[i]);
    }
}

void all_light (int length, char* color) {
    for (int i = 0; i < length; i++) {
        if (color == "RED") {
            set_pixel (250, 0, 0, i);
        }
        if (color == "BLUE") {
            set_pixel (0, 0, 250, i);
        }
        if (color == "GREEN") {
            set_pixel (0, 250, 0, i);
        }
        if (color == "YELLOW") {
            set_pixel (250, 250, 0, i);
        }
        if (color == "PINK") {
            set_pixel (250, 0, 250, i);
        }
        if (color == "WHITE") {
            set_pixel (250, 250, 250, i);
        }
        if (color == "OFF") {
            set_pixel (0, 0, 0, i);
        }
    }
    update_strip();
}

void recv_message () {
    while (true) {
        char a = uart_getc(uart0);
        if ((a != '{') && (a != '}')) {
            message[message_length] = a;
            message_length++;
        } else if (a == '{') {
            message_length = 0;
        } else if (a == '}') {
            break;
        }
    }
}

int main() {
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    while (true){
        recv_message();
        char *colors[]    = {"RED", "GREEN", "BLUE", "YELLOW", "WHITE", "PINK", "OFF"};
        char *patterns[]  = {"BREATHING_PATTERN", "SNAKE_PATTERN"};
        int colors_list   = sizeof(colors) / sizeof(colors[0]);
        int patterns_list = sizeof(patterns) / sizeof(patterns[0]);

        for (int i = 0; i < colors_list; i++) {
            if (strncmp(message, colors[i], message_length) == 0) {
                all_light (NUM_PIXELS, colors[i]);
            }
        }
        for (int i = 0; i < patterns_list; i++) {
            if (strncmp(message, patterns[i], message_length) == 0) {
                if (i == 0) {
                    loop_breathing_pattern (NUM_PIXELS);
                } else {
                    loop_snake_pattern (NUM_PIXELS);
                }
            }
        }
    }
}
