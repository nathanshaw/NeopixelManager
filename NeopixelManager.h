#ifndef __NEOS_H__
#define __NEOS_H__
/*
   This is a general purpose file which contains helper functions for dealing with using NeoPixel LEDs
   with the WS2812Serial library
   */
/*
// all these are declared as an array just in case dual-sided lights are used
int16_t remaining_onset_delay[2];           // to keep track of the onset
bool onset_on[2] = {false, false};              // to keep track of the onset

elapsedMillis last_click_onset[2]; // ensures that the LEDs are not turned on when they are in shutdown mode to allow for an accurate LUX reading
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
#include <PrintUtils.h>
#include <ValueTrackerDouble.h>

#ifndef UPDATE_ON_OFF_RATIOS
#define UPDATE_ON_OFF_RATIOS    1
#endif

#ifndef LED_MAPPING_STANDARD
#define LED_MAPPING_STANDARD   0
#endif

#ifndef LED_MAPPING_BOTTOM_UP
#define LED_MAPPING_BOTTOM_UP  3
#endif

#ifndef LED_MAPPING_ROUND
#define LED_MAPPING_ROUND      2
#endif

#ifndef LED_MAPPING_CENTER_OUT
#define LED_MAPPING_CENTER_OUT 1
#endif

#ifndef LED_MAPPING_CLOCK_HAND
#define LED_MAPPING_CLOCK_HAND 4
#endif

#ifndef LED_MAPPING_CLOCK_FILL
#define LED_MAPPING_CLOCK_FILL 6
#endif

#ifndef LED_MAPPING_OUTSIDE_IN
#define LED_MAPPING_OUTSIDE_IN 5
#endif

#ifndef LED_MAPPING_CUSTOM
#define LED_MAPPING_CUSTOM     7
#endif


// todo add on/off bools for the datalogging shiz
class NeoGroup {
    /* TODO add a class summary
     *
     */
    public:
        NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String _id, uint32_t f_min, uint32_t f_max);
        NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String _id);

        void init();
        void begin(){init();};
        void setPixel(uint16_t num,uint16_t red, uint16_t green, uint16_t blue, double brightness );

        /////////////////////////////// Datatracking /////////////////////////////
        double red_avg, green_avg, blue_avg;
        double getAverageBrightness(bool reset);// can reset the trackers if neededd
        void resetRGBAverageTracker();
        bool getLedsOn() {return leds_on;}; // for externally determining if the LEDs are currently on
        void update();

        /////////////////////////////// Flashes //////////////////////////////////
        bool flashOn(uint16_t red, uint16_t green, uint16_t blue); // perhaps add time for flash to flashOn
        bool flashOn();
        void flashOff();
        void setOnsetColors(uint16_t red, uint16_t green, uint16_t blue);
        void setSongColors(uint16_t red, uint16_t green, uint16_t blue);
        void powerOn(); // force a power on, overriding any shdn_timer
        unsigned long getOnOffLen() {return on_off_len;};
        unsigned long getOffLen() {return off_len;};
        unsigned long getShdnTimer() {return shdn_timer;};
        unsigned long getNumFlashes() {return num_flashes;};
        void setFlashOn(bool val) {flash_on = val;};
        bool getFlashOn() {return flash_on;};
        double getFPM();
        void resetFPM();

        void addToRemainingFlashDelay(long i);
        void setRemainingFlashDelay(long d);

        void setFlashBehaviour(bool f){flash_dominates = f;};

        void configureCustomMapping(uint16_t _display_order[], uint16_t l);

        ////////////////////////////// SHDN Timer ///////////////////////////////
        unsigned long getShdnLen();
        bool isInShutdown();
        void resetAvgBrightnessScaler();
        void setLuxBS(double scaler) {lux_bs = scaler;lux_bs_tracker.update();};

        void setMinMaxBrightnessFromBS(double s);
        double getOnRatio() {return on_ratio;};
        void resetOnRatio();
        double on_ratio = 0.5;

        void setMinBrightnessFromLux(double l);
        void setMaxBrightnessFromLux(double l);

        ////////////////////////// Brightness Scaler //////////////////////////////
        double getLuxBS() {return lux_bs;};

        double getAvgBrightnessScaler();
        double getAvgBrightness(String type);
        long getRemainingFlashDelay() {return remaining_flash_delay;};

        void updateUserBrightnessScaler(double b) {
            user_brightness_overide = true;
            user_bs = b;};

        void setMinBrightness(uint16_t m){MIN_BRIGHTNESS = m;};
        void setMaxBrightness(uint16_t m){MAX_BRIGHTNESS = m;};

        ///////////////////////////////// HSB Colors //////////////////////////////
        void updateHSB(double h, double s, double b);
        double getHue(){return hsb[0];};
        double getSat(){return hsb[1];};
        double getBright(){return hsb[2];};

        void setSaturatedColors(bool s){saturated_colors = s;};

        //////////////////////////////// ColorWipe /////////////////////////////////
        void colorWipe(uint16_t red, uint16_t green, uint16_t blue, double brightness, double bs);
        void colorWipe(uint16_t red, uint16_t green, uint16_t blue, double brightness);
        void colorWipeHSB(double h, double s, double b);
        void colorWipeAdd(uint16_t red, uint16_t green, uint16_t blue);
        void colorWipeAdd(uint16_t red, uint16_t green, uint16_t blue, double bs);

        void changeMapping(uint8_t m){mapping = m;};

        /////////////////////////////// Printing /////////////////////////////////
        void printGroupConfigs();
        void printColors();
        void setPrintOnset(bool s){p_onset = s;};
        void setPrintOnRatio(bool s){p_on_ratio = s;};
        void setPrintColorWipe(bool s){p_color_wipe = s;};
        void setPrintLux(bool s){p_lux = s;};
        void setPrintExtremeLux(bool s){p_extreme_lux = s;};
        void setPrintLedsOn(bool s){p_leds_on = s;};
        void setPrintBrightnessScaler(bool s){p_bs = s;};
        void setPrintPackColors(bool s){p_pack_colors = s;};
        void setPrintAll(bool s);

        ///////////////////////////// Misc //////////////////////////////////////
        bool shutdown(uint16_t len);
        String getName() { return id;};

        void setExtremeLuxShdn(bool e){extreme_lux_shdn = e;};
        bool getLuxShdn() { return extreme_lux_shdn;};

    private:
        /////////////////// Printing ////////////////////
        bool p_onset = false;
        bool p_on_ratio = false;
        bool p_color_wipe = false;
        bool p_pack_colors = false;
        bool p_lux = false;
        bool p_extreme_lux = false;
        bool p_leds_on = false;
        bool p_bs = false;

        bool flash_dominates = false;
        uint8_t mapping = LED_MAPPING_STANDARD;
        uint16_t flash_debounce_time = 50;
        double hsb[3]; // limited from 0 - 255
        uint16_t rgb[3]; // limited from 0.0 - 1.0
        double hue2rgb(double p, double q, double t);
        void RgbToHsb(uint16_t red, uint16_t green, uint16_t blue);
        void HsbToRgb(double hue, double saturation, double lightness);
        void updateColorLog(uint16_t red, uint16_t green, uint16_t blue);
        bool extreme_lux_shdn = false;
        uint32_t packColors(uint16_t red, uint16_t green, uint16_t blue, double scaler);

        ////////////////////////////// Flashes //////////////////////////////////
        uint16_t onset_red = 0;
        uint16_t onset_green = 0;
        uint16_t onset_blue = 255;
        // max values for the click used for some mappings
        uint16_t song_red = 0;
        uint16_t song_green = 0;
        uint16_t song_blue = 0;

        long remaining_flash_delay = 0;// negative values expected, can not be a variable
        bool flash_on = false;
        long flash_min_time;  // how long is the shortest flash?
        long flash_max_time;  // how about the longest?
        elapsedMillis fpm_timer;

        unsigned long num_flashes = 0;// these are public so they can be tracked by the datalog manager
        unsigned long total_flashes = 0;
        double fpm;

        // a linked neogroup for sharing flash info?

        //////////////////////////// Data Tracking /////////////////////////////
        uint64_t red_tot, green_tot, blue_tot;
        uint64_t red_readings, green_readings, blue_readings;
        // related to auto-calibration and datalogging
        bool update_on_off_ratios = UPDATE_ON_OFF_RATIOS;
        String id;

        // what order each led will be displayed in with the custom display
        uint16_t display_order[40];
        // how many LEDs are in display group
        uint16_t display_groups[20];
        // number of display groups
        uint16_t num_display_groups = 0;
        // threshold for each display group
        float display_group_thresholds[20];

        //////////////////////////  LED Linking ////////////////////////////////
        WS2812Serial *leds;
        int idx_start;
        int idx_end;
        int num_pixels;
        elapsedMillis shdn_timer; // if this is below a certain threshold then shutdown everything
        unsigned long shdn_len = 0;// this needs to be a long to 
        bool leds_on = false;
        elapsedMillis last_flash; // the last time a flash message was received
        elapsedMillis on_off_len; // this is reset every time the leds shutdown or turn on (length of time on or off)
        elapsedMillis on_len;
        elapsedMillis off_len;
        elapsedMillis last_flash_update;

        //////////////////////// On Ratio /////////////////////////////////////
        uint64_t on_time = 1;
        uint64_t off_time = 1;
        unsigned long last_on_ratio_update = 0;
        void updateOnRatio(int color);

        /////////////////////// Brightness Scaler ////////////////////////////
        double lux_bs = 1.0;
        ValueTrackerDouble lux_bs_tracker = ValueTrackerDouble("neom_lux_bs",&lux_bs, 1.0);

        double brightness_scaler_total;
        double brightness_scaler_changes;

        uint16_t MIN_BRIGHTNESS = 0;
        uint16_t MAX_BRIGHTNESS = 765;

        /////////////////////// User Brightness Scaler //////////////////////
        double user_brightness_overide = true;
        double user_bs = 1.0;
        /////////////////////// Saturated Colors ////////////////////////////
        bool saturated_colors = false;
        float maxf(float l, float b);
        float minf(float l, float b);
};

void NeoGroup::init() {
    leds->begin();
}

uint32_t NeoGroup::packColors(uint16_t red, uint16_t green, uint16_t blue, double scaler) {
    /*
     * TODO write a function summary
     * 
     * */
    uint32_t color = 0;
    dprintMinorDivide(p_pack_colors);
    dprint(p_pack_colors, "Entering PackColors()        :\t");
    dprint(p_pack_colors, red);
    dprint(p_pack_colors, "\t");
    dprint(p_pack_colors, green); 
    dprint(p_pack_colors, "\t");
    dprintln(p_pack_colors, blue); 

    if (scaler > 1.01 || scaler < 0.99) {
        double dred = (double)red * scaler;
        double dgreen = (double)green * scaler;
        double dblue =(double)blue * scaler;

        double overshoot = dred + dgreen + dblue;
        if (overshoot > MAX_BRIGHTNESS) {
            dred = dred - (overshoot / 3);
            dgreen = dgreen - (overshoot / 3);
            dblue = dblue - (overshoot / 3);
        }

        red = min((uint16_t)dred, 255);
        green = min((uint16_t)dgreen, 255);
        blue = min((uint16_t)dblue, 255);

        dprint(p_pack_colors, "After lux_bs scaling     :\t");
        dprint(p_pack_colors, red);
        dprint(p_pack_colors, "\t");
        dprint(p_pack_colors, green); 
        dprint(p_pack_colors, "\t");
        dprintln(p_pack_colors, blue); 
    }

    if (saturated_colors) {
        if (red >= green && red >= blue) {
            red += (green / 3 + blue / 3);
            green /= 3;
            blue /= 3;
            constrain(red, 0, 255);
        }
        else if (green >= red && green >= blue) {
            green += (red / 3 + blue / 3);
            red /= 3;
            blue /= 3;
            constrain(green, 0, 255);
        }
        else if (blue >= red && blue >= green) {
            blue += (red / 3 + green / 3);
            green /= 3;
            blue /= 3;
            constrain(blue, 0, 255);
        }
        dprint(p_pack_colors, "After saturated_colors   :\t");
        dprint(p_pack_colors, red);
        dprint(p_pack_colors, "\t");
        dprint(p_pack_colors, green); 
        dprint(p_pack_colors, "\t");
        dprintln(p_pack_colors, blue); 
    }

    if (red + green + blue > MAX_BRIGHTNESS){
        uint16_t total = red + green + blue;
        dprint(p_pack_colors + p_color_wipe, "Too Bright, ");
        dprint(p_pack_colors + p_color_wipe, total);
        dprint(p_pack_colors + p_color_wipe, "reducing the brightness of each color: ");
        double factor = total / MAX_BRIGHTNESS;
        red = red / factor;
        green = green / factor;
        blue = blue / factor;
        dprint(p_pack_colors, "\tr: ");
        dprint(p_pack_colors, red);
        dprint(p_pack_colors, "\tg: ");
        dprint(p_pack_colors, green); 
        dprint(p_pack_colors, "\tb: ");
        dprint(p_pack_colors, blue); 
    }

    color = (red << 16) + (green << 8) + (blue);

    dprint(p_pack_colors, "                         :\t");
    dprint(p_pack_colors, red);
    dprint(p_pack_colors, "\t");
    dprint(p_pack_colors, green); 
    dprint(p_pack_colors, "\t");
    dprintln(p_pack_colors, blue); 
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
    dprint(p_bs, "MIN/MAX BRIGHTNESS updated due to lux readings: ");
    dprint(p_bs, MIN_BRIGHTNESS);
    dprint(p_bs, " / ");
    dprintln(p_bs, MAX_BRIGHTNESS);
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

bool NeoGroup::shutdown(uint16_t len) {
    // return 0 if lux shutdown not a success, 1 if it is
    if (!isInShutdown()) {
        dprint(p_lux, millis());dprint(p_lux, "\tSHUTTING DOWN GROUP ");
        dprint(p_lux, id);
        dprint(p_lux, " for a total of at least ");
        dprint(p_lux, len);
        dprint(p_lux," ms");
        // delay(1000);
        colorWipe(0, 0, 0, 0.0);
        dprintln(p_lux," -- neopixels shutdown now");
        // delay(1000);
        shdn_len = len;
        dprintln(p_lux, "shdn_len set to len ");
        // delay(1000);
        dprintln(p_lux, "-- shdn_timer is reset to 0");
        // delay(1000);
        if (leds_on != false) {
            leds_on = false;
            dprint(p_leds_on || p_lux, "\nsetting leds_on to false");
            dprintln(p_leds_on);
        }
        // delay(1000);
        dprintln(p_lux, "exiting neogroup.shutdown() now");
        shdn_timer = 0;
        return true;
    }
    else {
        dprint(p_lux, "SHUTDOWN FOR NEOGROUP CALLED WITH A LEN OF: ");
        dprintln(p_lux, len);
        dprintln(p_lux, "but has been ignored as NeoGroup is already in shutdown");
        return false;
    }
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
    dprintln(p_lux, "POWER ON MESSAGE RECEIVED");
    shdn_timer += shdn_len;
}

unsigned long NeoGroup::getShdnLen() {
    if (shdn_timer <= shdn_len) {
        return shdn_len - shdn_timer;
    } else  {
        return 0;
    }
}

void NeoGroup::setOnsetColors(uint16_t red, uint16_t green, uint16_t blue) {
    onset_red = red;
    onset_green = green;
    onset_blue = blue;
}

void NeoGroup::setSongColors(uint16_t red, uint16_t green, uint16_t blue) {
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

void NeoGroup::RgbToHsb(uint16_t red, uint16_t green, uint16_t blue)
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
    dprint(p_color_wipe, "Entering colorWipeHSB() : ");
    updateHSB(h, s, b);
    HsbToRgb(h, s, b);
    // TODO calculate brightness properly
    colorWipe(rgb[0], rgb[1], rgb[2], b);
}

void NeoGroup::colorWipeAdd(uint16_t red, uint16_t green, uint16_t blue, double bs) {
    red += rgb[0];
    red = constrain(red, 0, 255);
    green += rgb[1];
    green = constrain(green, 0 , 255);
    blue += rgb[2];
    blue = constrain(blue, 0, 255);
    colorWipe(red, green, blue, bs);
}


void NeoGroup::colorWipeAdd(uint16_t red, uint16_t green, uint16_t blue) {
    red += rgb[0];
    red = constrain(red, 0, 255);
    green += rgb[1];
    green = constrain(green, 0 , 255);
    blue += rgb[2];
    blue = constrain(blue, 0, 255);
    colorWipe(red, green, blue, lux_bs);
}

void NeoGroup::colorWipe(uint16_t red, uint16_t green, uint16_t blue, double brightness) {
    colorWipe(red, green, blue, brightness, lux_bs);
}

void NeoGroup::updateColorLog(uint16_t red, uint16_t green, uint16_t blue) {

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

void NeoGroup::setPixel(uint16_t num,uint16_t red, uint16_t green, uint16_t blue, double brightness ) {
    dprintln(p_color_wipe, "Entering setPixel() in NeoGroup - ");
    leds->setPixel(num, packColors(red, green, blue, brightness));
    leds->show();
    dprint(p_color_wipe, "set pixel  ");
    dprintln(p_color_wipe, num); 
}

void NeoGroup::configureCustomMapping(uint16_t _display_order[], uint16_t l) {
    num_pixels = l;
    for (int i = 0; i < 20; i++) {
        display_groups[i] = 0;
    }
    num_display_groups = 0;
    for (int i = 0; i < l; i++) {
        display_order[i] = _display_order[i];
        // keeps track of the number of leds in each display group
        display_groups[_display_order[i]]++;
        num_display_groups = max(num_display_groups, _display_order[i]);
    }
    for (int i = 0; i < num_display_groups; i++) {
        display_group_thresholds[i] = display_groups[i] / l;
    }
}

void NeoGroup::colorWipe(uint16_t red, uint16_t green, uint16_t blue, double brightness, double bs) {
    // TODO this logic is broken...
    // need a debug rating of at least 2 to print these
    // dprintMinorDivide(p_color_wipe);
    dprint(p_color_wipe, "Entering ColorWipe() in NeoGroup - ");
    dprint(p_color_wipe, id);
    dprint(p_color_wipe, " - num_pixels: ");
    dprint(p_color_wipe, num_pixels); 
    dprint(p_color_wipe, " rgb (brightness/lux bs): ");
    dprint(p_color_wipe, red);
    dprint(p_color_wipe, "/t");
    dprint(p_color_wipe, green);
    dprint(p_color_wipe, "/t");
    dprint(p_color_wipe, blue);
    dprint(p_color_wipe, "/t");
    dprint(p_color_wipe, brightness);
    dprint(p_color_wipe, "/t");
    dprintln(p_color_wipe, bs);

    if (extreme_lux_shdn == true) {
        dprintln(p_extreme_lux, F(" colorWipe returning due extreme lux conditions"));
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
        dprint(p_color_wipe, F(" colorWipe returning due to shdn_timer being less than the shutdown length : ")); 
        dprint(p_color_wipe, shdn_timer);
        dprint(p_color_wipe, " < ");
        dprintln(p_color_wipe, shdn_len);
        updateColorLog(0, 0, 0);
        return;
    }

    /////////////////////////////////////////////////////////////////////////////
    if (flash_on == true) {
        // if the flash is on then add the flash colors to the color wipe colors
        if (flash_dominates == false) {
            dprintln(p_color_wipe, " Flash blocked colorWipe");
            red += onset_red;
            green += onset_green;
            blue += onset_blue;
        }
        else {
            dprintln(p_color_wipe, " Flash blocked colorWipe");
            red += onset_red;
            green += onset_green;
            blue += onset_blue;
        }
    }
    //////////////////////////////////////////////////////////////////////////////
    // packColors will take the red, green and blue values (0 - 255) and
    // scale them according the brightness scaler
    if (user_brightness_overide == true) {
        bs = bs * user_bs;
    }
    // colors needs to be called here for the datalogging?
    int colors = packColors(red, green, blue, bs);

    //////////////////////////////////////////////////////////////////////////////
    if (mapping == LED_MAPPING_STANDARD) {
        for (int i = 0; i < num_pixels; i++) {
            leds->setPixel(idx_start + i, colors);
            // dprint(p_color_wipe, idx_start+i);
            // dprint(p_color_wipe, ": ");
            // dprint(p_color_wipe, colors); 
            // dprint(p_color_wipe, "\t");
        }
        // dprintln(p_color_wipe);
    } else if (mapping == LED_MAPPING_CUSTOM) {
        // no matter what the first group is displayed at the colorWipe brightness
        for (int i = 0; i < display_groups[0]; i++) {
            // leds->setPixel(

        }
        // for each display group we have
        for (int i = 0; i < num_display_groups; i++) {
           // if the colorWipe brightness is higher than the threshold, the leds in that group are set to full color 
           // if (brightness > 

        }
    } else if (mapping == LED_MAPPING_OUTSIDE_IN) {
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
        uint16_t _red;
        uint16_t _green;
        uint16_t _blue;

        // loop for the center rings
        dprint(p_color_wipe, "outside in brightness: ");
        dprintln(p_color_wipe, brightness);
        ///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////// OUTER RING //////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        // for the outer ring, if the brightness is 0.5 or greater, the displayed color will
        // be the input RGB values
        // if the brightness is less, they will be proportionate
        if (brightness <= 0.5) {
            _red = red * (brightness * 2);
            _green = green * (brightness * 2);
            _blue= blue * (brightness * 2);
            for (int i = 0; i < 20; i++) {
                // if the brightness is greater than 51 then all the LEDS should be 
                // at max brightness for the center rings
                leds->setPixel(outer_rings[i], _red, _green, _blue);
            }
        } else {
            for (int i = 0; i < 20; i++) {
                // if the brightness is greater than 51 then all the LEDS should be 
                // at max brightness for the center rings
                leds->setPixel(outer_rings[i], red, green, blue);
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////// MIDDLE RING //////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        // temp rgb values for if we need to present a scaledd display for the center ring
        if (brightness <= 0.8) {
            _red = (brightness - 0.5) * 3.3333333333 * red;
            _green= (brightness - 0.5) * 3.3333333333 * green;
            _blue =  (brightness - 0.5) * 3.3333333333 * blue;
            for (int i = 0; i < 12; i++) {
                leds->setPixel(middle_rings[i], _red, _green , _blue);
            }
        } else {
            for (int i = 0; i < 12; i++) {
                leds->setPixel(middle_rings[i], red, green, blue);
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////// CENTER RING //////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        // temp rgb values for if we need to present a scaledd display for the center ring
        _red = (brightness - center_ring_weight - middle_ring_weight) * 2 * red;
        _green= (brightness - center_ring_weight - middle_ring_weight) * 2 * green;
        _blue =  (brightness - center_ring_weight - middle_ring_weight) * 2 * blue;
        for (int i = 0; i < 8; i++) {
            // if the brightness is greater than 51 then all the LEDS should be 
            // at max brightness for the center rings
            leds->setPixel(center_rings[i], _red, _green , _blue);
        }
        dprintln(p_color_wipe);
        // Serial.println("WARNING LED_MAPPING_OUTSIDE_IN is not currently implemented");
    } else if (mapping == LED_MAPPING_CLOCK_HAND) { 

        int idx = brightness / (1.0 / 12.0);
        int times[12][6] = {
            {0, 13, 18, 20, 33, 38},
            {9, 13, 18, 29, 33, 38},
            {9, 14, 19, 29, 34, 39},
            {8, 14, 19, 28, 34, 39},
            {7, 15, 19, 27, 35, 39},
            {6, 15, 19, 26, 35, 39},
            {5, 10, 16, 25, 30, 36},
            {4, 10, 16, 24, 30, 36},
            {3, 11, 17, 23, 31, 37},
            {2, 11, 17, 22, 31, 37},
            {1, 12, 17, 21, 32, 37},
            {0, 12, 17, 20, 32, 37}
        };
        // first determine the LEDs which 
        for (int i = 0; i < 40; i++) {
            leds->setPixel(i, 0, 0, 0);
        }
        for (int i = 0; i < 6; i++) {
            leds->setPixel(times[idx][i], red, green, blue);
        }
    } else if (mapping == LED_MAPPING_CLOCK_FILL) { 
        // first determine the LEDs which 

        int idx = brightness / (1.0 / 12.0);
        int times[12][6] = {
            {0, 13, 18, 20, 33, 38},
            {9, 13, 18, 29, 33, 38},
            {9, 14, 19, 29, 34, 39},
            {8, 14, 19, 28, 34, 39},
            {7, 15, 19, 27, 35, 39},
            {6, 15, 19, 26, 35, 39},
            {5, 10, 16, 25, 30, 36},
            {4, 10, 16, 24, 30, 36},
            {3, 11, 17, 23, 31, 37},
            {2, 11, 17, 22, 31, 37},
            {1, 12, 17, 21, 32, 37},
            {0, 12, 17, 20, 32, 37}
        };
        // first determine the LEDs which 
        for (int i = 0; i < 40; i++) {
            leds->setPixel(i, 0, 0, 0);
        }
        for (int i = 0; i < idx; i++){
            for (int ii = 0; ii < 6; ii++) {
                leds->setPixel(times[i][ii], red, green, blue);
            }
        }
        Serial.println("WARNING LED_MAPPING_CLOCK_FILL is not currently implemented");
    }
    else if (mapping == LED_MAPPING_ROUND) {
        // TODO this logic is broken for when a flash is happening
        red = red * num_pixels;
        green = green * num_pixels;
        blue = blue * num_pixels;
        for (int i = 0; i < num_pixels; i++) {
            // if we have more than the max then just add the max to the target
            uint16_t _red = 0;
            uint16_t _green = 0;
            uint16_t _blue = 0;
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
            int _colors = packColors(_red, _green, _blue, lux_bs);
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
        // loop for the center rings
        dprint(p_color_wipe, "center out brightness: ");
        dprintln(p_color_wipe, brightness);

        ///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////// CENTER RING //////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        // temp rgb values for if we need to present a scaledd display for the center ring
        uint16_t _red = brightness * 5 * red;
        uint16_t _green= brightness * 5 * green;
        uint16_t _blue = brightness * 5 * blue;
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
        dprintln(p_color_wipe);
    }
    else if (mapping == LED_MAPPING_BOTTOM_UP) {
        Serial.println("WARNING - this colorwipe mapping LED_MAPPING_BOTTOM_UP is not currently implemented");
    }
    else {
        dprintln(p_color_wipe, F("WARNING - The mapping strategy currently selected is not implemented, colorWipe is exiting now"));
    }
    leds->show();
    // if the LEDs are on set "led_on" to true, otherwise turn "led_on" to false
    // also reset led_off_len if the leds  were just turned off
    if (colors == 0) {
        if (leds_on != false) {
            leds_on = false;
            dprintln(p_leds_on, F(" setting leds_on to false"));
        }
    } else {
        if (leds_on != true) {
            leds_on = true;
            dprintln(p_leds_on, F(" setting leds_on to true"));
        }
    }
    updateColorLog(red, green, blue);
    // dprintln(p_color_wipe, F(" finished updating the neopixels"));

}

////////////// General Purpose Helper Functions /////////////////////////
void NeoGroup::flashOff() {
    // if the flash is allowed to be turned off
    if (remaining_flash_delay <= 0) {
        dprint(p_onset, id);
        dprint(p_onset, " FlashOff : ");
        dprintln(p_onset, last_flash);
        flash_on = false;
        if (leds_on != false) {
            dprintln(p_leds_on, F("leds_on set to false"));
            leds_on = false;
        }
        colorWipe(0, 0, 0, 0.0);
        remaining_flash_delay = 0;
        // last_flash = 0;
    }
}

bool NeoGroup::flashOn(uint16_t red, uint16_t green, uint16_t blue) {
    // if it has been uint32_t enough since the last flash occured
    if (last_flash < flash_debounce_time) {
        dprint(p_onset, F("Flash skipped due to flash_debounce_time : "));
        dprintln(p_onset, last_flash);
        return false;
    }
    if (red + green + blue > 0 && shdn_timer > shdn_len) {
        // if a flash is not currently on
        if ( (flash_on == false) || (leds_on == false) ) {
            remaining_flash_delay = flash_min_time;
            colorWipe(red, green, blue, lux_bs); // has to be on first as flash_on will block the colorWipe
            flash_on = true; // turn the light on along with the flag
            if (leds_on != true) {
                dprintln(p_leds_on, F("leds_on set to true"));
                leds_on = true;
            }
            last_flash = 0; // reset the elapsed millis variable as the light was just turned on
            num_flashes = num_flashes  + 1;
            total_flashes++;
            getFPM();
            dprint(p_onset, id);
            dprint(p_onset, " FLASH ON #");
            dprint(p_onset, num_flashes);
            dprint(p_onset, " Flashed "); dprint(p_onset, remaining_flash_delay);
            dprint(p_onset, " FPM "); dprintln(p_onset, fpm);
        } else { // if a flash is on then increment the remaining_flash_Delay
            addToRemainingFlashDelay(1);
            if (remaining_flash_delay > flash_max_time) {
                remaining_flash_delay = flash_max_time;
            }
        }
        return true;
    }
    return false;
}

bool NeoGroup::flashOn() {
    return flashOn(onset_red, onset_green, onset_blue);
}

//////////////////////////////////////////////////////
////////////// Update Methods/////////////////////////
//////////////////////////////////////////////////////
void NeoGroup::update() {
    // if there is time remaining in the flash it either needs to be turned
    // on or the timer needs to increment
    if (remaining_flash_delay > 0) {
        dprintMinorDivide(p_onset);
        dprint(p_onset, "flash delay "); dprint(p_onset, id); dprint(p_onset, " : ");
        dprint(p_onset, remaining_flash_delay); dprintTab(p_onset);
        dprint(p_onset, last_flash_update); dprintTab(p_onset);
        // if the flash is not currently on, turn the flash on
        if (flash_on == false) { //and the light is not currently on
            dprintln(p_onset, F("-- Turning the Flash ON --"));
            flashOn(onset_red, onset_green, onset_blue);// flash on
        }
        // if the flash is already on subtract from the timer
        else {
            dprintln(p_onset, "- - - - - - - - - - - - - - - - - - -");
            dprint(p_onset, "last_flash :\t"); dprintln(p_onset, last_flash);
            dprint(p_onset, "remaining_flash_delay "); 
            dprint(p_onset, id); dprint(p_onset, ":\t");
            dprint(p_onset, remaining_flash_delay); dprint(p_onset, "\t");
            remaining_flash_delay = remaining_flash_delay - last_flash_update;
            remaining_flash_delay = max(remaining_flash_delay, 0);
            dprintln(p_onset, remaining_flash_delay);
            if (remaining_flash_delay == 0) {
                dprint(p_onset, F("Click time over, turning off flash ")); dprintln(p_onset, id);
                flashOff(); // turn off the NeoPixels
            }
        }
    }
    // this is so the timer does not get reset 
    // if it has been running for less than one ms
    if (last_flash_update != 0) {
        last_flash_update = 0;
        dprintln(p_onset, "updated last_flash_upate to 0");
    }
}


double NeoGroup::getAvgBrightnessScaler() {
    return lux_bs_tracker.getAvg();
}

void NeoGroup::resetAvgBrightnessScaler() {
    lux_bs_tracker.getAvg(true);
}

void NeoGroup::resetOnRatio() {
    on_ratio = (double)on_time / (double)(on_time + off_time);
    on_time = 0;
    off_time = 0;
    dprintln(p_on_ratio, "reset the led on/off ratio counters");
}

// mode 0 is just front, mode 1 is just rear, mode 2 is both (using combined values?), mode 3 is both using independent values
void NeoGroup::updateOnRatio(int color) {
    // when color wipe is called it should take care of this for  us
    // to keep track of on/off times
    bool update = false;
    if (color > 0 && !isInShutdown()) {
        on_time += on_off_len;
        on_off_len = 0;
        off_len = 0;
        update = true;
    } else if (color <= 0){
        off_time += on_off_len;
        on_off_len = 0;
        on_len = 0;
        update = true;
    }

    if (on_time > 0 && off_time > 0 && update) {
        on_ratio = (double)on_time / (double)(on_time + off_time);
    }

    dprint(p_on_ratio, "updated led on/off ratio "); dprint(p_on_ratio, " :\t");
    dprint(p_on_ratio, on_ratio); dprint(p_on_ratio, "\t=\t"); dprint(p_on_ratio, on_time);
    dprint(p_on_ratio, "\t"); dprintln(p_on_ratio, off_time);
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

void NeoGroup::setPrintAll(bool s) {
        p_onset = s;
        p_on_ratio = s;
        p_color_wipe = s;
        p_pack_colors = s;
        p_lux = s;
        p_extreme_lux = s;
        p_leds_on = s;
        p_bs = s;
}

float NeoGroup::minf(float l, float b) {
    if (l < b){
        return l;
    }
    return b;
}

float NeoGroup::maxf(float l, float b) {
    if (l > b){
        return l;
    }
    return b;
}

#endif // __LEDS_H__
