#define SYS_MQTT_PAHO

#define SYS_MQTT_INDEX0_MQTT_PORT        				${SYS_MQTT_MQTT_PORT}
#define SYS_MQTT_INDEX0_BROKER_NAME        				"${SYS_MQTT_BROKER_NAME}"
#define SYS_MQTT_INDEX0_ENABLE_TLS        				${SYS_MQTT_ENABLE_TLS?string('true', 'false')}
#define SYS_MQTT_INDEX0_RECONNECT        				${SYS_MQTT_RECONNECT?string('true', 'false')}
#define SYS_MQTT_INDEX0_CLIENT_ID        				"${SYS_MQTT_CLIENT_ID}"
#define SYS_MQTT_INDEX0_KEEPALIVE_INTERVAL 				${SYS_MQTT_KEEPALIVE_INTERVAL}

<#if SYS_MQTT_SUB_ENABLE == true>
<#if SYS_MQTT_SUB_QOS == "At most once (0)">
#define SYS_MQTT_INDEX0_SUB_QOS							0
<#elseif SYS_MQTT_SUB_QOS == "At least once (1)">
#define SYS_MQTT_INDEX0_SUB_QOS							1
<#elseif SYS_MQTT_SUB_QOS == "Exactly once (2)">
#define SYS_MQTT_INDEX0_SUB_QOS							2
</#if>
#define SYS_MQTT_INDEX0_SUB_TOPIC_COUNT					1
#define SYS_MQTT_INDEX0_TOPIC_NAME        				"${SYS_MQTT_SUB_TOPIC_NAME}"
#define SYS_MQTT_INDEX0_ENTRY_VALID        				true
<#else>
#define SYS_MQTT_INDEX0_SUB_TOPIC_COUNT					0
#define SYS_MQTT_INDEX0_TOPIC_NAME        				" "
#define SYS_MQTT_INDEX0_SUB_QOS							0
#define SYS_MQTT_INDEX0_ENTRY_VALID        				false
</#if>

<#if SYS_MQTT_APPDEBUG_ENABLE == true>
#define SYS_MQTT_ENABLE_DEBUG_PRINT						
#define SYS_MQTT_DEBUG_PRESTR						"${SYS_MQTT_APPDEBUG_PRESTR}"

<#if SYS_MQTT_APPDEBUG_ERR_LEVEL == true>
#define SYS_MQTT_APPDEBUG_ERR_LEVEL_ENABLE						
</#if>
<#if SYS_MQTT_APPDEBUG_DBG_LEVEL == true>
#define SYS_MQTT_APPDEBUG_DBG_LEVEL_ENABLE						
</#if>
<#if SYS_MQTT_APPDEBUG_INFO_LEVEL == true>
#define SYS_MQTT_APPDEBUG_INFO_LEVEL_ENABLE						
</#if>
<#if SYS_MQTT_APPDEBUG_FUNC_LEVEL == true>
#define SYS_MQTT_APPDEBUG_FUNC_LEVEL_ENABLE						
</#if>

<#if SYS_MQTT_APPDEBUG_CFG_FLOW == true>
#define SYS_MQTT_APPDEBUG_CFG_FLOW_ENABLE						
</#if>
<#if SYS_MQTT_APPDEBUG_DATA_FLOW == true>
#define SYS_MQTT_APPDEBUG_DATA_FLOW_ENABLE						
</#if>
<#if SYS_MQTT_APPDEBUG_PAHO_FLOW == true>
#define SYS_MQTT_APPDEBUG_PAHO_FLOW_ENABLE						
</#if>
</#if>

<#if SYS_MQTT_PUB_ENABLE == true>
#define SYS_MQTT_DEF_PUB_TOPIC_NAME							"${SYS_MQTT_PUB_TOPIC_NAME}"
<#if SYS_MQTT_PUB_QOS == "At most once (0)">
#define SYS_MQTT_DEF_PUB_QOS							0
<#elseif SYS_MQTT_PUB_QOS == "At least once (1)">
#define SYS_MQTT_DEF_PUB_QOS							1
<#elseif SYS_MQTT_PUB_QOS == "Exactly once (2)">
#define SYS_MQTT_DEF_PUB_QOS							2
</#if>
#define SYS_MQTT_DEF_PUB_RETAIN								${SYS_MQTT_PUB_RETAIN?string('true', 'false')}
</#if>

<#if SYS_MQTT_EXTRA_CRED == true>
#define SYS_MQTT_CONN_USERNAME							"${SYS_MQTT_USER_NAME}"
#define SYS_MQTT_CONN_PASSWORD							"${SYS_MQTT_PASSWORD}"
</#if>

<#if SYS_MQTT_LWT_ENABLE == true>
#define SYS_MQTT_CFG_LWT_ENABLE								true
#define SYS_MQTT_CFG_LWT_TOPIC_NAME							"${SYS_MQTT_LWT_TOPIC_NAME}"
<#if SYS_MQTT_LWT_QOS == "At most once (0)">
#define SYS_MQTT_CFG_LWT_QOS							0
<#elseif SYS_MQTT_LWT_QOS == "At least once (1)">
#define SYS_MQTT_CFG_LWT_QOS							1
<#elseif SYS_MQTT_LWT_QOS == "Exactly once (2)">
#define SYS_MQTT_CFG_LWT_QOS							2
</#if>
#define SYS_MQTT_CFG_LWT_RETAIN								${SYS_MQTT_LWT_RETAIN?string('true', 'false')}
#define SYS_MQTT_CFG_LWT_MESSAGE							"${SYS_MQTT_LWT_MESSAGE}"
<#else>
#define SYS_MQTT_CFG_LWT_ENABLE								false
</#if>
