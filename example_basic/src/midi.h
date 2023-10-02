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

#define APCMINI_NUM_ROWS      8
#define APCMINI_DISPLAY_WIDTH 8

#define APCMINI_BUTTON_CLIP_STOP 82
#define APCMINI_BUTTON_SOLO      83
#define APCMINI_BUTTON_REC_ARM   84
#define APCMINI_BUTTON_MUTE      85
#define APCMINI_BUTTON_SELECT    86
#define APCMINI_BUTTON_UNLABELED_1    87
#define APCMINI_BUTTON_UNLABELED_2    88
#define APCMINI_BUTTON_STOP_ALL_CLIPS 89
#define APCMINI_BUTTON_SHIFT     98
#define APCMINI_BUTTON_UP        64
#define APCMINI_BUTTON_DOWN      65
#define APCMINI_BUTTON_LEFT      66
#define APCMINI_BUTTON_RIGHT     67
#define APCMINI_BUTTON_VOLUME    68
#define APCMINI_BUTTON_PAN       69
#define APCMINI_BUTTON_SEND      70
#define APCMINI_BUTTON_DEVICE    71


#include "ofxMidi.h"
#include "ofSoundPlayer.h"

class APCDisplayManager {
    public:

    int8_t last_note_values[127];

	ofxMidiIn *midiIn;
	ofxMidiOut *midiOut;
    ofxPiMapper *mapper;
    ofSoundPlayer *player;

	int currently_selected_audio_clip = -1;

    APCDisplayManager(ofxPiMapper *mapper, ofxMidiIn *midiIn, ofxMidiOut *midiOut, ofSoundPlayer *player) {
        this->midiIn = midiIn;
        this->midiOut = midiOut;
        this->mapper = mapper;
        this->player = player;
    }

    int get_apcmini_note_for_preset(int i) {
        int row = 1 - (i/8);
        int column = i % 8;
        int note = 0x30 + (row*8) + column;
        //ofLogNotice("get_apcmini_note_for_preset(") << i << ") returning note " << note;
        return note;
    }
    int get_preset_for_apcmini_note(int i) {
        int x = i - 0x30;
        int row = 1 - (x / 8);
        int column = x % 8;
        int f_key = (row*8) + column;
        return f_key;
    }

    int get_apcmini_note_for_audio_slot(int i) {
        int row = 1 - (i/8);
        int column = i % 8;
        int note = 0x00 + (row*8) + column;
        //ofLogNotice("get_apcmini_note_for_audio_slot(") << i << ") returning note " << note;
        return note;
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


    void indicateAudio(int clip, int value) {
        this->sendNoteOn(1, get_apcmini_note_for_audio_slot(clip), value);
    }

    void update() {
        for (int i = 0 ; i < 16 ; i++) {
            indicatePreset(i, mapper->_application.getSurfaceManager()->getActivePresetIndex()==i ? APCMINI_GREEN : APCMINI_OFF);
        }
        for (int i = 0 ; i < 16 ; i++) {
            indicateAudio(i, currently_selected_audio_clip==i && player->isPlaying() ? APCMINI_GREEN_BLINK : APCMINI_OFF );
        }
    }
};