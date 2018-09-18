//
// Created by cpasjuste on 01/12/16.
//

#include "c2dui.h"

#ifdef __PFBA__
// TODO: remove pfba deps
#define BDF_ORIENTATION_FLIPPED     (1 << 1)
#define BDF_ORIENTATION_VERTICAL    (1 << 2)

extern "C" int BurnDrvGetFlags();

#endif

using namespace c2d;
using namespace c2dui;

C2DUIVideo::C2DUIVideo(C2DUIGuiMain *gui, void **_pixels, int *_pitch,
                       const c2d::Vector2f &size, int format)
        : C2DTexture(size, format) {

    printf("game resolution: %ix%i\n", (int) getTextureRect().width, (int) getTextureRect().height);

    this->ui = gui;

    if (_pixels) {
        lock(nullptr, _pixels, _pitch);
        unlock();
    }

    setShader(ui->getConfig()->getValue(C2DUIOption::Index::ROM_SHADER, true));
    setFiltering(ui->getConfig()->getValue(C2DUIOption::Index::ROM_FILTER, true));
    updateScaling();
}

void C2DUIVideo::updateScaling() {

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

#ifndef __PSP2__
    if (vertical) {
        switch (rotation_cfg) {
            case 1: // ON
                rotation = flip ? 90 : 270;
                rotated = 1;
                break;
            case 2: // CAB MODE
                rotation = flip ? 0 : 180;
                break;
            default: // OFF
                rotation = flip ? 180 : 0;
                break;
        }
    }
#else
    // TODO: force right to left orientation on psp2,
    // should add platform specific code
    if ((rotation_cfg == 0 || rotation_cfg == 3) && vertical) {
        if (!flip) {
            rotation = 180;
        }
    } else if (rotation_cfg == 2 && vertical) {
        if (flip) {
            rotation = 180;
        }
    } else {
        if (flip) {
            rotation = 90;
            rotated = 1;
        } else if (vertical) {
            rotation = -90;
            rotated = 1;
        } else {
            rotation = 0;
        }
    }
#endif

    if (rotated) {
        scale_max.x = screen.x / getTextureRect().height;
        scale_max.y = screen.y / getTextureRect().width;
    } else {
        scale_max.x = screen.x / getTextureRect().width;
        scale_max.y = screen.y / getTextureRect().height;
    }

    switch (scale_mode) {

        case 1: // 2x
            sx = sy = std::min(scale_max.x, 2.0f);
            if (sy > scale_max.y) {
                sx = sy = std::min(scale_max.y, 2.0f);
            }
            break;

        case 2: // 3x
            sx = sy = std::min(scale_max.x, 3.0f);
            if (sy > scale_max.y) {
                sx = sy = std::min(scale_max.y, 3.0f);
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
                float size_y = sx * getTextureRect().width * 1.33f;
                sy = std::min(scale_max.x, size_y / getTextureRect().height);
            } else {
                sy = scale_max.y;
                float size_x = sy * getTextureRect().height * 1.33f;
                sx = std::min(scale_max.x, size_x / getTextureRect().width);
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
}
