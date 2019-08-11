
#ifndef ACHILLES_WEDGE_H
#define ACHILLES_WEDGE_H

#include "Achilles.h"


//  ***********  METHODS YOU NEED TO CALL *************
// Initialize communications, set up callbacks
void setupComm(int wedgeAddress);

// You need to call to indicate that local activity was seen
void localActivitySeen();

//  ***********  METHODS FOR INTERACTING WITH POINTS *************

// In each of the following, point should be a value in the range 0 to 7
void setPoint(int p);
void setPointTo(int p, bool value);
void clearPoint(int p);
bool getPoint(int p);

// These methods effect all of the points
void clearAllPoints();
void setPointBits(uint8_t bitvector);


//  ***********  METHODS TO GET INFO ABOUT SYSTEM STATE *************

SystemMode getSystemMode();
int getLightLevel();
Daytime getDaytime();

// Number of seconds since any global activity;
uint16_t secondsSinceGlobalActivity();

//  ***********  METHODS TO PLAY A SOUND *************

// Request that a sound be played; overwrites previous request if queued
void playSound(uint16_t track, bool global);

// is a sound currently queued
bool soundQueued();


//  ***********  METHODS TO GET INFO ABOUT COMM STATE *************

// Return true if comms seem to be OK; messages have been sent and received in the past second
bool commOK();

// Return true if a message has been received since the last time
// this method returned true.
extern bool newMessageReceived();

// Return true if a message has been sent since the last time
// this method returned true.
extern bool newMessageSent();

// Number of milliseconds since a message was received
unsigned long millisSinceLastMessageReceived();

//  ***********  STUFF YOU NEED TO GO DEEPER *************


//extern unsigned long lastLocalActivity;
//extern unsigned long lastGlobalActivity;
//
//extern volatile boolean receivedMsg;
//extern unsigned long lastMsgReceivedAt;
//
//extern FromWidgetData fromWidgetData;
//extern ToWidgetData toWidgetData;
#endif
