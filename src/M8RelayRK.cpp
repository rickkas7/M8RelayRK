

#include "M8RelayRK.h"


M8Relay::M8Relay(TwoWire &wire, uint8_t mcp23008_addr) : gpio(wire, mcp23008_addr) {

}

M8Relay::~M8Relay() {

}

bool M8Relay::begin() {
#if (PLATFORM_ID == PLATFORM_TRACKER)
    // Turn on CAN_5V power
    pinMode(CAN_PWR, OUTPUT);
    digitalWrite(CAN_PWR, HIGH);
    delay(100);
#endif

    gpio.begin();

    for(uint16_t ii = 0; ii < numOutputs; ii++) {
        gpio.pinMode(ii, OUTPUT);
        gpio.digitalWrite(ii, LOW);
    }

    return true;
}

bool M8Relay::relayOnOff(uint16_t relayNum, bool on) {
    if (relayNum < numOutputs) {
        gpio.digitalWrite(relayNum, on ? HIGH : LOW);
        return true;
    }
    else {
        return false;
    }
}

M8RelayLatching::M8RelayLatching(TwoWire &wire, uint8_t mcp23008_addr) : 
    M8Relay(wire, mcp23008_addr), 
    timer(30, &M8RelayLatching::timerCallback, *this) {

}
M8RelayLatching::~M8RelayLatching() {

}

bool M8RelayLatching::begin() {
    bool bResult = M8Relay::begin();

    os_queue_create(&queue, sizeof(M8RelayQueueEntry), 8, 0);

    timer.start();
    
    return bResult;
}

bool M8RelayLatching::relaySet(uint16_t relayNum, bool set) {
    M8RelayQueueEntry entry;

    entry.s.gpioNum = relayNum * 2 + (set ? 1 : 0);

    // Add an item in the queue to see the value high (activate the relay)
    entry.s.value = HIGH;    
    os_queue_put(queue, (void *)&entry, 0, 0);

    // Then, 30 ms later, restore it to LOW (coil off).
    entry.s.value = LOW;
    os_queue_put(queue, (void *)&entry, 0, 0);

    return true;
}

bool M8RelayLatching::relayOnOff(uint16_t relayNum, bool on) {
    return false;
}


void M8RelayLatching::timerCallback() {
    M8RelayQueueEntry entry;

    if (os_queue_take(queue, &entry, 0, 0) == 0) {
        Log.info("gpio=%d value=%d", (int)entry.s.gpioNum, (int)entry.s.value);
		gpio.digitalWrite(entry.s.gpioNum, entry.s.value);
    }
}


