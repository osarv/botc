#include <Arduino.h>
#include <IRremote.h>

const int playerNum = 20;
const int buzzerPin = 12;
const int IRpin = 11;

const char BUTTON_PREV_PLAYER = 0x45;
const char BUTTON_NEXT_PLAYER = 0x46;
const char BUTTON_TOGGLE_PLAYER_STATE = 0x47;
const char BUTTON_TIMER_ADD_ONE_MIN= 0x40;
const char BUTTON_TIMER_ADD_TEN_SEC = 0x44;
const char BUTTON_SET_TIMER = 0x43;
const char BUTTON_RESET_TIMER = 0x9;

typedef struct {
    char dataPin;
    char clockPin;
    char latchPin;
} ShiftReg;

ShiftReg shiftReg0 = {3, 4, 5};
ShiftReg shiftReg1 = {6, 7, 8};
ShiftReg shiftReg2 = {9, 10, 11};
ShiftReg shiftReg3 = {14, 15, 16};
ShiftReg shiftReg4 = {17, 18, 19};

void shiftRegSetup(ShiftReg shiftReg) {
    pinMode(shiftReg.dataPin, OUTPUT);
    pinMode(shiftReg.clockPin, OUTPUT);
    pinMode(shiftReg.latchPin, OUTPUT);
}

void shiftRegWrite(ShiftReg shiftReg, byte data) {
    digitalWrite(shiftReg.latchPin, LOW);
    shiftOut(shiftReg.dataPin, shiftReg.clockPin, MSBFIRST, data);
    digitalWrite(shiftReg.latchPin, HIGH);
}

char playerStates [20] = {0};
char composeShiftRegData(int playerIdx) {
    char dataByte = 0;
    int baseIdx = playerIdx - (playerIdx % 4);
    for (int i = 0; i < 4; i++) {
        dataByte |= playerStates[baseIdx + i] << 2 * i;
    }
    return dataByte;
}

void toggleStateOfPlayer(int playerIdx) {
    ShiftReg activeSReg;
    if (playerIdx < 4) {
        activeSReg = shiftReg0;
    } else if (playerIdx < 8) {
        activeSReg = shiftReg1;
    } else if (playerIdx < 12) {
        activeSReg = shiftReg2;
    } else if (playerIdx < 16) {
        activeSReg = shiftReg3;
    } else activeSReg = shiftReg4;

    playerStates[playerIdx]++;
    playerStates[playerIdx] %= 3;
    shiftRegWrite(activeSReg, composeShiftRegData(playerIdx));
}

void returnToTS() {
    unsigned long startTime = millis();
    while(millis() - startTime < 2000) {
        tone(buzzerPin, 50);
        delay(100);
        tone(buzzerPin, 80);
        delay(100);
        tone(buzzerPin, 60);
        delay(100);
        tone(buzzerPin, 40);
        delay(100);
        tone(buzzerPin, 70);
        delay(100);
    }
    noTone(buzzerPin);
}

void setup() {
    Serial.begin(9600);
    IrReceiver.begin(2, ENABLE_LED_FEEDBACK);
    shiftRegSetup(shiftReg0);
    shiftRegSetup(shiftReg1);
    shiftRegSetup(shiftReg2);
    shiftRegSetup(shiftReg3);
    shiftRegSetup(shiftReg4);
}

const int IRdelayMs = 1000;
int IRdelayBaseline = 0;
int selectedPlayer = 0;
int timerVal = 0;

void loop() {
    int currentTime = millis();
    if (IrReceiver.decode() && currentTime - IRdelayBaseline > IRdelayMs) {
        IRdelayBaseline = currentTime;
        uint8_t cmd = IrReceiver.decodedIRData.command;
        switch(cmd) {
            case BUTTON_PREV_PLAYER:
                IRdelayBaseline -= IRdelayMs;
                selectedPlayer--;
                if (selectedPlayer < 0) selectedPlayer += playerNum;
                Serial.print("selected player: ");
                Serial.println(selectedPlayer);
                break;

            case BUTTON_NEXT_PLAYER:
                IRdelayBaseline -= IRdelayMs;
                selectedPlayer++;
                if (selectedPlayer >= playerNum) selectedPlayer -= playerNum;
                Serial.print("selected player: ");
                Serial.println(selectedPlayer);
                break;

            case BUTTON_TOGGLE_PLAYER_STATE:
                Serial.println("toggling player state");
                toggleStateOfPlayer(selectedPlayer);
                break;

            case BUTTON_TIMER_ADD_ONE_MIN:
                timerVal += 60;
                Serial.println("added one minute to the timer");
                break;

            case BUTTON_TIMER_ADD_TEN_SEC:
                timerVal += 10;
                Serial.println("added ten seconds to the timer");
                break;

            case BUTTON_SET_TIMER:
                Serial.print("setting timer for ");
                Serial.print(timerVal);
                Serial.println(" seconds");
                timerVal = 0;
                break;

            case BUTTON_RESET_TIMER:
                returnToTS();
                timerVal = 0;
                Serial.println("timer is reset");
                break;
        }
        IrReceiver.resume();
    }
}
