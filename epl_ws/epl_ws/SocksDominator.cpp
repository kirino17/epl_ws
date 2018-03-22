#include "stdafx.h"
#include "SocksDominator.h"


SocksDominator::SocksDominator(const char *domain, LPFN_WS_EVENT_CALLBACK onopen,
	LPFN_WS_EVENT_CALLBACK onmessage,
	LPFN_WS_EVENT_CALLBACK onclose,
	LPFN_WS_EVENT_CALLBACK onerror)
{
	m_domain = domain;
	m_onopen = onopen;
	m_onmessage = onmessage;
	m_onclose = onclose;
	m_onerror = onerror;
	m_readyState = 0;
}


SocksDominator::~SocksDominator()
{

}

void SocksDominator::OnMsgDispatchs(void) {
	this->GetHandler()->receive().then([this](websocket_incoming_message body) ->void {
		if (body.message_type() == websocket_message_type::close) {
			if (this->GetCallback(WS_EVT_CLOSE)) {
				this->SetReadyState(WS_STATE_CLOSING);
				this->GetCallback(WS_EVT_CLOSE)((DWORD)this);

				this->GetHandler()->close().then([this]() ->void {
					this->SetReadyState(WS_STATE_CLOSE);
				}).then([this](pplx::task<void> t) {
					try
					{
						t.get();
					}
					catch (const std::exception& ex)
					{
						this->SetReadyState(WS_STATE_CLOSE);
					}
				});
			}
			return;
		}
		this->SetBody(&body);
		if (this->GetCallback(WS_EVT_ONMESSAGE)) {
			this->GetCallback(WS_EVT_ONMESSAGE)((DWORD)this);
			this->OnMsgDispatchs();
		}
	}).then([this](pplx::task<void> t) ->void {
		try
		{
			t.get();
		}
		catch (const std::exception& ex)
		{
			if (this->GetCallback(WS_EVT_CLOSE)) {
				this->SetReadyState(WS_STATE_CLOSE);
				this->GetCallback(WS_EVT_CLOSE)((DWORD)this);
			}
		}
	});
}