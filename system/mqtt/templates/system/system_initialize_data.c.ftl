
const SYS_MQTT_Config g_sSysMqttConfig =
{
	.intf = SYS_MQTT_INDEX0_MQTT_INTF,
	.sBrokerConfig.brokerName = SYS_MQTT_INDEX0_BROKER_NAME, 
	.sBrokerConfig.serverPort = SYS_MQTT_INDEX0_MQTT_PORT,
	.sBrokerConfig.keepAliveInterval = SYS_MQTT_INDEX0_KEEPALIVE_INTERVAL,
	.sBrokerConfig.autoConnect = SYS_MQTT_INDEX0_RECONNECT,
	.sBrokerConfig.tlsEnabled = SYS_MQTT_INDEX0_ENABLE_TLS,
	.sBrokerConfig.clientId = SYS_MQTT_INDEX0_CLIENT_ID,
	.subscribeCount = SYS_MQTT_INDEX0_SUB_TOPIC_COUNT,
	.sSubscribeConfig[0].topicName = SYS_MQTT_INDEX0_TOPIC_NAME,
	.sSubscribeConfig[0].qos = SYS_MQTT_INDEX0_SUB_QOS,
	.sSubscribeConfig[0].entryValid = SYS_MQTT_INDEX0_ENTRY_VALID,
<#if SYS_MQTT_EXTRA_CRED == true>
	.sBrokerConfig.username = SYS_MQTT_CONN_USERNAME,
	.sBrokerConfig.password = SYS_MQTT_CONN_PASSWORD,
</#if>	
<#if SYS_MQTT_LWT_ENABLE == true>
	.bLwtEnabled = true,
	.sLwtConfig.qos = SYS_MQTT_CFG_LWT_QOS,
	.sLwtConfig.retain = SYS_MQTT_CFG_LWT_RETAIN,
	.sLwtConfig.message = SYS_MQTT_CFG_LWT_MESSAGE,
	.sLwtConfig.topicName = SYS_MQTT_CFG_LWT_TOPIC_NAME,
<#else>
	.bLwtEnabled = false,
</#if>	
};

