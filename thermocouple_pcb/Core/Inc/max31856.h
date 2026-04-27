/*
 * max31856.h
 *
 *  Created on: 27. apr. 2026
 *      Author: joaki
 */

#ifndef MAX31856_H
#define MAX31856_H

#include "stm32g0xx_hal.h"

/* SPI read/write functions */
void MAX31856_ReadBytes(uint8_t cs_port_pin,
                        uint8_t startReg,
                        uint8_t *buf,
                        uint16_t len,
                        GPIO_TypeDef *cs_port,
                        uint16_t cs_pin);

void MAX31856_WriteRegister(GPIO_TypeDef *cs_port,
                            uint16_t cs_pin,
                            uint8_t reg,
                            uint8_t value);

void MAX31856_Init(GPIO_TypeDef *cs_port, uint16_t cs_pin);
float MAX31856_ReadThermocouple(GPIO_TypeDef *cs_port, uint16_t cs_pin);
float MAX31856_ReadColdJunction(GPIO_TypeDef *cs_port, uint16_t cs_pin);
uint8_t MAX31856_ReadFault(GPIO_TypeDef *cs_port, uint16_t cs_pin);

#endif /* MAX31856_H */
