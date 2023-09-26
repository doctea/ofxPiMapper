#pragma once

#define APCMINI_CC_SLIDER_1 0x30
#define APCMINI_CC_SLIDER_2 0x31
#define APCMINI_CC_SLIDER_3 0x32
#define APCMINI_CC_SLIDER_4 0x33
#define APCMINI_CC_SLIDER_5 0x34
#define APCMINI_CC_SLIDER_6 0x35
#define APCMINI_CC_SLIDER_7 0x36
#define APCMINI_CC_SLIDER_8 0x37
#define APCMINI_CC_SLIDER_MASTER 0x38

#define APCMINI_OFF           0
#define APCMINI_ON            1
#define APCMINI_GREEN         1
#define APCMINI_GREEN_BLINK   2
#define APCMINI_RED           3
#define APCMINI_RED_BLINK     4
#define APCMINI_YELLOW        5
#define APCMINI_YELLOW_BLINK  6

#include "ofxMidi.h"

class APCDisplayManager {
    public:

    int8_t last_note_values[127];

	ofxMidiIn *midiIn;
	ofxMidiOut *midiOut;
    ofxPiMapper *mapper;

    APCDisplayManager(ofxPiMapper *mapper, ofxMidiIn *midiIn, ofxMidiOut *midiOut) {
        this->midiIn = midiIn;
        this->midiOut = midiOut;
        this->mapper = mapper;
    }

    int get_apcmini_note_for_preset(int i) {
        int row = 1 - (i/8);
        int column = i % 8;
        int note = 0x30 + (row*8) + column;
        ofLogNotice("get_apcmini_note_for_preset(") << i << ") returning note " << note;
        return note;
    }
    int get_preset_for_apcmini_note(int i) {
        int x = i - 0x30;
        int row = 1 - (x / 8);
        int column = x % 8;
        int f_key = (row*8) + column;
        return f_key;
    }

    int get_audio_slot_for_apcmini_note(int i) {
        int x = i;// - 0x30;
        int row = 1 - (x / 8);
        int column = x % 8;
        int f_key = (row*8) + column;
        return f_key;
    }

    void sendNoteOn(int channel, int pitch, int velocity, bool force = false) {
        if (force || last_note_values[pitch] != velocity) {
            midiOut->sendNoteOn(channel, pitch, velocity);
            last_note_values[pitch] = velocity;
        }
    }

    void indicatePreset(int preset, int value) {
        this->sendNoteOn(1, get_apcmini_note_for_preset(preset), value);
    }

    void update() {
        for (int i = 0 ; i < 16 ; i++) {
            indicatePreset(i, mapper->_application.getSurfaceManager()->getActivePresetIndex()==i ? APCMINI_GREEN : APCMINI_OFF);
        }
    }
};