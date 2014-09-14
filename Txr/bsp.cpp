
//****************************************************************************
// Model: pelican.qm
// File:  ./bsp.cpp
//
// This code has been generated by QM tool (see state-machine.com/qm).
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//****************************************************************************
//${.::bsp.cpp} ..............................................................
#include "qp_port.h"
#include "Txr.h"
#include "bsp.h"
#include "Arduino.h"                                   // Arduino include file
#include <Encoder.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "RollingAverager.h"


Q_DEFINE_THIS_FILE

//#define SAVE_POWER

#define TICK_DIVIDER       ((F_CPU / BSP_TICKS_PER_SEC / 1024) - 1)

#if TICK_DIVIDER > 255
#   error BSP_TICKS_PER_SEC too small
#elif TICK_DIVIDER < 2
#   error BSP_TICKS_PER_SEC too large
#endif

// Switches and buttons
#define PLAY_SW    4  // Freerun switch
#define FREE_SW    12 // Playback switch
#define cBUTTON    A5 // Calibrate button
#define p1BUTTON   A1 // Preset 1
#define p2BUTTON   A2 // Preset 2
#define p3BUTTON   A3 // Preset 3
#define p4BUTTON   A4 // Preset 4

// LEDs
#define rLED       6  // Red LED
#define rLED2      9  // Red #2 LED
#define gLED       10 // Green LED
#define gLED2      11 // Green #2 LED
#define enc_rLED   3  // Start record mode, red LED built into rotary encoder
#define enc_gLED   5  // Green led in record mode, green LED built into rotary encoder


static Encoder encoder(1,2);
static int PrevButtonState = 0;
static int PrevModeState = -1;  // force signal on startup with -1

#ifdef Q_SPY
uint8_t l_TIMER2_COMPA;
#endif

// ISRs ----------------------------------------------------------------------
ISR(TIMER4_COMPA_vect) {

    // No need to clear the interrupt source since the Timer4 compare
    // interrupt is automatically cleard in hardware when the ISR runs.

    QF::TICK(&l_TIMER2_COMPA);                // process all armed time events
    
    // Check state of buttons
    int curButtonState = CALBUTTON_ON();
    if (curButtonState != PrevButtonState) {
      if (curButtonState != 0) {
        QF::PUBLISH(Q_NEW(QEvt, ENC_DOWN_SIG), &l_TIMER2_COMPA);
      }
      else {
        QF::PUBLISH(Q_NEW(QEvt, ENC_UP_SIG), &l_TIMER2_COMPA);
      }
       
      PrevButtonState = curButtonState;
    }
    
    //Check mode switches
    curButtonState = MODE_SWITCHES();
    if (curButtonState != PrevModeState) {
      if (curButtonState & 0x40) {
        QF::PUBLISH(Q_NEW(QEvt, PLAY_MODE_SIG), &l_TIMER2_COMPA);
      } 
      else if (curButtonState & 0x10) {
        QF::PUBLISH(Q_NEW(QEvt, FREE_MODE_SIG), &l_TIMER2_COMPA);
      }
      else {
        QF::PUBLISH(Q_NEW(QEvt, Z_MODE_SIG), &l_TIMER2_COMPA);
      }
      PrevModeState = curButtonState;
    }
}

//............................................................................
void BSP_init(void) {
  pinMode(PLAY_SW, INPUT);
  pinMode(FREE_SW, INPUT); 
  pinMode(cBUTTON, INPUT);
  pinMode(p1BUTTON, INPUT);
  pinMode(p2BUTTON, OUTPUT);
  pinMode(p3BUTTON, INPUT);
  pinMode(p4BUTTON, INPUT);
  pinMode(rLED, OUTPUT);  
  pinMode(rLED2, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(gLED2, OUTPUT);
  pinMode(enc_rLED, OUTPUT);
  pinMode(enc_gLED, OUTPUT); 
  
  digitalWrite(A5, HIGH);
    
    
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"clie1");
  Mirf.setTADDR((byte *)"serv1");
  Mirf.payload = sizeof(unsigned long);
  Mirf.config();
  
  Serial.begin(9600);   // set the highest stanard baud rate of 115200 bps

  //if (QS_INIT((void *)0) == 0) {       // initialize the QS software tracing
  //    Q_ERROR();
  //}

  //QS_OBJ_DICTIONARY(&l_SysTick_Handler);
}

//............................................................................
void QF::onStartup(void) {
  // set Timer2 in CTC mode, 1/1024 prescaler, start the timer ticking
  TCCR4A = (1 << WGM41) | (0 << WGM40);
  TCCR4B = (1 << CS42 ) | (1 << CS41) | (1 << CS40);               // 1/2^10
  //ASSR &= ~(1 << AS2);
  TIMSK4 = (1 << OCIE4A);                 // Enable TIMER2 compare Interrupt
  TCNT4 = 0;
  OCR4A = TICK_DIVIDER;     // must be loaded last for Atmega168 and friends
}

//............................................................................
void QF::onCleanup(void) {
}

//............................................................................
void QF::onIdle() {

    GREEN_LED_ON();     // toggle the GREEN LED on Arduino on and off, see NOTE1
    GREEN_LED_OFF();

#ifdef SAVE_POWER

  SMCR = (0 << SM0) | (1 << SE);  // idle sleep mode, adjust to your project

  // never separate the following two assembly instructions, see NOTE2
  __asm__ __volatile__ ("sei" "\n\t" :: );
  __asm__ __volatile__ ("sleep" "\n\t" :: );

  SMCR = 0;                                              // clear the SE bit

#else
  QF_INT_ENABLE();
#endif
}

void BSP_UpdateRxProxy(long pos)
{  
  if (!Mirf.isSending()) {
    Mirf.send((byte *)&pos);
  }
}

long BSP_GetEncoder()
{
  return encoder.read();
}

int BSP_GetPot()
{  
  return analogRead(A0);
}

//............................................................................
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    QF_INT_DISABLE();                                // disable all interrupts
    GREEN_LED_ON();                                  // GREEN LED permanently ON
    asm volatile ("jmp 0x0000");    // perform a software reset of the Arduino
}

//////////////////////////////////////////////////////////////////////////////
// NOTE1:
// The Arduino's User LED is used to visualize the idle loop activity.
// The brightness of the LED is proportional to the frequency of invcations
// of the idle loop. Please note that the LED is toggled with interrupts
// locked, so no interrupt execution time contributes to the brightness of
// the User LED.
//
// NOTE2:
// The QF_onIdle() callback is called with interrupts *locked* to prevent
// a race condtion of posting a new event from an interrupt while the
// system is already committed to go to sleep. The only *safe* way of
// going to sleep mode is to do it ATOMICALLY with enabling interrupts.
// As described in the "AVR Datasheet" in Section "Reset and Interrupt
// Handling", when using the SEI instruction to enable interrupts, the
// instruction following SEI will be executed before any pending interrupts.
// As the Datasheet shows in the assembly example, the pair of instructions
//     SEI       ; enable interrupts
//     SLEEP     ; go to the sleep mode
// executes ATOMICALLY, and so *no* interrupt can be serviced between these
// instructins. You should NEVER separate these two lines.
//

