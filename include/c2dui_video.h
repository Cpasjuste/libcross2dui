//
// Created by cpasjuste on 25/11/16.
//

#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <cstring>

namespace c2dui {

    class C2DUIVideo : public c2d::C2DTexture {

    public:

        C2DUIVideo(C2DUIGuiMain *ui, void **pixels, int *pitch,
                   const c2d::Vector2f &size, int format = C2D_TEXTURE_FMT_RGB565);

        virtual void updateScaling();

    private:

        C2DUIGuiMain *ui = NULL;
    };
}

#endif //_VIDEO_H_
