//
// Created by cpasjuste on 25/11/16.
//

#ifdef __SWITCH__
#ifndef NX_VIDEO_H_
#define NX_VIDEO_H_

#include <cstring>

#define C2DUIVideo C2DUINXVideo

namespace c2dui {

    class C2DUINXVideo : public c2d::Texture {

    public:

        C2DUINXVideo(C2DUIGuiMain *ui, void **pixels, int *pitch, const c2d::Vector2f &size);

        ~C2DUINXVideo();

        void updateScaling();

        int lock(c2d::FloatRect *rect, void **pixels, int *pitch);

        void unlock();

        int save(const char *path);

        void clear();

        unsigned char *pixels = NULL;

    private:

        C2DUIGuiMain *ui = NULL;
        int scale_mode = 0;

        void draw(c2d::Transform &transform);
    };
}

#endif //NX_VIDEO_H_
#endif //__SWITCH__