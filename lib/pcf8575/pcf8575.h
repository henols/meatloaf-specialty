
#ifndef PCF8575_H
#define PCF8575_H

#include <I2Cbus.hpp>

#define I2C_ADDRESS  0x20
#define I2C_SDA  GPIO_NUM_21
#define I2C_SCL  GPIO_NUM_22

/* PCF8575 port bits */
#define P00  0
#define P01  1
#define P02  2
#define P03  3
#define P04  4
#define P05  5
#define P06  6
#define P07  7
#define P10  8
#define P11  9
#define P12  10
#define P13  11
#define P14  12
#define P15  13
#define P16  14
#define P17  15

/**
 * @brief PCF8575 Arduino class
 */
class PCF8575 {
public:
	/**
	 * Create a new PCF8575 instance
	 */
	PCF8575();

	/**
	 * Start the I2C controller and store the PCF8575 chip address
	 */
	void begin(uint8_t address = I2C_ADDRESS, gpio_num_t sda = I2C_SDA, gpio_num_t scl = I2C_SCL);

	/**
	 * Set the direction of a pin (OUTPUT, INPUT or INPUT_PULLUP)
	 * 
	 * @param pin The pin to set
	 * @param mode The new mode of the pin
	 * @remarks INPUT_PULLUP does physicaly the same thing as INPUT (no software pull-up resistors available) but is REQUIRED if you use external pull-up resistor
	 */
	void pinMode(uint8_t pin, uint8_t mode);

	/**
	 * Set the state of a pin (HIGH or LOW)
	 * 
	 * @param pin The pin to set
	 * @param value The new state of the pin
	 * @remarks Software pull-up resistors are not available on the PCF8575
	 */
	void digitalWrite(uint8_t pin, uint8_t value);

	/**
	 * Read the state of a pin
	 * 
	 * @param pin The pin to read back
	 * @return
	 */
	uint8_t digitalRead(uint8_t pin);

	/**
	 * Set the state of all pins in one go
	 * 
	 * @param value The new value of all pins (1 bit = 1 pin, '1' = HIGH, '0' = LOW)
	 */
	void write(uint16_t value);

	/**
	 * Read the state of all pins in one go
	 * 
	 * @return The current value of all pins (1 bit = 1 pin, '1' = HIGH, '0' = LOW)
	 */
	uint16_t read();

	/**
	 * Exactly like write(0x00), set all pins to LOW
	 */
	void clear();

	/**
	 * Exactly like write(0xFF), set all pins to HIGH
	 */
	void set();

	/**
	 * Toggle the state of a pin
	 */
	void toggle(uint8_t pin);

protected:

	I2C_t& myI2C = i2c0;  // i2c0 and i2c1 are the default objects

	/** Current input pins values */
	volatile uint16_t _PIN;

	/** Output pins values */
	volatile uint16_t _POUT;

	/** Pins modes values (OUTPUT or INPUT) */
	volatile uint16_t _DDR;

	/** PCF8575 I2C address */
	uint8_t _address;

	/** 
	 * Read GPIO states and store them in _PIN variable
	 *
	 * @remarks Before reading current GPIO states, current _PIN variable value is moved to _oldPIN variable
	 */
	void readGPIO();

	/** 
	 * Write value of _POUT variable to the GPIO
	 * 
	 * @remarks Only pin marked as OUTPUT are set, for INPUT pins their value are unchanged
	 * @warning To work properly (and avoid any states conflicts) readGPIO() MUST be called before call this function !
	 */
	void updateGPIO();
};

#endif
