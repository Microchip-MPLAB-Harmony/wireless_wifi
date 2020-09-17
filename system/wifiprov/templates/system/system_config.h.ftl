#define SYS_WIFIPROV_NVMADDR        		${SYS_WIFIPROV_NVMADDR}
<#if SYS_WIFIPROV_SAVECONFIG == true>
#define SYS_WIFIPROV_SAVECONFIG        			true
<#else>
#define SYS_WIFIPROV_SAVECONFIG        			false
</#if>
<#--
<#if SYS_WIFIPROV_HTTP == true>
#define SYS_WIFIPROV_HTTPPORT        		${SYS_WIFIPROV_HTTPPORT}
</#if>
-->
<#if SYS_WIFIPROV_SOCKET == true>
#define SYS_WIFIPROV_SOCKETPORT        		${SYS_WIFIPROV_SOCKETPORT}
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
