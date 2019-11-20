/*
  Copyright 2019 www.dev5.cn, Inc. dev5@qq.com
 
  This file is part of X-MSG-IM.
 
  X-MSG-IM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  X-MSG-IM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU Affero General Public License
  along with X-MSG-IM.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <libx-msg-ap-core.h>
#include <libx-msg-ap-msg.h>
#include "XmsgAp.h"

XmsgAp* XmsgAp::inst = new XmsgAp();

XmsgAp::XmsgAp()
{

}

XmsgAp* XmsgAp::instance()
{
	return XmsgAp::inst;
}

bool XmsgAp::start(const char* path)
{
	Log::setInfo();
	shared_ptr<XmsgApCfg> cfg = XmsgApCfg::load(path); 
	if (cfg == nullptr)
		return false;
	Log::setLevel(cfg->cfgPb->log().level().c_str());
	Log::setOutput(cfg->cfgPb->log().output());
	Xsc::init();
	vector<shared_ptr<XscServer>> servers;
	if (cfg->pubXscTcpServerCfg() != nullptr) 
	{
		shared_ptr<XscTcpServer> pubTcpServer(new XscTcpServer(cfg->cfgPb->cgt(), shared_ptr<XmsgApTcpLog>(new XmsgApTcpLog()))); 
		if (!pubTcpServer->startup(cfg->pubXscTcpServerCfg()))
			return false;
		servers.push_back(pubTcpServer);
	}
	if (cfg->pubXscHttpServerCfg() != nullptr) 
	{
		shared_ptr<XscHttpServer> pubHttpServer(new XscHttpServer(cfg->cfgPb->cgt(), shared_ptr<XmsgApHttpLog>(new XmsgApHttpLog()))); 
		if (!pubHttpServer->startup(cfg->pubXscHttpServerCfg()))
			return false;
		servers.push_back(pubHttpServer);
	}
	if (cfg->pubXscWebSocketServerCfg() != nullptr) 
	{
		shared_ptr<XscWebSocketServer> pubWebSocketServer(new XscWebSocketServer(cfg->cfgPb->cgt(), shared_ptr<XmsgApWebSocketLog>(new XmsgApWebSocketLog()))); 
		if (!pubWebSocketServer->startup(cfg->pubXscWebSocketServerCfg()))
			return false;
		servers.push_back(pubWebSocketServer);
	}
	shared_ptr<XscTcpServer> priServer(new XscTcpServer(cfg->cfgPb->cgt(), shared_ptr<XmsgApTcpLog>(new XmsgApTcpLog()))); 
	if (!priServer->startup(cfg->priXscTcpServerCfg()))
		return false;
	vector<shared_ptr<XmsgImN2HMsgMgr>> pubMsgMgrs;
	for (auto& it : servers)
		pubMsgMgrs.push_back(shared_ptr<XmsgImN2HMsgMgr>(new XmsgImN2HMsgMgr(it)));
	XmsgApMsg::init(pubMsgMgrs, shared_ptr<XmsgImN2HMsgMgr>(new XmsgImN2HMsgMgr(priServer))); 
	for (auto& it : servers)
	{
		if (!it->publish())
			return false;
	}
	if (!priServer->publish()) 
		return false;
	if (!this->connect2ne(priServer))
		return false;
	Xsc::hold([](ullong now)
	{

	});
	return true;
}

bool XmsgAp::connect2ne(shared_ptr<XscTcpServer> priServer)
{
	return true;
}

XmsgAp::~XmsgAp()
{

}

