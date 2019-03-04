//
// Created by cpasjuste on 03/02/18.
//

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

UIEmu::UIEmu(UIMain *u) : RectangleShape(u->getSize()) {

    printf("UIEmu()\n");

    ui = u;
    setFillColor(Color::Transparent);

    fpsText = new Text("0123456789", (unsigned int) ui->getFontSize(), ui->getSkin()->font);
    fpsText->setString("FPS: 00/60");
    fpsText->setPosition(16, 16);
    fpsText->setVisibility(Visibility::Hidden);
    add(fpsText);

    setVisibility(Visibility::Hidden);
}

void UIEmu::addAudio(c2d::Audio *_audio) {

    if (audio) {
        delete (audio);
        audio = nullptr;
    }

    audio = _audio;
}

void UIEmu::addAudio(int rate, float fps, Audio::C2DAudioCallback cb) {

    if (audio) {
        delete (audio);
        audio = nullptr;
    }

    auto *aud = new C2DAudio(rate, fps, cb);
    addAudio(aud);
}

void UIEmu::addVideo(C2DUIVideo *_video) {

    if (video) {
        delete (video);
        video = nullptr;
    }

    video = _video;
    add(video);
}

void UIEmu::addVideo(UIMain *ui, void **pixels, int *pitch,
                     const c2d::Vector2f &size, Texture::Format format) {

    if (video) {
        delete (video);
        video = nullptr;
    }

    auto *_video = new C2DUIVideo(ui, pixels, pitch, size, format);
    addVideo(_video);
}

int UIEmu::load(RomList::Rom *rom) {

    printf("UIEmu::load(%s)\n", rom->path.c_str());

    // set fps text on top
    getFpsText()->setLayer(2);

    setVisibility(Visibility::Visible);
    ui->getUiProgressBox()->setVisibility(Visibility::Hidden);
    ui->getUiRomList()->setVisibility(Visibility::Hidden);

    resume();

    return 0;
}

void UIEmu::pause() {

    printf("UIEmu::pause()\n");

    if (audio) {
        audio->pause(1);
    }
    // set ui input configuration
    ui->updateInputMapping(false);
    // enable auto repeat delay
    ui->getInput()->setRepeatDelay(INPUT_DELAY);

    paused = true;
}

void UIEmu::resume() {

    printf("UIEmu::resume()\n");

    // set per rom input configuration
    ui->updateInputMapping(true);
    // disable auto repeat delay
    ui->getInput()->setRepeatDelay(0);

    if (audio) {
        audio->pause(0);
    }

    paused = false;
}

void UIEmu::stop() {

    printf("UIEmu::stop()\n");

    if (audio) {
        printf("Closing audio...\n");
        audio->pause(1);
        delete (audio);
        audio = nullptr;
    }

    if (video) {
        printf("Closing video...\n");
        delete (video);
        video = nullptr;
    }

    ui->getUiHighlight()->setVisibility(Visibility::Visible);
    ui->updateInputMapping(false);
    setVisibility(Visibility::Hidden);
}

UIMain *UIEmu::getUi() {
    return ui;
}

C2DUIVideo *UIEmu::getVideo() {
    return video;
}

c2d::Audio *UIEmu::getAudio() {
    return audio;
}

float UIEmu::getFrameDuration() {
    return frameDuration;
}

void UIEmu::setFrameDuration(float f) {
    frameDuration = f;
}

c2d::Text *UIEmu::getFpsText() {
    return fpsText;
}

char *UIEmu::getFpsString() {
    return fpsString;
}

bool UIEmu::isPaused() {
    return paused;
}

