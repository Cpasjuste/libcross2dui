//
// Created by cpasjuste on 01/12/16.
//

#ifdef __SWITCH__

#include <png.h>
#include "c2dui.h"

#ifdef __PFBA__
// TODO: remove pfba deps
#define BDF_ORIENTATION_FLIPPED     (1 << 1)
#define BDF_ORIENTATION_VERTICAL    (1 << 2)

extern "C" int BurnDrvGetFlags();

#endif

using namespace c2d;
using namespace c2dui;

C2DUINXVideo::C2DUINXVideo(C2DUIGuiMain *gui, void **_pixels, int *_pitch, const c2d::Vector2f &size)
        : Texture(size, C2D_TEXTURE_FMT_RGB565) {

    this->ui = gui;

    printf("C2DUINXVideo::C2DUINXVideo(%i, %i)\n", (int) getSize().x, (int) getSize().y);

    pixels = (unsigned char *) malloc((size_t) (size.x * size.y * bpp));
    lock(nullptr, _pixels, _pitch);

    updateScaling();

    //printf("C2DUINXVideo::C2DUINXVideo(%i, %i)\n", (int) getSize().x, (int) getSize().y);
}

void C2DUINXVideo::clear() {

    for (int i = 0; i < 2; i++) {

        u32 w, h;
        u32 *fb = (u32 *) gfxGetFramebuffer(&w, &h);

        for (u32 y = 0; y < h; y++) {
            for (u32 x = 0; x < w; x++) {
                fb[gfxGetFramebufferDisplayOffset((u32) x, (u32) y)] =
                        (u32) RGBA8_MAXALPHA(0, 0, 0);
            }
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }
}

void C2DUINXVideo::draw(c2d::Transform &transform) {

    // dont draw with cross2d, we directly write to the framebuffer
}

int C2DUINXVideo::save(const char *path) {

    unsigned char *shot = pixels;
    unsigned char *converted = NULL;
    png_bytep *rows = NULL;

    const char *szAuthor = "Cpasjuste";
    const char *szDescription = "Screenshot";
    const char *szCopyright = "Cpasjuste";
    const char *szSoftware = "libcross2d @ libpng";
    const char *szSource = "libcross2d";

    FILE *ff = nullptr;
    png_text text_ptr[7];
    int num_text = 7;
    time_t currentTime;
    png_time_struct png_time_now;

    int w = (int) getSize().x, h = (int) getSize().y;

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        return -1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(ff);
        remove(path);
        return -1;
    }

    // Convert the image to 32-bit
    if (bpp < 4) {
        unsigned char *pTemp = (unsigned char *) malloc(w * h * sizeof(int));
        if (bpp == 2) {
            for (int i = 0; i < h * w; i++) {
                signed short nColour = ((signed short *) shot)[i];
                // Red
                *(pTemp + i * 4 + 0) = (unsigned char) ((nColour & 0x1F) << 3);
                *(pTemp + i * 4 + 0) |= *(pTemp + 4 * i + 0) >> 5;
                // Green
                *(pTemp + i * 4 + 1) = (unsigned char) (((nColour >> 5) & 0x3F) << 2);
                *(pTemp + i * 4 + 1) |= *(pTemp + i * 4 + 1) >> 6;
                // Blue
                *(pTemp + i * 4 + 2) = (unsigned char) (((nColour >> 11) & 0x1F) << 3);
                *(pTemp + i * 4 + 2) |= *(pTemp + i * 4 + 2) >> 5;
            }
        } else {
            memset(pTemp, 0, w * h * sizeof(int));
            for (int i = 0; i < h * w; i++) {
                *(pTemp + i * 4 + 0) = *(shot + i * 3 + 0);
                *(pTemp + i * 4 + 1) = *(shot + i * 3 + 1);
                *(pTemp + i * 4 + 2) = *(shot + i * 3 + 2);
            }
        }
        converted = pTemp;
        shot = converted;
    }

    // Get the time
    time(&currentTime);
    png_convert_from_time_t(&png_time_now, currentTime);

    ff = fopen(path, "wb");
    if (ff == NULL) {
        printf("C2DUINXVideo::save: fopen failed: `%s`\n", path);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        if (converted) {
            free(converted);
        }
        return -1;
    }

    // Fill the PNG text fields
    text_ptr[0].key = (png_charp) "Title";
    text_ptr[0].text = (png_charp) "ROM";
    text_ptr[1].key = (png_charp) "Author";
    text_ptr[1].text = (png_charp) szAuthor;
    text_ptr[2].key = (png_charp) "Description";
    text_ptr[2].text = (png_charp) szDescription;
    text_ptr[3].key = (png_charp) "Copyright";
    text_ptr[3].text = (png_charp) szCopyright;
    text_ptr[4].key = (png_charp) "Software";
    text_ptr[4].text = (png_charp) szSoftware;
    text_ptr[5].key = (png_charp) "Source";
    text_ptr[5].text = (png_charp) szSource;
    text_ptr[6].key = (png_charp) "Comment";
    text_ptr[6].text = (png_charp) "Created by running the game in an emulator";
    for (int i = 0; i < num_text; i++) {
        text_ptr[i].compression = PNG_TEXT_COMPRESSION_NONE;
    }
    png_set_text(png_ptr, info_ptr, text_ptr, num_text);
    png_init_io(png_ptr, ff);
    png_set_IHDR(png_ptr, info_ptr, (png_uint_32) w, (png_uint_32) h, 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);
    png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);
    png_set_bgr(png_ptr);
    rows = (png_bytep *) malloc(h * sizeof(png_bytep));
    for (int y = 0; y < h; y++) {
        rows[y] = shot + (y * w * sizeof(int));
    }
    png_write_image(png_ptr, rows);
    png_write_end(png_ptr, info_ptr);
    if (rows) {
        free(rows);
    }
    fclose(ff);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    if (converted) {
        free(converted);
    }

    return 0;
}

int C2DUINXVideo::lock(c2d::FloatRect *rect, void **pix, int *p) {

    if (!rect) {
        *pix = pixels;
    } else {
        *pix = (void *) (pixels + (int) rect->top * pitch + (int) rect->left * bpp);
    }

    if (p) {
        *p = pitch;
    }

    return 0;
}

void C2DUINXVideo::unlock() {

    //printf("C2DUINXVideo::unlock()\n");

    bool point = filtering == C2D_TEXTURE_FILTER_POINT;

    unsigned short *tex_buf = (unsigned short *) pixels;
    int x, y, w, h, cx, cy;
    unsigned int p, r, g, b, sf = 1;
    unsigned subx, suby;
    u32 fb_w, fb_h;
    u32 pixel;

    Vector2f screen = ui->getRenderer()->getSize();
    s32 vw = point ? (s32) screen.x : (s32) (screen.x / getScale().x);
    s32 vh = point ? (s32) screen.y : (s32) (screen.y / getScale().y);

    // rotation
    if (getRotation() == 0) {
        gfxConfigureTransform(NATIVE_WINDOW_TRANSFORM_FLIP_V);
    } else if (getRotation() == 90) {
        gfxConfigureTransform(NATIVE_WINDOW_TRANSFORM_FLIP_V
                              | NATIVE_WINDOW_TRANSFORM_ROT_90);
        vw = point ? (s32) screen.x : (s32) (screen.y / getScale().x);
        vh = point ? (s32) screen.y : (s32) (screen.x / getScale().y);
    } else if (getRotation() == 180) {
        gfxConfigureTransform(NATIVE_WINDOW_TRANSFORM_FLIP_H);
    } else if (getRotation() == 270) {
        gfxConfigureTransform(NATIVE_WINDOW_TRANSFORM_FLIP_H
                              | NATIVE_WINDOW_TRANSFORM_ROT_90);
        vw = point ? (s32) screen.x : (s32) (screen.y / getScale().x);
        vh = point ? (s32) screen.y : (s32) (screen.x / getScale().y);
    }

    gfxConfigureResolution(vw, vh);
    u32 *fb_buf = (u32 *) gfxGetFramebuffer(&fb_w, &fb_h);

    w = (int) getSize().x;
    h = (int) getSize().y;
    // point scaling (1x, 2x, 3x)
    if (point) {
        sf = (unsigned int) std::max((int) getScale().x, (int) getScale().y);
        // be sure we don't scale higher than fb size
        while (w * sf > fb_w || h * sf > fb_h) {
            sf--;
        }
    }

    // center image
    cx = (fb_w - (w * sf)) / 2;
    cy = (fb_h - (h * sf)) / 2;

    //printf("res:%ix%i | fb:%ix%i | c:%ix%i | tex:%ix%i | scale:%fx%f (point=%i)\n",
    //       vw, vh, fb_w, fb_h, cx, cy, (int) getSize().x, (int) getSize().y, getScale().x, getScale().y, sf);

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {

            p = tex_buf[y * w + x];
            r = ((p & 0xf800) >> 11) << 3;
            g = ((p & 0x07e0) >> 5) << 2;
            b = (p & 0x001f) << 3;
            pixel = RGBA8_MAXALPHA(r, g, b);

            if (point) {
                for (subx = 0; subx < sf; subx++) {
                    for (suby = 0; suby < sf; suby++) {
                        fb_buf[(u32) gfxGetFramebufferDisplayOffset(
                                (u32) ((x * sf) + subx + cx),
                                (u32) ((y * sf) + suby + cy))] = pixel;
                    }
                }
            } else {
                fb_buf[(u32) gfxGetFramebufferDisplayOffset((u32) x + cx, (u32) y + cy)] = pixel;
            }
        }
    }

    gfxFlushBuffers();
    gfxSwapBuffers();
    gfxWaitForVsync();
}

void C2DUINXVideo::updateScaling() {

    //printf("C2DUINXVideo::updateScaling()\n");

    int rotated = 0;
    float rotation = 0;
    int rotation_cfg = ui->getConfig()->getValue(C2DUIOption::Index::ROM_ROTATION, true);
    int scale_mode = ui->getConfig()->getValue(C2DUIOption::Index::ROM_SCALING, true);
#ifdef __PFBA__
    int vertical = BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL;
    int flip = BurnDrvGetFlags() & BDF_ORIENTATION_FLIPPED;
#else
    int vertical = false;
    int flip = false;
#endif

    Vector2f screen = ui->getRenderer()->getSize();
    Vector2f scale_max;
    float sx = 1, sy = 1;

    //gfxSetMode(GfxMode_TiledDouble);
    setFiltering(ui->getConfig()->getValue(C2DUIOption::Index::ROM_FILTER, true));

    // clear fb before changing res/rot
    clear();

    if (vertical) {
        switch (rotation_cfg) {
            case 1: // ON
                rotation = flip ? 90 : 270;
                rotated = 1;
                break;
            case 2: // FLIP // TODO
            case 3: // CAB MODE
                rotation = flip ? 0 : 180;
                break;
            default: // OFF
                rotation = flip ? 180 : 0;
                break;
        }
    }

    if (rotated) {
        scale_max.x = screen.x / getSize().y;
        scale_max.y = screen.y / getSize().x;
    } else {
        scale_max.x = screen.x / getSize().x;
        scale_max.y = screen.y / getSize().y;
    }

    switch (scale_mode) {

        case 0: // 1x
            if (rotated) {
                // TODO
            }
            break;

        case 1: // 2x
        case 2: // 3x
            sx = sy = std::min(scale_max.x, (float) (scale_mode + 1));
            if (sy > scale_max.y) {
                sx = sy = std::min(scale_max.y, (float) (scale_mode + 1));
            }
            break;

        case 3: // fit
            sx = sy = scale_max.y;
            if (sx > scale_max.x) {
                sx = sy = scale_max.x;
            }
            break;

        case 4: // fit 4:3
            if (rotated) {
                sx = scale_max.y;
                float size_y = sx * getSize().x * 1.33f;
                sy = std::min(scale_max.x, size_y / getSize().y);
            } else {
                sy = scale_max.y;
                float size_x = sy * getSize().y * 1.33f;
                sx = std::min(scale_max.x, size_x / getSize().x);
            }
            break;

        case 5: // fullscreen
            sx = rotated ? scale_max.y : scale_max.x;
            sy = rotated ? scale_max.x : scale_max.y;
            break;

        default:
            break;
    }

    setOriginCenter();
    setPosition(screen.x / 2, screen.y / 2);
    setScale(sx, sy);
    setRotation(rotation);

    //printf("C2DUINXVideo::updateScaling()\n");
}

C2DUINXVideo::~C2DUINXVideo() {

    printf("C2DUINXVideo::~C2DUINXVideo()\n");
    if (pixels) {
        free(pixels);
    }
    printf("Done\n");
}

#endif
