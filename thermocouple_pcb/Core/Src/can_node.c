/*
 * can_node.c
 *
 *  Created on: 27. apr. 2026
 *      Author: joakim
 */


#include "can_node.h"
#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;

void CAN_SendTemperature(uint16_t can_id,
                         float tc_temp,
                         float cj_temp,
                         float pcb_temp,
                         uint8_t fault)
{
    FDCAN_TxHeaderTypeDef TxHeader;
    uint8_t TxData[8] = {0};

    /* Scale temperatures to int16 (°C × 10) */
    int16_t tc  = (int16_t)(tc_temp  * CAN_TEMP_SCALE);
    int16_t cj  = (int16_t)(cj_temp  * CAN_TEMP_SCALE);
    int16_t pcb = (int16_t)(pcb_temp * CAN_TEMP_SCALE);

    /* Pack into 8 bytes big-endian */
    TxData[0] = (uint8_t)(tc  >> 8);
    TxData[1] = (uint8_t)(tc  & 0xFF);
    TxData[2] = (uint8_t)(cj  >> 8);
    TxData[3] = (uint8_t)(cj  & 0xFF);
    TxData[4] = (uint8_t)(pcb >> 8);
    TxData[5] = (uint8_t)(pcb & 0xFF);
    TxData[6] = fault;
    TxData[7] = 0x00;

    TxHeader.Identifier          = can_id;
    TxHeader.IdType              = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType         = FDCAN_DATA_FRAME;
    TxHeader.DataLength          = FDCAN_DLC_BYTES_8;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch       = FDCAN_BRS_OFF;
    TxHeader.FDFormat            = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker       = 0;

    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData);
}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(fdcanHandle->Instance == FDCAN1)
    {
        __HAL_RCC_FDCAN_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* PA11 = FDCAN1_RX, PA12 = FDCAN1_TX */
        GPIO_InitStruct.Pin       = GPIO_PIN_11 | GPIO_PIN_12;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
        HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
    }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{
    if(fdcanHandle->Instance == FDCAN1)
    {
        __HAL_RCC_FDCAN_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);
        HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
    }
}
