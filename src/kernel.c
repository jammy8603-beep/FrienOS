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

        uint32_t win_w = 460, win_h = 280;
        uint32_t win_x = (fb_width - win_w) / 2;
        uint32_t win_y = (fb_height - win_h) / 2;

        fill_rect(win_x, win_y, win_w, win_h, 0x334155);
        fill_rect(win_x, win_y, win_w, 36, 0x2563EB);
        fill_rect(win_x + 40, win_y + 90, win_w - 80, 34, 0x1E293B);
        fill_rect(win_x + 40, win_y + 145, win_w - 80, 34, 0x1E293B);
        fill_rect(win_x + 150, win_y + 205, 160, 40, 0x10B981);
    }

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
