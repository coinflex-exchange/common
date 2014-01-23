#include "compiler.h"
#include "http.h"
#include "nbio.h"

namespace {
class Handshake;
}

class WebSocketServer : public Socket6, public EPollable {
	friend Handshake;

protected:
	using Socket6::Socket6;

protected:
	operator int () const override _pure;
	void ready(EPoll &epoll, uint32_t events) override;
	virtual void prepare_response_headers(const HttpRequestHeaders &, HttpResponseHeaders &) { }
	virtual void client_attached(Socket6 &&socket, EPoll &epoll) = 0;

};
