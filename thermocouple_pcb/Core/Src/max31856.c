/*
 * max31856.c
 *
 *  Created on: 27. apr. 2026
 *      Author: joaki
 */


#include "max31856.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

static void SPI_ReadBytes(GPIO_TypeDef *cs_port,
                          uint16_t cs_pin,
                          uint8_t startReg,
                          uint8_t *buf,
                          uint16_t len)
{
    uint8_t addr = startReg & 0x7F;
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &addr, 1, 100);
    HAL_SPI_Receive(&hspi1, buf, len, 100);
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
}

void MAX31856_WriteRegister(GPIO_TypeDef *cs_port,
                            uint16_t cs_pin,
                            uint8_t reg,
                            uint8_t value)
{
    uint8_t tx[2];
    tx[0] = reg | 0x80;
    tx[1] = value;
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx, 2, 100);
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
}

void MAX31856_Init(GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
    HAL_Delay(50);

    /* Dummy read to wake up SPI — first transaction after power-up
     * can be lost otherwise, causing the Type N config write to fail */
    uint8_t dummy;
    SPI_ReadBytes(cs_port, cs_pin, 0x00, &dummy, 1);
    HAL_Delay(10);

    /* Write CR0 before CR1 as per MAX31856 datasheet */
    MAX31856_WriteRegister(cs_port, cs_pin, MAX31856_CR0_REG, 0x80);
    HAL_Delay(10);
    MAX31856_WriteRegister(cs_port, cs_pin, MAX31856_CR1_REG,
                           MAX31856_TC_TYPE_N);
    HAL_Delay(10);
}

float MAX31856_ReadThermocouple(GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    uint8_t b[3];
    SPI_ReadBytes(cs_port, cs_pin, 0x0C, b, 3);
    int32_t raw = ((int32_t)b[0] << 16) |
                  ((int32_t)b[1] << 8)  |
                  b[2];
    raw >>= 5;
    if (raw & (1 << 18))
        raw |= ~((1 << 19) - 1);
    return raw * 0.0078125f;
}

float MAX31856_ReadColdJunction(GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    uint8_t b[2];
    SPI_ReadBytes(cs_port, cs_pin, 0x0A, b, 2);
    int16_t raw = (int16_t)((b[0] << 8) | b[1]);
    raw >>= 2;
    return raw * 0.015625f;
}

uint8_t MAX31856_ReadFault(GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    uint8_t fault;
    SPI_ReadBytes(cs_port, cs_pin, 0x0F, &fault, 1);
    return fault;
}
