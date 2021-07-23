<#assign PLATFORM_USART_INITIALIZE              = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_Initialize">
<#assign PLATFORM_USART_READCALLBACKREGISTER    = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_ReadCallbackRegister">
<#assign PLATFORM_USART_READ                    = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_Read">
<#assign PLATFORM_USART_READISBUSY              = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_ReadIsBusy">
<#assign PLATFORM_USART_READCOUNTGET            = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_ReadCountGet">
<#assign PLATFORM_USART_WRITECALLBACKREGISTER   = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_WriteCallbackRegister">
<#assign PLATFORM_USART_WRITE                   = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_Write">
<#assign PLATFORM_USART_WRITEISBUSY             = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_WriteIsBusy">
<#assign PLATFORM_USART_WRITECOUNTGET           = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_WriteCountGet">
<#assign PLATFORM_USART_ERRORGET                = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_ErrorGet">
<#assign PLATFORM_USART_SERIALSETUP             = .vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX + "_SerialSetup">
<#assign PLATFORM_USART_REGS                    = "${APP_USART_PLIB}_REGS">
<#assign PLATFORM_USART_PLIB_MULTI_IRQN         = "core.${APP_USART_PLIB}_MULTI_IRQn">
<#assign PLATFORM_USART_PLIB_SINGLE_IRQN        = "core.${APP_USART_PLIB}_SINGLE_IRQn">
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
    <#assign PLATFORM_USART_PLIB_TX_COMPLETE_INDEX  = "core.${APP_USART_PLIB}_USART_TX_COMPLETE_INT_SRC">
    <#assign PLATFORM_USART_PLIB_TX_READY_INDEX     = "core.${APP_USART_PLIB}_USART_TX_READY_INT_SRC">
    <#assign PLATFORM_USART_PLIB_RX_INDEX           = "core.${APP_USART_PLIB}_USART_RX_INT_SRC">
    <#assign PLATFORM_USART_PLIB_ERROR_INDEX        = "core.${APP_USART_PLIB}_USART_ERROR_INT_SRC">
</#if>
/**
 *
 * Copyright (c) 2019 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "platform/platform.h"

#include "definitions.h"

#include "osal/osal.h"
#include "peripheral/sercom/usart/plib_sercom_usart_common.h"

#define USART_RECEIVE_BUFFER_SIZE   512
#define USART_TRANSMIT_BUFFER_SIZE  512

static USART_SERIAL_SETUP sercomSetup;
static OSAL_MUTEX_HANDLE_TYPE usartWriteMutex;

static uint8_t usartReceiveBuffer[USART_RECEIVE_BUFFER_SIZE];
static size_t usartReceiveInOffset;
static size_t usartReceiveOutOffset;
static volatile size_t usartReceiveLength;

static char usartTransmitBuffer[USART_TRANSMIT_BUFFER_SIZE];
static size_t usartTransmitInOffset;
static size_t usartTransmitOutOffset;
static volatile size_t usartTransmitLength;

static void _USARTReceiveEventHandler(uintptr_t context)
{
    usartReceiveInOffset++;
    usartReceiveLength++;

    if (USART_RECEIVE_BUFFER_SIZE == usartReceiveInOffset)
    {
        usartReceiveInOffset = 0;
    }

    ${PLATFORM_USART_READ}(&usartReceiveBuffer[usartReceiveInOffset], 1);
}

static void _USARTTransmitEventHandler(uintptr_t context)
{
    usartTransmitOutOffset++;
    usartTransmitLength--;

    if (USART_TRANSMIT_BUFFER_SIZE == usartTransmitOutOffset)
    {
        usartTransmitOutOffset = 0;
    }

    if (usartTransmitLength > 0)
    {
        ${PLATFORM_USART_WRITE}(&usartTransmitBuffer[usartTransmitOutOffset], 1);
    }
}

static void _USARTInit(uint32_t baud)
{
    sercomSetup.baudRate    = baud;
    sercomSetup.dataWidth   = USART_DATA_8_BIT;
    sercomSetup.parity      = USART_PARITY_NONE;
    sercomSetup.stopBits    = USART_STOP_1_BIT;

    ${PLATFORM_USART_SERIALSETUP}(&sercomSetup, 0);

    ${PLATFORM_USART_WRITECALLBACKREGISTER}(&_USARTTransmitEventHandler, 0);
    ${PLATFORM_USART_READCALLBACKREGISTER}(&_USARTReceiveEventHandler, 0);

    usartTransmitInOffset  = 0;
    usartTransmitOutOffset = 0;
    usartTransmitLength    = 0;

    usartReceiveInOffset   = 0;
    usartReceiveOutOffset  = 0;
    usartReceiveLength     = 0;

    ${PLATFORM_USART_READ}(&usartReceiveBuffer[usartReceiveInOffset], 1);
}

void SerialBridge_PlatformInit(void)
{
    OSAL_MUTEX_Create(&usartWriteMutex);

    _USARTInit(115200);
}

void SerialBridge_PlatformUARTSetBaudRate(uint32_t baud)
{
    if (sercomSetup.baudRate == baud)
    {
        return;
    }

    if (OSAL_RESULT_TRUE == OSAL_MUTEX_Lock(&usartWriteMutex, OSAL_WAIT_FOREVER))
    {
        while (-1)
        {
            if (false == ${PLATFORM_USART_WRITEISBUSY}())
            {
                if (0 == usartTransmitLength)
                {
                    break;
                }

                ${PLATFORM_USART_WRITE}(&usartTransmitBuffer[usartTransmitOutOffset], 1);
            }
        }

        while (0 == (${PLATFORM_USART_REGS}->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_TXC_Msk))
        {
        }

        OSAL_MUTEX_Unlock(&usartWriteMutex);
    }

    ${PLATFORM_USART_INITIALIZE}();

    _USARTInit(baud);
}

size_t SerialBridge_PlatformUARTReadGetCount(void)
{
    size_t count;
    bool usartInterruptStatus = false;

    /* Disable USART interrupts */
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
    <#if PLATFORM_USART_PLIB_RX_INDEX?eval??>
    usartInterruptStatus = SYS_INT_SourceDisable(${PLATFORM_USART_PLIB_RX_INDEX?eval});
    </#if>
<#else>
    <#if PLATFORM_USART_PLIB_SINGLE_IRQN?eval??>
    usartInterruptStatus = SYS_INT_SourceDisable(${PLATFORM_USART_PLIB_SINGLE_IRQN?eval});
    <#else>
    usartInterruptStatus = SYS_INT_SourceDisable(${APP_USART_PLIB}_IRQn);
    </#if>
</#if>

    count = usartReceiveLength;

    if (true == usartInterruptStatus)
    {
        /* Enable USART interrupts */
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
    <#if PLATFORM_USART_PLIB_RX_INDEX?eval??>
        SYS_INT_SourceEnable(${PLATFORM_USART_PLIB_RX_INDEX?eval});
    </#if>
<#else>
    <#if PLATFORM_USART_PLIB_SINGLE_IRQN?eval??>
        SYS_INT_SourceEnable(${PLATFORM_USART_PLIB_SINGLE_IRQN?eval});
    <#else>
        SYS_INT_SourceEnable(${APP_USART_PLIB}_IRQn);
    </#if>
</#if>
    }

    return count;
}

uint8_t SerialBridge_PlatformUARTReadGetByte(void)
{
    uint8_t byte = 0;

    if (0 == SerialBridge_PlatformUARTReadGetBuffer(&byte, 1))
    {
        return 0;
    }

    return byte;
}

size_t SerialBridge_PlatformUARTReadGetBuffer(void *pBuf, size_t numBytes)
{
    bool usartInterruptStatus = false;

    size_t count = SerialBridge_PlatformUARTReadGetCount();

    if (0 == count)
    {
        return 0;
    }

    if (numBytes > count)
    {
        numBytes = count;
    }

    if ((usartReceiveOutOffset + numBytes) > USART_RECEIVE_BUFFER_SIZE)
    {
        uint8_t *pByteBuf;
        size_t partialReadNum;

        pByteBuf = pBuf;
        partialReadNum = (USART_RECEIVE_BUFFER_SIZE - usartReceiveOutOffset);

        memcpy(pByteBuf, &usartReceiveBuffer[usartReceiveOutOffset], partialReadNum);

        pByteBuf += partialReadNum;
        numBytes -= partialReadNum;

        memcpy(pByteBuf, usartReceiveBuffer, numBytes);

        usartReceiveOutOffset = numBytes;

        numBytes += partialReadNum;
    }
    else
    {
        memcpy(pBuf, &usartReceiveBuffer[usartReceiveOutOffset], numBytes);

        usartReceiveOutOffset += numBytes;
    }

    /* Disable USART interrupts */
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
    <#if PLATFORM_USART_PLIB_RX_INDEX?eval??>
    usartInterruptStatus = SYS_INT_SourceDisable(${PLATFORM_USART_PLIB_RX_INDEX?eval});
    </#if>
<#else>
    <#if PLATFORM_USART_PLIB_SINGLE_IRQN?eval??>
    usartInterruptStatus = SYS_INT_SourceDisable(${PLATFORM_USART_PLIB_SINGLE_IRQN?eval});
    <#else>
    usartInterruptStatus = SYS_INT_SourceDisable(${APP_USART_PLIB}_IRQn);
    </#if>
</#if>

    usartReceiveLength -= numBytes;

    if (true == usartInterruptStatus)
    {
        /* Enable USART interrupts */
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
    <#if PLATFORM_USART_PLIB_RX_INDEX?eval??>
        SYS_INT_SourceEnable(${PLATFORM_USART_PLIB_RX_INDEX?eval});
    </#if>
<#else>
    <#if PLATFORM_USART_PLIB_SINGLE_IRQN?eval??>
        SYS_INT_SourceEnable(${PLATFORM_USART_PLIB_SINGLE_IRQN?eval});
    <#else>
        SYS_INT_SourceEnable(${APP_USART_PLIB}_IRQn);
    </#if>
</#if>
    }

    return numBytes;
}

size_t SerialBridge_PlatformUARTWriteGetCount(void)
{
    size_t count;
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
    bool usartTxReadyIntStatus    = false;
    bool usartTxCompleteIntStatus = false;
<#else>
    bool usartInterruptStatus = false;
</#if>

    /* Disable USART interrupts */
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
    <#if PLATFORM_USART_PLIB_TX_READY_INDEX?eval??>
    usartTxReadyIntStatus = SYS_INT_SourceDisable(${PLATFORM_USART_PLIB_TX_READY_INDEX?eval});
    </#if>
    <#if PLATFORM_USART_PLIB_TX_COMPLETE_INDEX?eval??>
    usartTxCompleteIntStatus = SYS_INT_SourceDisable(${PLATFORM_USART_PLIB_TX_COMPLETE_INDEX?eval});
    </#if>
<#else>
    <#if PLATFORM_USART_PLIB_SINGLE_IRQN?eval??>
    usartInterruptStatus = SYS_INT_SourceDisable(${PLATFORM_USART_PLIB_SINGLE_IRQN?eval});
    <#else>
    usartInterruptStatus = SYS_INT_SourceDisable(${APP_USART_PLIB}_IRQn);
    </#if>
</#if>

    count = usartTransmitLength;

    /* Enable USART interrupts */
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
    if (true == usartTxReadyIntStatus)
    {
    <#if PLATFORM_USART_PLIB_TX_READY_INDEX?eval??>
        SYS_INT_SourceEnable(${PLATFORM_USART_PLIB_TX_READY_INDEX?eval});
    </#if>
    }

    if (true == usartTxCompleteIntStatus)
    {
    <#if PLATFORM_USART_PLIB_TX_COMPLETE_INDEX?eval??>
        SYS_INT_SourceEnable(${PLATFORM_USART_PLIB_TX_COMPLETE_INDEX?eval});
    </#if>
    }
<#else>
    if (true == usartInterruptStatus)
    {
    <#if PLATFORM_USART_PLIB_SINGLE_IRQN?eval??>
        SYS_INT_SourceEnable(${PLATFORM_USART_PLIB_SINGLE_IRQN?eval});
    <#else>
        SYS_INT_SourceEnable(${APP_USART_PLIB}_IRQn);
    </#if>
    }
</#if>

    return count;
}

bool SerialBridge_PlatformUARTWritePutByte(uint8_t b)
{
    return SerialBridge_PlatformUARTWritePutBuffer((void*)&b, 1);
}

bool SerialBridge_PlatformUARTWritePutBuffer(const void *pBuf, size_t numBytes)
{
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
    bool usartTxReadyIntStatus    = false;
    bool usartTxCompleteIntStatus = false;
<#else>
    bool usartInterruptStatus = false;
</#if>

    if ((NULL == pBuf) || (0 == numBytes))
    {
        return false;
    }

    if (OSAL_RESULT_TRUE == OSAL_MUTEX_Lock(&usartWriteMutex, OSAL_WAIT_FOREVER))
    {
        if ((SerialBridge_PlatformUARTWriteGetCount() + numBytes) > USART_TRANSMIT_BUFFER_SIZE)
        {
            OSAL_MUTEX_Unlock(&usartWriteMutex);
            return false;
        }

        if ((usartTransmitInOffset + numBytes) > USART_TRANSMIT_BUFFER_SIZE)
        {
            uint8_t *pByteBuf;
            size_t partialWriteNum;

            pByteBuf = (uint8_t*)pBuf;

            partialWriteNum = USART_TRANSMIT_BUFFER_SIZE - usartTransmitInOffset;

            memcpy(&usartTransmitBuffer[usartTransmitInOffset], pByteBuf, partialWriteNum);

            pByteBuf += partialWriteNum;
            numBytes -= partialWriteNum;

            memcpy(usartTransmitBuffer, pByteBuf, numBytes);

            usartTransmitInOffset = numBytes;

            numBytes += partialWriteNum;
        }
        else
        {
            memcpy(&usartTransmitBuffer[usartTransmitInOffset], pBuf, numBytes);

            usartTransmitInOffset += numBytes;
        }

        /* Disable USART interrupts */
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
    <#if PLATFORM_USART_PLIB_TX_READY_INDEX?eval??>
        usartTxReadyIntStatus = SYS_INT_SourceDisable(${PLATFORM_USART_PLIB_TX_READY_INDEX?eval});
    </#if>

    <#if PLATFORM_USART_PLIB_TX_COMPLETE_INDEX?eval??>
        usartTxCompleteIntStatus = SYS_INT_SourceDisable(${PLATFORM_USART_PLIB_TX_COMPLETE_INDEX?eval});
    </#if>
<#else>
    <#if PLATFORM_USART_PLIB_SINGLE_IRQN?eval??>
        usartInterruptStatus = SYS_INT_SourceDisable(${PLATFORM_USART_PLIB_SINGLE_IRQN?eval});
    <#else>
        usartInterruptStatus = SYS_INT_SourceDisable(${APP_USART_PLIB}_IRQn);
    </#if>
</#if>

        usartTransmitLength += numBytes;

        /* Enable USART interrupts */
<#if PLATFORM_USART_PLIB_MULTI_IRQN?eval??>
        if (true == usartTxReadyIntStatus)
        {
    <#if PLATFORM_USART_PLIB_TX_READY_INDEX?eval??>
            SYS_INT_SourceEnable(${PLATFORM_USART_PLIB_TX_READY_INDEX?eval});
    </#if>
        }

        if (true == usartTxCompleteIntStatus)
        {
    <#if PLATFORM_USART_PLIB_TX_COMPLETE_INDEX?eval??>
            SYS_INT_SourceEnable(${PLATFORM_USART_PLIB_TX_COMPLETE_INDEX?eval});
    </#if>
        }
<#else>
        if (true == usartInterruptStatus)
        {
    <#if PLATFORM_USART_PLIB_SINGLE_IRQN?eval??>
            SYS_INT_SourceEnable(${PLATFORM_USART_PLIB_SINGLE_IRQN?eval});
    <#else>
            SYS_INT_SourceEnable(${APP_USART_PLIB}_IRQn);
    </#if>
        }
</#if>

        if (false == ${PLATFORM_USART_WRITEISBUSY}())
        {
            ${PLATFORM_USART_WRITE}(&usartTransmitBuffer[usartTransmitOutOffset], 1);
        }

        OSAL_MUTEX_Unlock(&usartWriteMutex);
    }

    return true;
}
