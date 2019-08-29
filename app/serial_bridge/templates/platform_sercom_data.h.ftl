#ifndef PLATFORM_HARMONY_INT_SERCOM_DATA_H
#define PLATFORM_HARMONY_INT_SERCOM_DATA_H

#include "peripheral/sercom/usart/plib_sercom_usart_common.h"

#define PLATFORM_USART_INT_SOURCE   ${APP_USART_PLIB}_IRQn
#define PLATFORM_USART_REGS         ${APP_USART_PLIB}_REGS

typedef struct
{
    void (*initialize)(void);
    void (*readCallbackRegister)(SERCOM_USART_CALLBACK callback, uintptr_t context);
    bool (*read)(void *buffer, const size_t size);
    bool (*readIsBusy)(void);
    size_t (*readCountGet)(void);

    void (*writeCallbackRegister)(SERCOM_USART_CALLBACK callback, uintptr_t context);
    bool (*write)(void *buffer, const size_t size);
    bool (*writeIsBusy)(void);
    size_t (*writeCountGet)(void);

    USART_ERROR (*errorGet)(void);
    bool (*serialSetup)(USART_SERIAL_SETUP *serialSetup, uint32_t clkFrequency);
} PLATFORM_USART_PLIB_INTERFACE;

extern const PLATFORM_USART_PLIB_INTERFACE platformUsartPlibAPI;

#endif /* PLATFORM_HARMONY_INT_SERCOM_DATA_H */
