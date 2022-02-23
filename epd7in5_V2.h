/**
 *  @filename   :   epd7in5.h
 *  @brief      :   Header file for e-paper library epd7in5.cpp
 *  @author     :   Yehui from Waveshare
 *  
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef EPD7IN5_V2_H
#define EPD7IN5_V2_H

#include "epdif.h"
#include "bigfont.h"

// Display resolution
#define EPD_WIDTH       800
#define EPD_HEIGHT      480

// EPD4IN2 commands
#define PANEL_SETTING                               0x00
#define POWER_SETTING                               0x01
#define POWER_OFF                                   0x02
#define POWER_OFF_SEQUENCE_SETTING                  0x03
#define POWER_ON                                    0x04
#define POWER_ON_MEASURE                            0x05
#define BOOSTER_SOFT_START                          0x06
#define DEEP_SLEEP                                  0x07
#define DATA_START_TRANSMISSION_1                   0x10
#define DATA_STOP                                   0x11
#define DISPLAY_REFRESH                             0x12
#define DATA_START_TRANSMISSION_2                   0x13
#define LUT_FOR_VCOM                                0x20 
#define LUT_WHITE_TO_WHITE                          0x21
#define LUT_BLACK_TO_WHITE                          0x22
#define LUT_WHITE_TO_BLACK                          0x23
#define LUT_BLACK_TO_BLACK                          0x24
#define PLL_CONTROL                                 0x30
#define TEMPERATURE_SENSOR_COMMAND                  0x40
#define TEMPERATURE_SENSOR_SELECTION                0x41
#define TEMPERATURE_SENSOR_WRITE                    0x42
#define TEMPERATURE_SENSOR_READ                     0x43
#define VCOM_AND_DATA_INTERVAL_SETTING              0x50
#define LOW_POWER_DETECTION                         0x51
#define TCON_SETTING                                0x60
#define RESOLUTION_SETTING                          0x61
#define GSST_SETTING                                0x65
#define GET_STATUS                                  0x71
#define AUTO_MEASUREMENT_VCOM                       0x80
#define READ_VCOM_VALUE                             0x81
#define VCM_DC_SETTING                              0x82
#define PARTIAL_WINDOW                              0x90
#define PARTIAL_IN                                  0x91
#define PARTIAL_OUT                                 0x92
#define PROGRAM_MODE                                0xA0
#define ACTIVE_PROGRAMMING                          0xA1
#define READ_OTP                                    0xA2
#define POWER_SAVING                                0xE3


class Epd : EpdIf {
public:
    Epd();
    ~Epd();
    int  Init(void);
    void WaitUntilIdle(void);
    void Reset(void);
    void SetLut(void);
    void DisplayFrame(const unsigned char* frame_buffer);
    void SendCommand(unsigned char command);
    void SendData(unsigned char data);
    void Sleep(void);
    void Clear(void);
    void Displaypart(const unsigned char* pbuffer, unsigned long Start_X, unsigned long Start_Y,unsigned long END_X,unsigned long END_Y);
    void SetPartialWindow(const unsigned char* frame_buffer, int x, int y, int w, int h);
    int CharToLetterId(const char charToTransform, const BigFont* font);
    int DisplayChar(const int letterId, const int formerLetterId, const BigFont* font, int colored, int x, int y);
    void DisplayText(const char* text, const BigFont* font, int colored, int x, int y);
    int BigStringWidth(const char* text, const BigFont* font);
    void DisplayFrame(void);
    void ClearFrameHidden(void);
    void ClearFrame(void);
 
private:
    unsigned int reset_pin;
    unsigned int dc_pin;
    unsigned int cs_pin;
    unsigned int busy_pin;
    unsigned long width;
    unsigned long height;

	void SetLut(unsigned char *lut);
    void SetLut_by_host(unsigned char *lut_vcom, unsigned char *lut_ww, unsigned char *lut_bw, unsigned char *lut_wb, unsigned char *lut_bb);
};

#endif /* EPD7IN5_H */

/* END OF FILE */
