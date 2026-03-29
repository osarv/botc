#include <Arduino.h>
#include <Servo.h>
//#include <LiquidCrystal.h>

/*
ISR(TIMER1_COMPA_vect) {
    digitalWrite(buzzerPin, !digitalRead(buzzerPin)); // Toggle the buzzer state
}

void setup() {
    sei(); // Enable global interrupts
    pinMode(buzzerPin, OUTPUT);
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare interrupt

    // CTC mode (Clear Timer on Compare)
    TCCR1B |= (1 << WGM12);

    // Set compare value for desired frequency
    OCR1A = 15; //arduino UNO clock rate 16Mhz

    // Prescaler = 1 and start the timer
    TCCR1B |= (1 << CS10);

}
*/

const int buzzerPin = 13;
const int servo1Pin = 12;
const int servo2Pin = 11;
Servo clockServo1;
Servo clockServo2;

void returnToTS() {
    int startTime = millis();
    while(millis() - startTime < 5000) {
        tone(buzzerPin, 1000);
        delay(100);
        tone(buzzerPin, 2000);
        delay(100);
        tone(buzzerPin, 1000);
        delay(100);
        tone(buzzerPin, 3000);
        delay(100);
    }
    noTone(buzzerPin);
}

void setClockAngle(int angle) {
    if (angle < 180) {
        clockServo1.write(angle);
        clockServo2.write(0);
    }
    else {
        clockServo1.write(180);
        clockServo2.write(angle - 180);
    }
}

void setup() {
    clockServo1.attach(servo1Pin);
    clockServo2.attach(servo2Pin);
}

void loop() {
    returnToTS();
    for (int i = 0; i <= 360; i++) {
        setClockAngle(i);
        delay(30);
    }
}
