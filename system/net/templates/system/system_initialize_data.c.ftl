
const SYS_NET_Config g_sSysNetConfig0 =
{
	.mode = SYS_NET_INDEX0_MODE, 
	.intf = SYS_NET_INDEX0_INTF,
	.port = SYS_NET_INDEX0_PORT,
	.enable_reconnect = SYS_NET_INDEX0_RECONNECT,
	.enable_tls = SYS_NET_INDEX0_ENABLE_TLS,
    .ip_prot = SYS_NET_INDEX0_IPPROT,
	.host_name = SYS_NET_INDEX0_HOST_NAME,
};

<#if SYS_NET_IDX1 == true>
const SYS_NET_Config g_sSysNetConfig1 =
{
	.mode = SYS_NET_INDEX1_MODE, 
	.intf = SYS_NET_INDEX1_INTF,
	.port = SYS_NET_INDEX1_PORT,
	.enable_reconnect = SYS_NET_INDEX1_RECONNECT,
	.enable_tls = SYS_NET_INDEX1_ENABLE_TLS,
    .ip_prot = SYS_NET_INDEX0_IPPROT,
	.host_name = SYS_NET_INDEX1_HOST_NAME,
};
</#if>

