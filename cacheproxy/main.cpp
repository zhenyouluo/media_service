﻿/*
 * main.cpp
 *
 *  Created on: 2014年7月6日
 *      Author: jimm
 */

#include <sys/stat.h>
#include <sys/types.h>
#include "../netevent/net_namespace.h"
#include "../netevent/net_impl.h"
#include "../netevent/net_typedef.h"
#include "../netevent/net_reactor.h"
#include "../netevent/net_handler.h"
#include "../netevent/net_connection.h"
#include "../netevent/net_acceptor.h"
#include "../netevent/net_connector.h"
#include "../logger/logger.h"
#include "../frame/frame.h"
#include "server_typedef.h"
#include "regist_message.h"
#include "server_config.h"
#include "packetparser.h"
#include "packetparser_factory.h"
#include "message_handler.h"
#include "datacenter.h"

using namespace LOGGER;
using namespace NETEVENT;
using namespace FRAME;

int32_t InitNet(CNetHandler *pNetHandler)
{
	CServerConfig *pServerConfig = (CServerConfig *)g_Frame.GetConfig(CONFIG_SERVER);
	if(pServerConfig == NULL)
	{
		return 1;
	}

	//创建包解析器工厂
	IPacketParserFactory *pParserFactory = new CPacketParserFactory();

	CMessageHandler *pMessageHandler = new CMessageHandler();

	//创建服务器
	CAcceptor *pAcceptor = new CAcceptor(pNetHandler, pParserFactory, pMessageHandler);
	pAcceptor->Bind(pServerConfig->GetListenPort());

	CMemcacheClient *pMemcacheClient = g_DataCenter.GetMemcacheClient();
	pMemcacheClient->Set("test", "123", 0);
	return 0;
}

int32_t main()
{
	//启动日志线程
	CLogger::Start();

	//创建网络事件处理器
	CNetHandler *pNetHandler = new CNetHandler();
	pNetHandler->CreateReactor();

	g_Frame.AddRunner(pNetHandler);

	if(g_Frame.Init(SERVER_NAME) != 0)
	{
		return 0;
	}

	InitNet(pNetHandler);

	while(true)
	{
		g_Frame.Run();
	}

	g_Frame.Uninit();

	return 0;
}



