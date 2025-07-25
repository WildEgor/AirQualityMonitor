#include <Arduino.h>

#include "qrcode.h"
#include "qrencode.h"

QRcode::QRcode(TFT_eSPI *tft)
{
    this->_tft = tft;
}

void QRcode::init(int x, int y, int16_t width, int16_t height)
{
    _x = x;
    _y = y;
    _screenwidth = width;
    _screenheight = height;
    _init();
}

void QRcode::init(int x, int y)
{
    _x = x;
    _y = y;
    _screenwidth = _tft->width();
    _screenheight = _tft->height();
    _init();
}

void QRcode::_init()
{
    int min = _screenwidth;

    if (_screenheight < _screenwidth)
        min = _screenheight;

    _multiply = min / WD;

    _offsetsX = (_screenwidth - (WD * _multiply)) / 2 + _x;
    _offsetsY = (_screenheight - (WD * _multiply)) / 2 + _y;
}

void QRcode::_render(int x, int y, int color)
{
    x = (x * _multiply) + _offsetsX;
    y = (y * _multiply) + _offsetsY;
    if (color == 1)
    {
        _tft->drawPixel(x, y, TFT_BLACK);
        if (_multiply > 1)
        {
            _tft->drawPixel(x + 1, y, TFT_BLACK);
            _tft->drawPixel(x + 1, y + 1, TFT_BLACK);
            _tft->drawPixel(x, y + 1, TFT_BLACK);
        }
    }
    else
    {
        _tft->drawPixel(x, y, TFT_WHITE);
        if (_multiply > 1)
        {
            _tft->drawPixel(x + 1, y, TFT_WHITE);
            _tft->drawPixel(x + 1, y + 1, TFT_WHITE);
            _tft->drawPixel(x, y + 1, TFT_WHITE);
        }
    }
}

void QRcode::create(String message, bool invert)
{
    message.toCharArray((char *)strinbuf, 260);

    qrencode();

    // print QR Code
    for (byte x = 0; x < WD; x += 2)
    {
        for (byte y = 0; y < WD; y++)
        {
            if (QRBIT(x, y) && QRBIT((x + 1), y))
            {
                // black square on top of black square
                if (!invert)
                {
                    _render(x, y, 1);
                    _render((x + 1), y, 1);
                }
                else
                {
                    _render(x, y, 0);
                    _render((x + 1), y, 0);
                }
            }
            if (!QRBIT(x, y) && QRBIT((x + 1), y))
            {
                // white square on top of black square
                if (!invert)
                {
                    _render(x, y, 0);
                    _render((x + 1), y, 1);
                }
                else
                {
                    _render(x, y, 1);
                    _render((x + 1), y, 0);
                }
            }
            if (QRBIT(x, y) && !QRBIT((x + 1), y))
            {
                // black square on top of white square
                if (!invert)
                {
                    _render(x, y, 1);
                    _render((x + 1), y, 0);
                }
                else
                {
                    _render(x, y, 0);
                    _render((x + 1), y, 1);
                }
            }
            if (!QRBIT(x, y) && !QRBIT((x + 1), y))
            {
                // white square on top of white square
                if (!invert)
                {
                    _render(x, y, 0);
                    _render((x + 1), y, 0);
                }
                else
                {
                    _render(x, y, 1);
                    _render((x + 1), y, 1);
                }
            }
        }
    }
}