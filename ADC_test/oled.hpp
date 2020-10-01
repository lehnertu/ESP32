/*
 * Ulf Lehnert 9/2020
 * This is a wrapper for the Heltec OLED library.
 * 
 *   - provides some convenience by fixing parameters as does heltec.h
 *   - provides some additional helper methods
 */

#include "heltec.h"

class HeltecOLED : public SSD1306Wire
{
  public :

    // default constructor just calls the Heltec constructor - no additional data
    HeltecOLED() : SSD1306Wire(0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64) {};

    // clear a rectangle on the display (e.g. one text line)
    void clearRect (int16_t x, int16_t y, int16_t width, int16_t height)
    {
        OLEDDISPLAY_COLOR old_color = getColor();
        setColor(BLACK);
        fillRect (x, y, width, height);
        setColor(old_color);
    }

    // get the pixel height of the current font
    uint8_t fontHeight()
    {
        return pgm_read_byte(fontData + HEIGHT_POS);
    }
    
    // clear one text line (i.e. before printing a new string)
    void clearLine(int16_t x, int16_t y)
    {
        clearRect(x,y,width()-x,fontHeight());
    }
};
