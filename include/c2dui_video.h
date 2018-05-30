//
// Created by cpasjuste on 25/11/16.
//

#ifndef _VIDEO_H_
#define _VIDEO_H_

#ifdef __NX__
#include "video_nx.h"
#else

#include <cstring>

//class C2DUIGuiMain;

namespace c2dui {

    class C2DUIVideo : public c2d::C2DTexture {

    public:

        C2DUIVideo(C2DUIGuiMain *ui, const c2d::Vector2f &size);

        virtual ~C2DUIVideo();

        virtual void updateScaling();

    private:

        C2DUIGuiMain *ui = NULL;
    };
}
#endif
#endif //_VIDEO_H_
