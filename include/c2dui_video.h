//
// Created by cpasjuste on 25/11/16.
//

#ifndef _VIDEO_H_
#define _VIDEO_H_

//#ifdef __SWITCH__
//#include "c2dui_video_nx.h"
//#else

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

//#endif //  __SWITCH__
#endif //_VIDEO_H_
