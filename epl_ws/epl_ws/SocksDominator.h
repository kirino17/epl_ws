#pragma once
#include <Windows.h>
#include <sstream>
#include <string>
#include <Shlwapi.h>
#include <vector>
#include <list>
#include "cpprest\containerstream.h"
#include "cpprest\filestream.h"
#include "cpprest\http_client.h"
#include "cpprest\json.h"
#include "cpprest\producerconsumerstream.h"
#include "cpprest\details\web_utilities.h"
#include "cpprest\uri.h"
#include "cpprest\streams.h"
#include "cpprest\ws_client.h"
#include "cpprest\ws_msg.h"
#include "cpprest\asyncrt_utils.h"
#include <atlconv.h>
#include <string>

using namespace pplx;
using namespace utility;
using namespace web;
using namespace concurrency::streams;
using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace web::websockets::client;


typedef void(CALLBACK *LPFN_WS_EVENT_CALLBACK)(DWORD e);
#define WS_EVT_ONOPEN		0
#define WS_EVT_ONMESSAGE	1
#define WS_EVT_CLOSE		2
#define WS_EVT_ERROR		3

#define WS_STATE_CONNECTING		0
#define WS_STATE_OPEN			1
#define WS_STATE_CLOSING		2
#define WS_STATE_CLOSE			3	

class SocksDominator
{
public:
	SocksDominator(const char *domain, LPFN_WS_EVENT_CALLBACK onopen, 
		LPFN_WS_EVENT_CALLBACK onmessage,
		LPFN_WS_EVENT_CALLBACK onclose,
		LPFN_WS_EVENT_CALLBACK onerror);
	~SocksDominator();

	websocket_client *GetHandler(void) { return &ws; }

	LPFN_WS_EVENT_CALLBACK GetCallback(int evt) {
		switch (evt)
		{
		case 0:
			return m_onopen;
		case 1:
			return m_onmessage;
		case 2:
			return m_onclose;
		case 3:
			return m_onerror;
		default:
			break;
		}
		return 0;
	}

	void SetBody(websocket_incoming_message *body) { m_body = body; }

	websocket_incoming_message *GetBody(void) { return m_body; };

	void Close(void) {
		this->SetReadyState(WS_STATE_CLOSING);
		this->GetHandler()->close().then([this]() {
			this->SetReadyState(WS_STATE_CLOSE);
		});
	}

	void SetReadyState(int state) {
		m_readyState = state;
	}

	int GetReadyState(void) { return m_readyState; }

	void OnMsgDispatchs(void);

private:
	websocket_client ws;
	std::string m_domain;
	LPFN_WS_EVENT_CALLBACK m_onopen;
	LPFN_WS_EVENT_CALLBACK m_onmessage;
	LPFN_WS_EVENT_CALLBACK m_onclose;
	LPFN_WS_EVENT_CALLBACK m_onerror;
	websocket_incoming_message *m_body;
	int m_readyState;
};

