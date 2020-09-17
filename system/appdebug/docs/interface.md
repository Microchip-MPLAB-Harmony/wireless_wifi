---
grand_parent: Services
parent: App Debug Service
title: App Debug System Service Interface
has_toc: true
nav_order: 2
---

# App Debug System Service Interface
{: .no_toc }

### Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---


## Data Types and Constants Summary

| Name | Description |
|-|-|
| [APP_LOG_LVL_DISABLE](#APP_LOG_LVL_DISABLE) | App Debug Service Logging Disabled |
| [APP_LOG_ERROR_LVL](#APP_LOG_ERROR_LVL) | App Debug Service Error Log Level |
| [APP_LOG_DBG_LVL](#APP_LOG_DBG_LVL) | App Debug Service Debug Log Level |
| [APP_LOG_INFO_LVL](#APP_LOG_INFO_LVL) | App Debug Service Info Log Level |
| [APP_LOG_FN_EE_LVL](#APP_LOG_FN_EE_LVL) | App Debug Service Service Entry/ Exit Log Level |
| [SYS_APPDEBUG_MAX_NUM_OF_USERS](#SYS_APPDEBUG_MAX_NUM_OF_USERS) | Number of instances of App Debug Service supported |
| [SYS_APPDEBUG_CONFIG](#SYS_APPDEBUG_CONFIG) | Defines the data required to initialize the app debug system service. |
| [SYS_APPDEBUG_CtrlMsgType](#SYS_APPDEBUG_CtrlMsgType) | Identifies the control message for which the User has called the SYS_APPDEBUG_CtrlMsg(). |
| [SYS_APPDEBUG_RESULT](#SYS_APPDEBUG_RESULT) | Identifies the return values for the Sys App Debug APIs. |

## Initialization functions Summary

| Name | Description |
|-|-|
| [SYS_APPDEBUG_Initialize](#SYS_APPDEBUG_Initialize) | Returns success/ failure for initialization of data structures of the |
| [SYS_APPDEBUG_Deinitialize](#SYS_APPDEBUG_Deinitialize) | Returns success/ failure for deinitialization of data structures of the |

## Setup functions Summary

| Name | Description |
|-|-|
| [SYS_APPDEBUG_Open](#SYS_APPDEBUG_Open) | Open an instance of the System App Debug service. |
| [SYS_APPDEBUG_Close](#SYS_APPDEBUG_Close) | Close the specific module instance of the SYS App Debug service |
| [SYS_APPDEBUG_CtrlMsg](#SYS_APPDEBUG_CtrlMsg) | Returns success/ failure for the flow/ level set operation asked by the user. |
| [SYS_APPDEBUG_ERR_PRINT](#SYS_APPDEBUG_ERR_PRINT) | Used for logging Error Level Logs |
| [SYS_APPDEBUG_DBG_PRINT](#SYS_APPDEBUG_DBG_PRINT) | Used for logging Debug Level Logs |
| [SYS_APPDEBUG_INFO_PRINT](#SYS_APPDEBUG_INFO_PRINT) | Used for logging Info Level Logs |
| [SYS_APPDEBUG_FN_ENTER_PRINT](#SYS_APPDEBUG_FN_ENTER_PRINT) | Used for logging Function Entry Logs |
| [SYS_APPDEBUG_FN_EXIT_PRINT](#SYS_APPDEBUG_FN_EXIT_PRINT) | Used for logging Function Exit Logs |

## Data Types and Constants


### APP_LOG_LVL_DISABLE

**Summary**

App Debug Service Logging Disabled  

**Remarks**

None. 

```c
#define APP_LOG_LVL_DISABLE 	0x0
```

### APP_LOG_ERROR_LVL


**Summary**

App Debug Service Error Log Level  

**Remarks**

None. 

```c
#define APP_LOG_ERROR_LVL 		0x1
```

### APP_LOG_DBG_LVL


**Summary**

App Debug Service Debug Log Level  

**Remarks**

None. 

```c
#define APP_LOG_DBG_LVL 		0x2
```

### APP_LOG_INFO_LVL


**Summary**

App Debug Service Info Log Level  

**Remarks**

None. 

```c
#define APP_LOG_INFO_LVL 		0x4
```

### APP_LOG_FN_EE_LVL


**Summary**

App Debug Service Service Entry/ Exit Log Level  

**Remarks**

None. 

```c
#define APP_LOG_FN_EE_LVL 		0x8
```

### SYS_APPDEBUG_MAX_NUM_OF_USERS


**Summary**

Number of instances of App Debug Service supported  

**Remarks**

None. 

```c
#define SYS_APPDEBUG_MAX_NUM_OF_USERS 8
```

### SYS_APPDEBUG_CONFIG


**Summary**

Defines the data required to initialize the app debug system service.  

**Description**

This structure defines the data required to initialize the app debug system service.  

**Remarks**

None. 

```c
typedef struct
{
/* Initial system Log level setting. */
unsigned int logLevel;

/* Initial system Log level setting. */
unsigned int logFlow;

/* Initial system Log level setting. */
const char *prefixString;
} SYS_APPDEBUG_CONFIG;
```

### SYS_APPDEBUG_CtrlMsgType


**Summary**

Identifies the control message for which the User has called the SYS_APPDEBUG_CtrlMsg().  

**Remarks**

None. 

```c
typedef enum {
SYS_APPDEBUG_CTRL_MSG_TYPE_SET_LEVEL,
SYS_APPDEBUG_CTRL_MSG_TYPE_SET_FLOW,
} SYS_APPDEBUG_CtrlMsgType;
```

### SYS_APPDEBUG_RESULT


**Summary**

Identifies the return values for the Sys App Debug APIs.  

**Remarks**

None. 

```c
typedef enum {
SYS_APPDEBUG_SUCCESS = 0,		// Success
SYS_APPDEBUG_FAILURE = -1, 	// Failure
} SYS_APPDEBUG_RESULT;
```

## Initialization functions


### SYS_APPDEBUG_Initialize

**Function**

```c
int32_t SYS_APPDEBUG_Initialize()
```

**Summary**

Returns success/ failure for initialization of data structures of the App Debug service  

**Description**

This function is used for initializing the data structures of the App Debug service and is called from within the System Task.  

**Parameters**

index 		- NULL; reserved for future use<br> init 			- NULL; reserved for future use<br>  

**Returns**

*SYS_APPDEBUG_SUCCESS* - Indicates the data structures were initialized successfully 

*SYS_APPDEBUG_FAILURE* - Indicates that it failed to initialize the data structures  

**Example**

```c
if( SYS_APPDEBUG_Initialize(NULL, NULL) == SYS_APPDEBUG_SUCCESS)
{
}
```

**Remarks**

If the Net system service is enabled using MHC, then auto generated code will take care of system task execution. 

### SYS_APPDEBUG_Deinitialize

**Function**

```c
int32_t SYS_APPDEBUG_Deinitialize()
```

**Summary**

Returns success/ failure for deinitialization of data structures of the App Debug service  

**Description**

This function is used for deinitializing the data structures of the App Debug service and is called from within the System Task.  

**Parameters**

None<br>  

**Returns**

*SYS_APPDEBUG_SUCCESS* - Indicates the data structures were deinitialized successfully 

*SYS_APPDEBUG_FAILURE* - Indicates that it failed to deinitialize the data structures.  

**Example**

```c
if( SYS_APPDEBUG_Deinitialize() == SYS_APPDEBUG_SUCCESS)
{
}
```

**Remarks**

If the Net system service is enabled using MHC, then auto generated code will take care of system task execution. 
## Setup functions




### SYS_APPDEBUG_Open

**Function**

```c
SYS_MODULE_OBJ SYS_APPDEBUG_Open (SYS_APPDEBUG_CONFIG *cfg)
```

**Summary**

Open an instance of the System App Debug service.  

**Description**

This function initializes the instance of the System App Debug Service.  

**Parameters**

cfg 		- Configuration with which the App Debug Service needs to be opened<br>  **Returns**

Returns:If successful, returns a valid handle to an object. Otherwise, it
returns SYS_MODULE_OBJ_INVALID.

**Example**

```c
SYS_APPDEBUG_CONFIG 	g_AppDebugServCfg;
SYS_MODULE_OBJ 		g_AppDebugServHandle;

memset(&g_AppDebugServCfg, 0, sizeof(g_AppDebugServCfg));
g_AppDebugServCfg.logLevel |= APP_LOG_ERROR_LVL;
g_AppDebugServCfg.prefixString = "MY_APP";
g_AppDebugServCfg.logFlow |= 0x1;

g_AppDebugServHandle = SYS_NET_Open(&g_AppDebugServCfg);
if (g_AppDebugServHandle == SYS_MODULE_OBJ_INVALID)
{
// Handle error
}
```

**Remarks**

This routine should be called everytime a user wants to open a new NET socket 

### SYS_APPDEBUG_Close

**Function**

```c
void SYS_APPDEBUG_Close ( SYS_MODULE_OBJ object )
```

**Summary**

Close the specific module instance of the SYS App Debug service  

**Description**

This function clsoes the specific module instance disabling its operation. Resets all of the internal data structures and fields for the specified instance to the default settings.  

**Precondition**

The SYS_APPDEBUG_Open function should have been called before calling this function.  

**Parameters**

*object* - SYS App Debug object handle, returned from SYS_APPDEBUG_Open<br>  

**Returns**

None.  

**Example**

```c
// Handle "objSysAppDebug" value must have been returned from SYS_APPDEBUG_Open.

SYS_APPDEBUG_Close (objSysAppDebug);
```

**Remarks**

Once the Open operation has been called, the Close operation must be called before the Open operation can be called again. 

### SYS_APPDEBUG_CtrlMsg

**Function**

```c
int32_t SYS_APPDEBUG_CtrlMsg(SYS_MODULE_OBJ obj,
SYS_APPDEBUG_CtrlMsgType eCtrlMsgType, void *data, uint16_t len)
```

**Summary**

Returns success/ failure for the flow/ level set operation asked by the user.  

**Description**

This function is used for setting the value of floe/ level for the app debug logs.  

**Precondition**

SYS_APPDEBUG_Open should have been called.  

**Parameters**

obj 	- SYS App Debug object handle, returned from SYS_APPDEBUG_Open<br> 
*eCtrlMsgType* - valid Msg Type data		- valid data buffer pointer based on the Msg Type<br> len		- length of the data buffer the pointer is pointing to<br>  

**Returns**

*SYS_APPDEBUG_SUCCESS* - Indicates that the Request was catered to successfully 

*SYS_APPDEBUG_FAILURE* - Indicates that the Request failed  

**Example**

```c
// Handle "objSysAppDebug" value must have been returned from SYS_APPDEBUG_Open.
uint32_t logLevel = 0x3;
if( SYS_APPDEBUG_CtrlMsg(objSysAppDebug, SYS_APPDEBUG_CTRL_MSG_TYPE_SET_LEVEL, &logLevel, 4) == SYS_APPDEBUG_SUCCESS)
{
}
```

**Remarks**

None. 

```c
uint32_t linenum,
char *msg, ...);
void SYS_APPDEBUG_PRINT_FN_ENTER(SYS_MODULE_OBJ obj,
uint32_t flow,
const char *function,
uint32_t linenum);
void SYS_APPDEBUG_PRINT_FN_EXIT(SYS_MODULE_OBJ obj,
uint32_t flow,
const char *function,
uint32_t linenum);

```

### SYS_APPDEBUG_ERR_PRINT

**Summary**

Used for logging Error Level Logs  

**Description**

This macro function is used for logging error level logs.  

**Precondition**

SYS_APPDEBUG_Open should have been called.  

**Parameters**

obj 	- SYS App Debug object handle, returned from SYS_APPDEBUG_Open<br> flow 	- valid flow defined by the User, log will come only if this flow is enabled<br> data		- valid string<br> ...		- any variable arguments if present<br>  

**Returns**

None.  

**Example**

```c
// Handle "objSysAppDebug" value must have been returned from SYS_APPDEBUG_Open.
SYS_APPDEBUG_ERR_PRINT(objSysAppDebug, MY_APP_FLOW_DATA, "Failed to allocate memory of size %d", size);
```

**Remarks**

None. 

#define SYS_APPDEBUG_ERR_PRINT(obj, flow, fmt, ...) \
SYS_APPDEBUG_PRINT(obj, flow, APP_LOG_ERROR_LVL, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)


### SYS_APPDEBUG_DBG_PRINT

**Summary**

Used for logging Debug Level Logs  

**Description**

This macro function is used for logging debug level logs.  

**Precondition**

SYS_APPDEBUG_Open should have been called.  

**Parameters**

obj 	- SYS App Debug object handle, returned from SYS_APPDEBUG_Open<br> flow 	- valid flow defined by the User, log will come only if this flow is enabled<br> data		- valid string<br> ...		- any variable arguments if present<br>  

**Returns**

None.  

**Example**

```c
// Handle "objSysAppDebug" value must have been returned from SYS_APPDEBUG_Open.
SYS_APPDEBUG_DBG_PRINT(objSysAppDebug, MY_APP_FLOW_DATA, "memory allocation reached Threshold");
```

**Remarks**

None. 

#define SYS_APPDEBUG_DBG_PRINT(obj, flow, fmt, ...) \
SYS_APPDEBUG_PRINT(obj, flow, APP_LOG_DBG_LVL, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)


### SYS_APPDEBUG_INFO_PRINT

**Summary**

Used for logging Info Level Logs  

**Description**

This macro function is used for logging info level logs.  

**Precondition**

SYS_APPDEBUG_Open should have been called.  

**Parameters**

obj 	- SYS App Debug object handle, returned from SYS_APPDEBUG_Open<br> flow 	- valid flow defined by the User, log will come only if this flow is enabled<br> data		- valid string<br> ...		- any variable arguments if present<br>  

**Returns**

None.  

**Example**

```c
// Handle "objSysAppDebug" value must have been returned from SYS_APPDEBUG_Open.
SYS_APPDEBUG_INFO_PRINT(objSysAppDebug, MY_APP_FLOW_DATA, "Allocate memory of size %d", size);
```

**Remarks**

None. 

#define SYS_APPDEBUG_INFO_PRINT(obj, flow, fmt, ...) \
SYS_APPDEBUG_PRINT(obj, flow, APP_LOG_INFO_LVL, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)


### SYS_APPDEBUG_FN_ENTER_PRINT

**Summary**

Used for logging Function Entry Logs  

**Description**

This macro function is used for logging function entry level logs.  

**Precondition**

SYS_APPDEBUG_Open should have been called.  

**Parameters**

obj 	- SYS App Debug object handle, returned from SYS_APPDEBUG_Open<br> flow 	- valid flow defined by the User, log will come only if this flow is enabled<br> data		- valid string<br> ...		- any variable arguments if present<br>  

**Returns**

None.  

**Example**

```c
// Handle "objSysAppDebug" value must have been returned from SYS_APPDEBUG_Open.
SYS_APPDEBUG_FN_ENTER_PRINT(objSysAppDebug, MY_APP_FLOW_DATA);
```

**Remarks**

None. 

#define SYS_APPDEBUG_FN_ENTER_PRINT(obj, flow) \
SYS_APPDEBUG_PRINT_FN_ENTER(obj, flow, __FUNCTION__, __LINE__)


### SYS_APPDEBUG_FN_EXIT_PRINT

**Summary**

Used for logging Function Exit Logs  

**Description**

This macro function is used for logging function exit level logs.  

**Precondition**

SYS_APPDEBUG_Open should have been called.  

**Parameters**

obj 	- SYS App Debug object handle, returned from SYS_APPDEBUG_Open<br> flow 	- valid flow defined by the User, log will come only if this flow is enabled<br> data		- valid string<br> ...		- any variable arguments if present<br>  

**Returns**

None.  

**Example**

```c
// Handle "objSysAppDebug" value must have been returned from SYS_APPDEBUG_Open.
SYS_APPDEBUG_FN_EXIT_PRINT(objSysAppDebug, MY_APP_FLOW_DATA);
```

**Remarks**

None. 

#define SYS_APPDEBUG_FN_EXIT_PRINT(obj, flow) \
SYS_APPDEBUG_PRINT_FN_EXIT(obj, flow, __FUNCTION__, __LINE__)

