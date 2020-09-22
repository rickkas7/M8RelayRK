#include "M8RelayRK.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler;

M8RelayLatching relays;

int testFunction(String extra);

void setup() {
	Particle.function("test", testFunction);
	relays.begin();
}

void loop() {

}

int testFunction(String param) {
	int res = -1;

	int relayNum, value;
	if (sscanf(param.c_str(), "%d,%d", &relayNum, &value) == 2) {
		bool bResult = relays.relaySet((uint16_t)relayNum, (bool) value);
		Log.info("relayNum=%d value=%d bResult=%d", relayNum, value, (int) bResult);
		if (bResult) {
			res = 0;
		}
	}

	return res;
}
