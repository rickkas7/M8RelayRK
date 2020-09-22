#include "M8RelayRK.h"

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

SerialLogHandler logHandler;

M8RelayLatching relays;
int relayNum = 0;
bool value = 0;

void setup() {
	relays.begin();
    pinMode(A1, INPUT_PULLUP);
}

void loop() {
    if (digitalRead(A1) == LOW) {
		bool bResult = relays.relaySet((uint16_t)relayNum, (bool) value);
		Log.info("relayNum=%d value=%d bResult=%d", relayNum, value, (int) bResult);

        if (value) {
            relayNum++;
            if (relayNum >= 2) {
                relayNum = 0;
            }
        }
        value = !value;

        delay(1000);
    }
}

