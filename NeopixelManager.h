#ifndef __NEOS_H__
#define __NEOS_H__
/*
   This is a general purpose file which contains helper functions for dealing with using NeoPixel LEDs
   with the WS2812Serial library
   */
/*
// all these are declared as an array just in case dual-sided lights are used
int16_t remaining_flash_delay[2];           // to keep track of the flash
bool flash_on[2] = {false, false};              // to keep track of the flash

elapsedMillis last_click_flash[2]; // ensures that the LEDs are not turned on when they are in shutdown mode to allow for an accurate LUX reading
elapsedMillis led_shdn_timer[2];
bool leds_on[2] = {false, false};               // set to false if the LEDs are turned off and true if the leds are on...
elapsedMillis led_off_len[2];          // keeps track of how uint32_t the LEDs are turned off, it will reset when colorWipe is called with a color of 0

// to keep track of how often the leds are on/off
elapsedMillis led_switch_timer[2];// each time color wipe is called this value is reset
uint32_t led_on_times[2] = {1, 1};
uint32_t led_off_times[2] = {1, 1};
double led_on_ratio[2];
*/
#include <WS2812Serial.h>
#include "../Configuration.h"
#include "../Macros.h"
#include <PrintUtils.h>

#ifndef P_CLICK
#define P_CLICK 0
#endif

#ifndef P_ON_RATIO 
#define P_ON_RATIO 0
#endif

#ifndef P_LED_ON_RATIO
#define P_LED_ON_RATIO 0
#endif

#ifndef P_COLOR_WIPE
#define P_COLOR_WIPE 0
#endif

#ifndef P_PACK_COLORS
#define P_PACK_COLORS 0
#endif

#ifndef FLASH_DEBOUNCE_TIME
#define FLASH_DEBOUNCE_TIME 50
#endif

#ifndef P_LUX 
#define P_LUX 0
#endif

#ifndef P_EXTREME_LUX
#define P_EXTREME_LUX 0
#endif

#ifndef P_LEDS_ON
#define P_LEDS_ON 0
#endif

#ifndef UPDATE_ON_OFF_RATIOS 
#define UPDATE_ON_OFF_RATIOS 1
#endif

#ifndef LED_MAPPING_STANDARD
#define LED_MAPPING_STANDARD 0
#endif

#ifndef LED_MAPPING_BOTTOM_UP 
#define LED_MAPPING_BOTTOM_UP 1
#endif

#ifndef LED_MAPPING_ROUND
#define LED_MAPPING_ROUND 2
#endif

#ifndef SATURATED_COLORS 
#define SATURATED_COLORS 0
#endif

#ifndef LED_MAPPING_CENTER_OUT
#define LED_MAPPING_CENTER_OUT 3
#endif


// todo add on/off bools for the datalogging shiz
class NeoGroup {
    /* TODO add a class summary
     *
     */
    public:
        NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String _id, uint32_t f_min, uint32_t f_max);
        NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String _id);
        void setFlashBehaviour(bool f){flash_dominates = f;};

        /////////////////////////////// Datatracking /////////////////////////////
        double red_avg, green_avg, blue_avg;
        double getAverageBrightness(bool reset);// can reset the trackers if neededd
        void resetRGBAverageTracker();
        bool getLedsOn() {return leds_on;}; // for externally determining if the LEDs are currently on

        /////////////////////////////// Flashes //////////////////////////////////
        uint32_t getOnOffLen() {return on_off_len;};
        uint32_t getOffLen() {return off_len;};
        uint32_t getShdnTimer() {return shdn_timer;};
        uint32_t getNumFlashes() {return num_flashes;};
        void setFlashOn(bool val) {flash_on = val;};
        bool getFlashOn() {return flash_on;};
        uint32_t num_flashes = 0;// these are public so they can be tracked by the datalog manager
        uint32_t total_flashes = 0;
        double fpm;
        double getFPM();
        void resetFPM();
        void addToRemainingFlashDelay(long i);
        void setRemainingFlashDelay(long d);

        ////////////////////////////// SHDN Timer ///////////////////////////////
        uint32_t getShdnLen();
        bool isInShutdown();
        void updateAvgBrightnessScaler();
        void resetAvgBrightnessScaler();
        void setBrightnessScaler(double scaler) {brightness_scaler = scaler;updateAvgBrightnessScaler();};
        void setMinMaxBrightnessFromBS(double s);
        double getOnRatio() {return on_ratio;};
        double on_ratio = 0.5;

        void setMinBrightnessFromLux(double l);
        void setMaxBrightnessFromLux(double l);

        ////////////////////////// Brightness Scaler //////////////////////////////
        double getBrightnessScaler() {return brightness_scaler;};
        double getAvgBrightnessScaler();
        double getAvgBrightness(String type);
        long getRemainingFlashDelay() {return remaining_flash_delay;};

        void updateUserBrightnessScaler(double b) {
            user_brightness_overide = true;
            user_brightness_scaler = b;};

        void setMinBrightness(uint8_t m){MIN_BRIGHTNESS = m;};
        void setMaxBrightness(uint8_t m){MAX_BRIGHTNESS = m;};

        ///////////////////////////////// HSB Colors //////////////////////////////
        void updateHSB(double h, double s, double b);
        double getHue(){return hsb[0];};
        double getSat(){return hsb[1];};
        double getBright(){return hsb[2];};

        //////////////////////////////// ColorWipes ///////////////////////////////
        void colorWipe(uint8_t red, uint8_t green, uint8_t blue, double brightness, double bs);
        void colorWipe(uint8_t red, uint8_t green, uint8_t blue, double brightness);
        void colorWipeHSB(double h, double s, double b);
        void colorWipeAdd(uint8_t red, uint8_t green, uint8_t blue);
        void colorWipeAdd(uint8_t red, uint8_t green, uint8_t blue, double bs);

        void changeMapping(uint8_t m){mapping = m;};

        //////////////////////////////// Flashes //////////////////////////////////
        bool flashOn(uint8_t red, uint8_t green, uint8_t blue); // perhaps add time for flash to flashOn
        bool flashOn();
        void flashOff();
        void update();

        void setFlashColors(uint8_t red, uint8_t green, uint8_t blue);
        void setSongColors(uint8_t red, uint8_t green, uint8_t blue);

        void powerOn(); // force a power on, overriding any shdn_timer

        /////////////////////////////// Printing /////////////////////////////////
        void printGroupConfigs();
        void printColors();

        ///////////////////////////// Misc //////////////////////////////////////
        bool shutdown(uint32_t len);
        String getName() { return id;};

        void setExtremeLuxShdn(bool e){extreme_lux_shdn = e;};
        bool getLuxShdn() { return extreme_lux_shdn;};

    private:
        bool flash_dominates = false;
        uint8_t mapping = LED_MAPPING_STANDARD;
        double hsb[3]; // limited from 0 - 255
        uint8_t rgb[3]; // limited from 0.0 - 1.0
        double hue2rgb(double p, double q, double t);
        void RgbToHsb(uint8_t red, uint8_t green, uint8_t blue);
        void HsbToRgb(double hue, double saturation, double lightness);stoffregenPaulStoffstoffregenPaulStoffregenWS2812Serial2020regenWS2812Serial2020
        void updateColorLog(uint8_t red, uint8_t green, uint8_t blue);
        bool extreme_lux_shdn = false;
        uint32_t packColors(uint8_t &red, uint8_t &green, uint8_t &blue, double scaler);

        ////////////////////////////// Flashes //////////////////////////////////
        uint8_t flash_red = 0;
        uint8_t flash_green = 0;
        uint8_t flash_blue = 255;
        // max values for the click used for some mappings
        uint8_t song_red = 0;
        uint8_t song_green = 0;
        uint8_t song_blue = 0;

        long remaining_flash_delay = 0;// negative values expected, can not be a variable
        bool flash_on = false;
        long flash_min_time;  // how long is the shortest flash?
        long flash_max_time;  // how about the longest?
        elapsedMillis fpm_timer;

        // a linked neogroup for sharing flash info?

        //////////////////////////// Data Tracking /////////////////////////////
        uint64_t red_tot, green_tot, blue_tot;
        uint64_t red_readings, green_readings, blue_readings;
        // related to auto-calibration and datalogging
        bool update_on_off_ratios = UPDATE_ON_OFF_RATIOS;
        String id;

        //////////////////////////  LED Linking ////////////////////////////////
        WS2812Serial *leds;
        int idx_start;
        int idx_end;
        int num_pixels;
        elapsedMillis shdn_timer; // if this is below a certain threshold then shutdown everything
        uint32_t shdn_len = 0;
        bool leds_on = false;
        elapsedMillis last_flash; // the last time a flash message was received
        elapsedMillis on_off_len; // this is reset every time the leds shutdown or turn on (length of time on or off)
        elapsedMillis on_len;
        elapsedMillis off_len;
        elapsedMillis last_flash_update;

        //////////////////////// On Ratio /////////////////////////////////////
        uint64_t on_time = 1;
        uint64_t off_time = 1;
        uint32_t last_on_ratio_update = 0;
        void updateOnRatio(int color);

        /////////////////////// Brightness Scaler ////////////////////////////
        double brightness_scaler = 1.0;
        double brightness_scaler_total;
        double brightness_scaler_changes;
        void   updateBrightnessScalerTotals();
        void   resetOnOffRatioCounters();

        uint16_t MIN_BRIGHTNESS = 0;
        uint16_t MAX_BRIGHTNESS = 765;

        /////////////////////// User Brightness Scaler //////////////////////
        double user_brightness_overide = true;
        double user_brightness_scaler = 1.0;

};

uint32_t NeoGroup::packColors(uint8_t &red, uint8_t &green, uint8_t &blue, double scaler) {
    /*
     * TODO write a function summary
     * 
     * */
    uint32_t color = 0;
    dprintMinorDivide(P_PACK_COLORS);
    dprint(P_PACK_COLORS, "Entering PackColors(): ");
    dprint(P_PACK_COLORS, "pre-bs  r: ");
    dprint(P_PACK_COLORS, red);
    dprint(P_PACK_COLORS, "\tg: ");
    dprint(P_PACK_COLORS, green); 
    dprint(P_PACK_COLORS, "\tb: ");
    dprint(P_PACK_COLORS, blue); 

    red = red * scaler;
    green = green * scaler;
    blue = blue * scaler;

    // red = min(red, MAX_BRIGHTNESS);
    // green = min(green, MAX_BRIGHTNESS);
    // blue = min(blue, MAX_BRIGHTNESS);

    if (SATURATED_COLORS) {
        if (red >= green && red >= blue) {
            red += (green/2 + blue/2);
            green /= 2;
            blue /= 2;
        }
        else if (green >= red && green >= blue) {
            green += (red/2 + blue/2);
            red /= 2;
            blue /= 2;
        }
        else if (blue >= red && blue >= green) {
            blue += (red/2 + green /2);
            green /= 2;
            blue /= 2;
        }
    }

    if (red + green + blue > MAX_BRIGHTNESS){
        uint16_t total = red + green + blue;
        dprint(P_PACK_COLORS + P_COLOR_WIPE, "Too Bright, ");
                dprint(P_PACK_COLORS + P_COLOR_WIPE, total);
                dprint(P_PACK_COLORS + P_COLOR_WIPE, "reducing the brightness of each color: ");
                double factor = total / MAX_BRIGHTNESS;
                red = red / factor;
                green = green / factor;
                blue = blue / factor;
                dprint(P_PACK_COLORS, "\tr: ");
                dprint(P_PACK_COLORS, red);
                dprint(P_PACK_COLORS, "\tg: ");
                dprint(P_PACK_COLORS, green); 
                dprint(P_PACK_COLORS, "\tb: ");
                dprint(P_PACK_COLORS, blue); 
                }

                // if (red < MIN_BRIGHTNESS) {red = MIN_BRIGHTNESS;};
                // if (green < MIN_BRIGHTNESS) {green = MIN_BRIGHTNESS;};
                // if (blue < MIN_BRIGHTNESS) {blue = MIN_BRIGHTNESS;};

                color = (red << 16) + (green << 8) + (blue);

                dprint(P_PACK_COLORS, " || final r: ");
                dprint(P_PACK_COLORS, red);
                dprint(P_PACK_COLORS, "\tg: ");
                dprint(P_PACK_COLORS, green); 
                dprint(P_PACK_COLORS, "\tb ");
                dprintln(P_PACK_COLORS, blue); 
                return color;
}

double NeoGroup::getAverageBrightness(bool reset) {
    double val = (blue_avg + red_avg + green_avg)/3;
    if (reset) {resetRGBAverageTracker();};
    return val;
}

void NeoGroup::setMinMaxBrightnessFromBS(double s){
    // assume ea number between 0.0 and 3.0
    if (s > 3.0) {
        s = 3.0;
    } else if (s < 0.0) {
        s = 0.0;
    }
    if  (s >= 1.0) {
        MAX_BRIGHTNESS = 765;
        MIN_BRIGHTNESS = (s - 1.0) * 30.0;
    } else {
        MAX_BRIGHTNESS = (uint16_t)(s * 765.0);
        MIN_BRIGHTNESS = 0;
    }
    dprint(P_BRIGHTNESS_SCALER, "MIN/MAX BRIGHTNESS updated due to lux readings: ");
    dprint(P_BRIGHTNESS_SCALER, MIN_BRIGHTNESS);
    dprint(P_BRIGHTNESS_SCALER, " / ");
    dprintln(P_BRIGHTNESS_SCALER, MAX_BRIGHTNESS);
}

void NeoGroup::resetFPM() {
    num_flashes = 0;
    fpm_timer = 0;
}

double NeoGroup::getFPM() {
    fpm = (double)num_flashes / (double)fpm_timer * 1000 * 60;
    return fpm;
}

NeoGroup::NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String _id) {
    // todo
    num_pixels = end_idx - start_idx + 1;
    idx_start = start_idx;
    idx_end = end_idx;
    leds = neos;
    id = _id;
}

NeoGroup::NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String _id, uint32_t f_min, uint32_t f_max) {
    // todo
    flash_min_time = f_min;
    flash_max_time = f_max;
    num_pixels = end_idx - start_idx + 1;
    idx_start = start_idx;
    idx_end = end_idx;
    leds = neos;
    id = _id;
}

bool NeoGroup::shutdown(uint32_t len) {
    // return 0 if lux shutdown not a success, 1 if it is
    if (!isInShutdown()) {
        dprint(P_LUX, millis());dprint(P_LUX, "\tSHUTTING DOWN GROUP ");
        dprint(P_LUX, id);
        dprint(P_LUX, " for a total of at least ");
        dprint(P_LUX, len);
        dprint(P_LUX," ms");
        shdn_len = len;
        colorWipe(0, 0, 0, 0.0);
        shdn_timer = 0;
        if (leds_on != false) {
            leds_on = false;
            dprint(P_LEDS_ON, "\nsetting leds_on to false");
        }
        dprintln(P_LEDS_ON);
        return 0;
    }
    return 1;
}

void NeoGroup::printColors() {
    Serial.print(id);
    Serial.print("  red  "); Serial.print(rgb[0]);
    Serial.print("\tgreen  "); Serial.print(rgb[1]);
    Serial.print("\tblue  ");
    Serial.print(rgb[2]);
    Serial.print("  =  "); Serial.print(rgb[0] + rgb[1] + rgb[0]);
    Serial.print("\thue "); Serial.print(hsb[0]);
    Serial.print("\tsat "); Serial.print(hsb[1]);
    Serial.print("\tbgt "); Serial.println(hsb[2]);
}

void NeoGroup::powerOn() {
    dprintln(P_LUX, "POWER ON MESSAGE RECEIVED");
    shdn_timer += shdn_len;
}

uint32_t NeoGroup::getShdnLen() {
    if (shdn_timer <= shdn_len) {
        return shdn_len - shdn_timer;
    } else  {
        return 0;
    }
}

void NeoGroup::setFlashColors(uint8_t red, uint8_t green, uint8_t blue) {
    flash_red = red;
    flash_green = green;
    flash_blue = blue;
}

void NeoGroup::setSongColors(uint8_t red, uint8_t green, uint8_t blue) {
    song_red = red;
    song_green = green;
    song_blue = blue;
}

void NeoGroup::updateHSB(double h, double s, double b) {
    hsb[0] = h;
    hsb[1] = s;
    hsb[2] = b;
}

double NeoGroup::hue2rgb(double p, double q, double t)
{
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1 / 6.0) return p + (q - p) * 6 * t;
    if (t < 1 / 2.0) return q;
    if (t < 2 / 3.0) return p + (q - p) * (2 / 3.0 - t) * 6;
    return p;
}

void NeoGroup::HsbToRgb(double hue, double saturation, double lightness)
{
    double r, g, b = 0.0;

    if (saturation == 0)
    {
        r = g = b = lightness; // achromatic
    }
    else
    {
        auto q = lightness < 0.5 ? lightness * (1 + saturation) : lightness + saturation - lightness * saturation;
        auto p = 2 * lightness - q;
        r = hue2rgb(p, q, hue + 1 / 3.0);
        g = hue2rgb(p, q, hue);
        b = hue2rgb(p, q, hue - 1 / 3.0);
    }


    uint8_t r8  = static_cast<uint8_t>(r * 255);
    uint8_t g8  = static_cast<uint8_t>(g * 255);
    uint8_t b8  = static_cast<uint8_t>(b * 255);
    updateColorLog(r8, g8, b8); // this is used to update the rgb[] array
}

void NeoGroup::RgbToHsb(uint8_t red, uint8_t green, uint8_t blue)
{
    /***************************************************
      Copyright (c) 2017 Luis Llamas
      (www.luisllamas.es)

      Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

      Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
     ****************************************************/
    auto rd = static_cast<double>(red) / 255;
    auto gd = static_cast<double>(green) / 255;
    auto bd = static_cast<double>(blue) / 255;
    auto ma = max(rd, max(gd, bd));
    auto mi = min(rd, min(gd, bd));
    hsb[2]= ma;

    auto d = ma - mi;
    hsb[1] = ma == 0 ? 0 : d / ma;

    hsb[0] = 0;
    if (ma != mi)
    {
        if (ma == rd)
        {
            hsb[0] = (gd - bd) / d + (gd < bd ? 6 : 0);
        }
        else if (ma == gd)
        {
            hsb[0] = (bd - rd) / d + 2;
        }
        else if (ma == bd)
        {
            hsb[0] = (rd - gd) / d + 4;
        }
        hsb[0] /= 6;
    }
}

void NeoGroup::resetRGBAverageTracker() {
    red_tot = 0;
    green_tot = 0;
    blue_tot = 0;
    red_readings = 0;
    green_readings = 0;
    blue_readings = 0;
}

void NeoGroup::colorWipeHSB(double h, double s, double b) {
    dprint(P_COLOR_WIPE, "Entering colorWipeHSB() : ");
    updateHSB(h, s, b);
    HsbToRgb(h, s, b);
    // TODO calculate brightness properly
    colorWipe(rgb[0], rgb[1], rgb[2], b);
}

void NeoGroup::colorWipeAdd(uint8_t red, uint8_t green, uint8_t blue, double bs) {
    red += rgb[0];
    red = constrain(red, 0, 255);
    green += rgb[1];
    green = constrain(green, 0 , 255);
    blue += rgb[2];
    blue = constrain(blue, 0, 255);
    colorWipe(red, green, blue, bs);
}


void NeoGroup::colorWipeAdd(uint8_t red, uint8_t green, uint8_t blue) {
    red += rgb[0];
    red = constrain(red, 0, 255);
    green += rgb[1];
    green = constrain(green, 0 , 255);
    blue += rgb[2];
    blue = constrain(blue, 0, 255);
    colorWipe(red, green, blue, brightness_scaler);
}

void NeoGroup::colorWipe(uint8_t red, uint8_t green, uint8_t blue, double brightness) {
    colorWipe(red, green, blue, brightness, brightness_scaler);
}

void NeoGroup::updateColorLog(uint8_t red, uint8_t green, uint8_t blue) {

    // for each ms this value has been present add the value of the last color
    //  to the running total
    //  WARNING - should not loop through on_off_len
    // for (uint16_t i = 0; i < on_off_len; i++) {
    red_tot += rgb[0];
    green_tot += rgb[1];
    blue_tot += rgb[2];
    red_readings++;
    green_readings++;
    blue_readings++;
    //}
    // then update the array which tracks the currently displayed color
    rgb[0] = red;
    rgb[1] = green;
    rgb[2] = blue;

    // update the average (perhaps should only do this every second or so?
    if (millis() + 1000 > last_on_ratio_update){
        red_avg = red_tot / (double)red_readings;
        green_avg = green_tot / (double)green_readings;
        blue_avg = blue_tot / (double)blue_readings;
        if (update_on_off_ratios) {
            updateOnRatio(red + green + blue);
        }
        last_on_ratio_update = millis();
    }
}

void NeoGroup::colorWipe(uint8_t red, uint8_t green, uint8_t blue, double brightness, double bs) {
    // TODO this logic is broken...
    // need a debug rating of at least 2 to print these
    dprintMinorDivide(P_COLOR_WIPE);
    dprint(P_COLOR_WIPE, "Entering ColorWipe() in NeoGroup - ");
    dprint(P_COLOR_WIPE, id);
    dprint(P_COLOR_WIPE, " - num_pixels: ");
    dprintln(P_COLOR_WIPE, num_pixels); 
    dprint(P_COLOR_WIPE, "rgb: ");
    dprint(P_COLOR_WIPE, red);
    dprint(P_COLOR_WIPE, "\t");
    dprint(P_COLOR_WIPE, green);
    dprint(P_COLOR_WIPE, "\t");
    dprintln(P_COLOR_WIPE, blue);

    if (extreme_lux_shdn == true) {
        dprintln(P_EXTREME_LUX, " colorWipe returning due extreme lux conditions");
        for (int i = 0; i < num_pixels; i++) {
            leds->setPixel(idx_start + i, 0);
        }
        leds_on = false;
        updateColorLog(0, 0, 0);
        return;
    }

    if (shdn_timer < shdn_len) {
        // if the LEDs are in shutdown mode than simply exit without changing the LEDs
        for (int i = 0; i < num_pixels; i++) {
            leds->setPixel(idx_start + i, 0);
        }
        leds_on = false;
        dprint(P_COLOR_WIPE, " colorWipe returning due to shdn_timer : "); 
        dprintln(P_COLOR_WIPE, shdn_timer);
        updateColorLog(0, 0, 0);
        return;
    }

    /////////////////////////////////////////////////////////////////////////////
    if (flash_on == true) {
        // if the flash is on then add the flash colors to the color wipe colors
        if (flash_dominates == false) {
            dprintln(P_COLOR_WIPE, " Flash blocked colorWipe");
            red += flash_red;
            green += flash_green;
            blue += flash_blue;
        }
        else {
            dprintln(P_COLOR_WIPE, " Flash blocked colorWipe");
            red += flash_red;
            green += flash_green;
            blue += flash_blue;
        }
    }
    //////////////////////////////////////////////////////////////////////////////
    // packColors will take the red, green and blue values (0 - 255) and
    // scale them according the brightness scaler
    if (user_brightness_overide == true) {
        bs = bs * user_brightness_scaler;
    }
    int colors = packColors(red, green, blue, bs);

    //////////////////////////////////////////////////////////////////////////////
    if (mapping == LED_MAPPING_STANDARD) {
        for (int i = 0; i < num_pixels; i++) {
            leds->setPixel(idx_start + i, colors);
            // dprint(P_COLOR_WIPE, idx_start+i);
            // dprint(P_COLOR_WIPE, ": ");
            // dprint(P_COLOR_WIPE, colors); 
            // dprint(P_COLOR_WIPE, "\t");
        }
        // dprintln(P_COLOR_WIPE);
    } else if (mapping == LED_MAPPING_ROUND) {
        // TODO this logic is broken for when a flash is happening
        red = red * num_pixels;
        green = green * num_pixels;
        blue = blue * num_pixels;
        for (int i = 0; i < num_pixels; i++) {
            // if we have more than the max then just add the max to the target
            uint8_t _red = 0;
            uint8_t _green = 0;
            uint8_t _blue = 0;
            if (red > song_red) {
                _red = song_red;
            } else {
                // otherwise use up what is left
                _green = green;
            }
            if (green > song_green) {
                _green = song_green;
            } else {
                // otherwise use up what is left
                _green = red;
            }
            if (blue > song_blue) {
                _blue = song_blue;
            } else {
                // otherwise use up what is left
                _blue = blue;
            }
            red = red - _red;
            green = green - _green;
            blue = blue - _blue;
            int _colors = packColors(_red, _green, _blue, brightness_scaler);
            leds->setPixel(idx_start + i, _colors);
        }
    }
    else if (mapping == LED_MAPPING_CENTER_OUT) {
        // center ring front: 16, 17, 18, 19
        // center ring rear: 36, 37, 38, 39
        // middle ring front: 10, 11, 12, 13, 14, 15
        // middle ring rear: 30, 31, 32, 33, 34, 35
        // outer ring front 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
        // outer ring rear: 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
        uint16_t center_rings[8] = {16, 17, 18, 19, 36, 37, 38, 39};
        uint16_t middle_rings[12] = {10, 11, 12, 13, 14, 15, 30, 31, 32, 33, 34, 35};
        uint16_t outer_rings[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
        double center_ring_weight = 0.2;
        double middle_ring_weight = 0.3;
        // double outer_ring_weight = 0.5;
        // loop for the center rings
        dprint(P_COLOR_WIPE, "center out brightness: ");
        dprintln(P_COLOR_WIPE, brightness);

        ///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////// CENTER RING //////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        // temp rgb values for if we need to present a scaledd display for the center ring
        uint8_t _red = brightness * 5 * red;
        uint8_t _green= brightness * 5 * green;
        uint8_t _blue = brightness * 5 * blue;
        for (int i = 0; i < 8; i++) {
            // if the brightness is greater than 51 then all the LEDS should be 
            // at max brightness for the center rings
            if (brightness > center_ring_weight) {
                leds->setPixel(center_rings[i], red, green, blue);
            } else {
                leds->setPixel(center_rings[i], _red, _green , _blue);
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////// MIDDLE RING //////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        // temp rgb values for if we need to present a scaledd display for the center ring
        _red = (brightness - center_ring_weight) * 3.3333333333 * red;
        _green= (brightness - center_ring_weight) * 3.3333333333 * green;
        _blue =  (brightness - center_ring_weight) * 3.3333333333 * blue;
        for (int i = 0; i < 12; i++) {
            // if the brightness is greater than 51 then all the LEDS should be 
            // at max brightness for the center rings
            if (brightness > center_ring_weight) {
                leds->setPixel(middle_rings[i], red, green, blue);
            } else {
                leds->setPixel(middle_rings[i], _red, _green , _blue);
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////// OUTER RING //////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        // temp rgb values for if we need to present a scaledd display for the center ring
        _red = (brightness - center_ring_weight - middle_ring_weight) * 2 * red;
        _green= (brightness - center_ring_weight - middle_ring_weight) * 2 * green;
        _blue =  (brightness - center_ring_weight - middle_ring_weight) * 2 * blue;
        for (int i = 0; i < 20; i++) {
            // if the brightness is greater than 51 then all the LEDS should be 
            // at max brightness for the center rings
            if (brightness > 254) {
                leds->setPixel(outer_rings[i], red, green, blue);
            } else {
                leds->setPixel(outer_rings[i], _red, _green , _blue);
            }
        }
        dprintln(P_COLOR_WIPE);
    }
    else if (mapping == LED_MAPPING_BOTTOM_UP) {
        // groups are pixe
        // g3      4   3         8
        // g2     5     2     7     9
        // g1        1         6  10
        int g1[3] = {1, 6, 10};
        uint8_t g1_colors[3];
        int g2[4] = {5, 2, 7, 9};
        uint8_t g2_colors[3];
        int g3[3] = {4, 3, 8};
        uint8_t g3_colors[3];
        // add all the energy for each colour together
        red = red * num_pixels;
        green = green * num_pixels;
        blue = blue * num_pixels;
        ////////////////////// Red //////////////////////////
        // then start with g1 by filling them up to their max
        if (red > song_red * 3) {
            g1_colors[0] = song_red;
            red = red - (3 * song_red);
        } else {
            g1_colors[0] = red / 3;
            red = 0;
        }
        // next move on to g2
        if (red > song_red * 4) {
            g2_colors[0] = song_red;
            red = red - (4 * song_red);
        } else {
            g2_colors[0] = red / 4;
            red = 0;
        }
        // and then finally g3 
        if (red > song_red * 3) {
            g3_colors[0] = song_red;
            red = red - (3 * song_red);
        } else {
            g3_colors[0] = red / 3;
            red = 0;
        }
        //////////////////////// Green ///////////////////////
        // then start with g1 by filling them up to their max
        if (green > song_green * 3) {
            g1_colors[1] = song_green ;
            green = green - (3 * song_green);
        } else {
            g1_colors[1] = green / 3;
            green = 0;
        }
        // next move on to g2
        if (green > song_green * 4) {
            g2_colors[1] = song_green;
            green = green - (4 * song_green);
        } else {
            g2_colors[1] = green / 4;
            green = 0;
        }
        // and then finally g3 
        if (green > song_green * 3) {
            g3_colors[1] = song_green;
            green = green - (3 * song_green);
        } else {
            g3_colors[1] = green / 3;
            green = 0;
        }

        ////////////////////// Blue //////////////////////////
        // then start with g1 by filling them up to their max
        if (blue > song_blue* 3) {
            g1_colors[0] = song_blue;
            blue = blue - (3 * song_blue);
        } else {
            g1_colors[0] = blue / 3;
            blue = 0;
        }
        // next move on to g2
        if (blue > song_blue * 4) {
            g2_colors[0] = song_blue;
            blue = blue - (4 * song_blue);
        } else {
            g2_colors[0] = blue / 4;
            blue = 0;
        }
        // and then finally g3 
        if (blue > song_blue * 3) {
            g3_colors[0] = song_blue;
            blue = blue - (3 * song_blue);
        } else {
            g3_colors[0] = blue / 3;
            blue = 0;
        }
        // now that we know what the colors should be for each group, we can now set the LEDs to that color
        for (int i = 0; i < 3; i++){
            leds->setPixel(g1[i], g1_colors[0], g1_colors[1], g1_colors[2]);
            leds->setPixel(g2[i], g2_colors[0], g2_colors[1], g2_colors[2]);
            leds->setPixel(g3[i], g3_colors[0], g3_colors[1], g3_colors[2]);
        }
        // g2 has an extra pixel =P
        leds->setPixel(g2[3], g2_colors[0], g2_colors[1], g2_colors[2]);
    }
    leds->show();
    // if the LEDs are on set "led_on" to true, otherwise turn "led_on" to false
    // also reset led_off_len if the leds  were just turned off
    if (colors == 0) {
        if (leds_on != false) {
            leds_on = false;
            dprintln(P_LEDS_ON, " setting leds_on to false");
        }
    } else {
        if (leds_on != true) {
            leds_on = true;
            dprintln(P_LEDS_ON, " setting leds_on to true");
        }
    }
    updateColorLog(red, green, blue);
    // dprintln(P_COLOR_WIPE, " finished updating the neopixels");
}

////////////// General Purpose Helper Functions /////////////////////////
void NeoGroup::flashOff() {
    // if the flash is allowed to be turned off
    if (remaining_flash_delay <= 0) {
        dprint(P_CLICK, id);
        dprint(P_CLICK, " FlashOff : ");
        dprintln(P_CLICK, last_flash);
        flash_on = false;
        if (leds_on != false) {
            dprintln(P_LEDS_ON, "leds_on set to false");
            leds_on = false;
        }
        colorWipe(0, 0, 0, 0.0);
        remaining_flash_delay = 0;
        // last_flash = 0;
    }
}

bool NeoGroup::flashOn(uint8_t red, uint8_t green, uint8_t blue) {
    // if it has been uint32_t enough since the last flash occured
    if (last_flash > FLASH_DEBOUNCE_TIME) {
        if (red + green + blue > 0 && shdn_timer > shdn_len) {
            // if a flash is not currently on
            if ( (flash_on == false) || (leds_on == false) ) {
                remaining_flash_delay = flash_min_time;
                colorWipe(red, green, blue, brightness_scaler * 1.5); // has to be on first as flash_on will block the colorWipe
                flash_on = true; // turn the light on along with the flag
                if (leds_on != true) {
                    dprintln(P_LEDS_ON, "leds_on set to true");
                    leds_on = true;
                }
                last_flash = 0; // reset the elapsed millis variable as the light was just turned on
                num_flashes = num_flashes  + 1;
                total_flashes++;
                getFPM();
                dprint(P_CLICK, id);
                dprint(P_CLICK, " FLASH ON #");
                dprint(P_CLICK, num_flashes);
                dprint(P_CLICK, " Flashed "); dprint(P_CLICK, remaining_flash_delay);
                dprint(P_CLICK, " FPM "); dprintln(P_CLICK, fpm);
            } else { // if a flash is on then increment the remaining_flash_Delay
                addToRemainingFlashDelay(1);
                if (remaining_flash_delay > flash_max_time) {
                    remaining_flash_delay = flash_max_time;
                }
            }
            return true;
        }
    } else {
        dprint(P_CLICK, "Flash skipped due to FLASH_DEBOUNCE_TIME : ");
        dprintln(P_CLICK, last_flash);
    }
    return false;
}

bool NeoGroup::flashOn() {
    return flashOn(flash_red, flash_green, flash_blue);
}

//////////////////////////////////////////////////////
////////////// Update Methods/////////////////////////
//////////////////////////////////////////////////////
void NeoGroup::update() {
    // if there is time remaining in the flash it either needs to be turned
    // on or the timer needs to increment
    if (remaining_flash_delay > 0) {
        dprintMinorDivide(P_CLICK);
        dprint(P_CLICK, "flash delay "); dprint(P_CLICK, id); dprint(P_CLICK, " : ");
        dprint(P_CLICK, remaining_flash_delay); dprintTab(P_CLICK);
        dprint(P_CLICK, last_flash_update); dprintTab(P_CLICK);
        // if the flash is not currently on, turn the flash on
        if (flash_on < 1) { //and the light is not currently on
            dprintln(P_CLICK, "-- Turning the Flash ON --");
            flashOn(flash_red, flash_green, flash_blue);// flash on
        }
        // if the flash is already on subtract from the timer
        else {
            dprintln(P_CLICK, "- - - - - - - - - - - - - - - - - - -");
            dprint(P_CLICK, "last_flash :\t"); dprintln(P_CLICK, last_flash);
            dprint(P_CLICK, "remaining_flash_delay "); 
            dprint(P_CLICK, id); dprint(P_CLICK, ":\t");
            dprint(P_CLICK, remaining_flash_delay); dprint(P_CLICK, "\t");
            remaining_flash_delay = remaining_flash_delay - last_flash_update;
            remaining_flash_delay = max(remaining_flash_delay, 0);
            dprintln(P_CLICK, remaining_flash_delay);
            if (remaining_flash_delay == 0) {
                dprint(P_CLICK, "Click time over, turning off flash "); dprintln(P_CLICK, id);
                flashOff(); // turn off the NeoPixels
            }
        }
    }
    // this is so the timer does not get reset 
    // if it has been running for less than one ms
    if (last_flash_update != 0) {
        last_flash_update = 0;
        dprintln(P_CLICK, "updated last_flash_upate to 0");
    }
}

void NeoGroup::updateAvgBrightnessScaler() {
    brightness_scaler_total += brightness_scaler;
    brightness_scaler_changes++;
}

double NeoGroup::getAvgBrightnessScaler() {
    return brightness_scaler_total / brightness_scaler_changes;
}

void NeoGroup::resetAvgBrightnessScaler() {
    brightness_scaler_total = 0;
    brightness_scaler_changes = 0;
}

void NeoGroup::resetOnOffRatioCounters() {
    on_ratio = (double)on_time / (double)(on_time + off_time);
    on_time = 0;
    off_time = 0;
    dprintln(P_ON_RATIO, "reset the led on/off ratio counters");
}

// mode 0 is just front, mode 1 is just rear, mode 2 is both (using combined values?), mode 3 is both using independent values
void NeoGroup::updateOnRatio(int color) {
    // when color wipe is called it should take care of this for  us
    // to keep track of on/off times
    bool update = false;
    if (color > 0 && !isInShutdown()) {
        on_time += on_off_len;
        off_len = 0;
        update = true;
    } else if (color <= 0){
        off_time += on_off_len;
        on_len = 0;
        update = true;
    }

    if (on_time > 0 && off_time > 0 && update) {
        on_ratio = (double)on_time / (double)(on_time + off_time);
        on_off_len = 0;
    }

    dprint(P_LED_ON_RATIO, "updated led on/off ratio "); dprint(P_LED_ON_RATIO, " :\t");
    dprint(P_LED_ON_RATIO, on_ratio); dprint(P_LED_ON_RATIO, "\t=\t"); dprint(P_LED_ON_RATIO, on_time);
    dprint(P_LED_ON_RATIO, "\t"); dprintln(P_LED_ON_RATIO, off_time);
}

void NeoGroup::addToRemainingFlashDelay(long i) {
    remaining_flash_delay += i;
    if (remaining_flash_delay > flash_max_time) {
        remaining_flash_delay = flash_max_time;
    }
}

void NeoGroup::setRemainingFlashDelay(long d) {
    remaining_flash_delay = d;
}

bool NeoGroup::isInShutdown() {
    if (shdn_timer < shdn_len) {
        return true;
    }
    return false;
}

#endif // __LEDS_H__
