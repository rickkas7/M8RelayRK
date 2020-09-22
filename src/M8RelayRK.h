#ifndef __M8RELAYRK_H
#define __M8RELAYRK_H

#include "Particle.h"

#include "MCP23008-RK.h"

/**
 * @brief Support for the M8 quad relay board. Also is the base class for M8RelayLatching
 */
class M8Relay {
public:
    /**
     * @brief Construct for the M8 quad relay board
     * 
     * @param wire The interface to use (Wire3 for the Tracker One M8 connector, the default)
     * 
     * @param mcp23008_addr The I2C address of the MCP23008. The default is 0x20 which is
     * the right value for the board as designed.
     */
    M8Relay(TwoWire &wire = Wire3, uint8_t mcp23008_addr = 0x20);
    virtual ~M8Relay();

    /**
     * @brief Initialize the library. Required. Usually called from setup().
     * 
     * @return True if successful or false if not.
     */
    virtual bool begin();

    /**
     * @brief Turns a relay on or off
     * 
     * @param relayNum Relay number 0 - 3 (inclusive). The board is labeled 1 - 4 so keep that in mind.
     * 
     * @param on Pass true to energize the relay (NO contact to common is connected), or false to 
     * release the relay (NC contact to common is connected).
     */
    virtual bool relayOnOff(uint16_t relayNum, bool on = true);

protected:
    MCP23008 gpio;
    uint16_t numOutputs = 4;
};

/**
 * @brief Class used internally to manage turning on and off latching relay coils
 */
typedef union {
    uint32_t value;
    struct {
        uint16_t gpioNum;
        uint8_t  value;
        uint8_t  reserved;
    } s;
} M8RelayQueueEntry;

/**
 * @brief Support for the M8 dual latching relay board
 */
class M8RelayLatching : public M8Relay {
public:
    /**
     * @brief Construct for the M8 dual latching relay board
     * 
     * @param wire The interface to use (Wire3 for the Tracker One M8 connector, the default)
     * 
     * @param mcp23008_addr The I2C address of the MCP23008. The default is 0x20 which is
     * the right value for the board as designed.
     */
    M8RelayLatching(TwoWire &wire = Wire3, uint8_t mcp23008_addr = 0x20);
    virtual ~M8RelayLatching();

    /**
     * @brief Initialize the library. Required. Usually called from setup().
     * 
     * @return True if successful or false if not.
     */
    bool begin();

    /**
     * @brief Puts the relay in set or reset mode
     * 
     * @param relayNum The relay number 0 or 1
     * 
     * @param set Put the relay is set mode (true, default), or reset mode (false).
     * 
     * Can also use relayReset(), but using the set parameter is useful if you're 
     * setting the relay state from a bool so you don't need an if statement.
     */
    bool relaySet(uint16_t relayNum, bool set = true);


    /**
     * @brief Puts the relay in reset mode
     * 
     * Same as using relaySet(relayNum, false).
     */
    bool relayReset(uint16_t relayNum) { return relaySet(relayNum, false); };

    /**
     * @brief relayOnOff is not used for latching relays
     */
    bool relayOnOff(uint16_t relayNum, bool on = true);

protected:
    void timerCallback();

    Timer timer;
    os_queue_t queue;
};

#endif /* __M8RELAYRK_H */
