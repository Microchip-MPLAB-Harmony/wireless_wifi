#include "definitions.h"
#include "platform_sercom_data.h"

const PLATFORM_USART_PLIB_INTERFACE platformUsartPlibAPI = {
    .initialize             = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_Initialize,
    .readCallbackRegister   = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_ReadCallbackRegister,
    .read                   = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_Read,
    .readIsBusy             = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_ReadIsBusy,
    .readCountGet           = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_ReadCountGet,
    .writeCallbackRegister  = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_WriteCallbackRegister,
    .write                  = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_Write,
    .writeIsBusy            = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_WriteIsBusy,
    .writeCountGet          = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_WriteCountGet,
    .errorGet               = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_ErrorGet,
    .serialSetup            = ${.vars["${APP_USART_PLIB?lower_case}"].USART_PLIB_API_PREFIX}_SerialSetup
};
