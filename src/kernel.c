#include <stdint.h>
#include <stddef.h>

struct mb2_tag {
    uint32_t type;
    uint32_t size;
};

struct mb2_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t  bpp;
    uint8_t  fb_type;
    uint16_t reserved;
};

static uint32_t *fb = NULL;
static uint32_t fb_width = 0;
static uint32_t fb_height = 0;
static uint32_t fb_pitch_pixels = 0;

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    if (!fb) return;
    for (uint32_t row = 0; row < h; row++) {
        if (y + row >= fb_height) break;
        for (uint32_t col = 0; col < w; col++) {
            if (x + col >= fb_width) break;
            fb[(y + row) * fb_pitch_pixels + (x + col)] = color;
        }
    }
}

/* Minimal 5x7 glyph bitmask for uppercase A-Z, 0-9, space, colon, and dash */
static const uint8_t *get_glyph_5x7(char c) {
    static const uint8_t space[7] = {0,0,0,0,0,0,0};
    static const uint8_t dash[7]  = {0,0,0,0x1F,0,0,0};
    static const uint8_t colon[7] = {0,0x04,0x04,0,0x04,0x04,0};
    static const uint8_t alpha[26][7] = {
        {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}, /* A */
        {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}, /* B */
        {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}, /* C */
        {0x1C,0x12,0x11,0x11,0x11,0x12,0x1C}, /* D */
        {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}, /* E */
        {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}, /* F */
        {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F}, /* G */
        {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}, /* H */
        {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}, /* I */
        {0x07,0x02,0x02,0x02,0x12,0x12,0x0C}, /* J */
        {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, /* K */
        {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}, /* L */
        {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}, /* M */
        {0x11,0x19,0x15,0x13,0x11,0x11,0x11}, /* N */
        {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, /* O */
        {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}, /* P */
        {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}, /* Q */
        {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}, /* R */
        {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}, /* S */
        {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}, /* T */
        {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}, /* U */
        {0x11,0x11,0x11,0x11,0x11,0x0A,0x04}, /* V */
        {0x11,0x11,0x11,0x15,0x15,0x1B,0x11}, /* W */
        {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}, /* X */
        {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}, /* Y */
        {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}  /* Z */
    };
    static const uint8_t digits[10][7] = {
        {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}, /* 0 */
        {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}, /* 1 */
        {0x0E,0x11,0x01,0x06,0x08,0x10,0x1F}, /* 2 */
        {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E}, /* 3 */
        {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, /* 4 */
        {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}, /* 5 */
        {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}, /* 6 */
        {0x1F,0x01,0x02,0x04,0x08,0x08,0x08}, /* 7 */
        {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, /* 8 */
        {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}  /* 9 */
    };

    if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
    if (c >= 'A' && c <= 'Z') return alpha[c - 'A'];
    if (c >= '0' && c <= '9') return digits[c - '0'];
    if (c == '-') return dash;
    if (c == ':') return colon;
    return space;
}

static void draw_text(uint32_t x, uint32_t y, uint32_t scale, const char *s, uint32_t color) {
    uint32_t cursor_x = x;
    for (size_t i = 0; s[i] != '\0'; i++) {
        const uint8_t *g = get_glyph_5x7(s[i]);
        for (uint32_t r = 0; r < 7; r++) {
            for (uint32_t c = 0; c < 5; c++) {
                if (g[r] & (1u << (4 - c))) {
                    fill_rect(cursor_x + c * scale, y + r * scale, scale, scale, color);
                }
            }
        }
        cursor_x += 6 * scale;
    }
}

/* Basic US QWERTY Set 1 scancode map */
static char scancode_to_char(uint8_t sc) {
    static const char map[128] = {
        0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', '\t',
        'Q','W','E','R','T','Y','U','I','O','P','[',']', '\n', 0,
        'A','S','D','F','G','H','J','K','L',';','\'','`', 0, '\\',
        'Z','X','C','V','B','N','M',',','.','/', 0, '*', 0, ' ',
        [0 ... 127] = 0
    };
    return map[sc & 0x7F];
}

void kernel_main(uint64_t mb2_info_addr) {
    struct mb2_tag *tag = (struct mb2_tag *)(mb2_info_addr + 8);
    while (tag->type != 0) {
        if (tag->type == 8) {
            struct mb2_tag_framebuffer *fb_tag = (struct mb2_tag_framebuffer *)tag;
            fb = (uint32_t *)(uintptr_t)fb_tag->addr;
            fb_width = fb_tag->width;
            fb_height = fb_tag->height;
            fb_pitch_pixels = fb_tag->pitch / 4;
            break;
        }
        tag = (struct mb2_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7));
    }

    if (fb) {
        fill_rect(0, 0, fb_width, fb_height, 0x1E293B);
        fill_rect(0, 0, fb_width, 32, 0x0F172A);
        draw_text(16, 9, 2, "FRIENOS", 0xF8FAFC);

        uint32_t win_w = 460, win_h = 280;
        uint32_t win_x = (fb_width - win_w) / 2;
        uint32_t win_y = (fb_height - win_h) / 2;
