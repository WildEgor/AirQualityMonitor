#pragma once
#include <TFT_eSPI.h>

class QRcode
{
public:
    QRcode(TFT_eSPI *display);
    void init(int x, int y);
    void init(int x, int y, int16_t width, int16_t height);
    void create(String message, bool invert);

private:
    TFT_eSPI *_tft;

    int _x;
    int _y;
    int _offsetsX;
    int _offsetsY;
    int _screenwidth;
    int _screenheight;
    int _multiply = 2;

    void _init();
    void _render(int x, int y, int color);
};