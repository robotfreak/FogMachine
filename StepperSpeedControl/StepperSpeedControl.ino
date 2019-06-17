// ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
/// \author  Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2009 Mike McCauley
// $Id: ConstantSpeed.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

AccelStepper stepper(1,5,4); // AccelStepper 2 wire board (2 pins) 5 step, 4 dir

// This defines the analog input pin for reading the control voltage
// Tested with a 10k linear pot between 5v and GND
#define ANALOG_IN A0

void setup()
{  
   stepper.setMaxSpeed(1000);
   stepper.setSpeed(100);	
}

void loop()
{  
   int analogIn = analogRead(ANALOG_IN);
   int motorSpeed = map(analogIn, 0, 1023, 0, 200);
   stepper.setSpeed(motorSpeed);  
   stepper.runSpeed();
}
