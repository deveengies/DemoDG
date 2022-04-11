//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: WebSocket SSL server, synchronous
//
//------------------------------------------------------------------------------

#include "server_certificate.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include <fstream>
#include <iomanip>
#include <list>
#include <map>
#include <filesystem>

using namespace std;

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>/** Read file into string. */

typedef websocket::stream<beast::ssl_stream<tcp::socket&>> WebSock;

const string listeClients = "../../workDir/liste_clients.txt";
const string fileToSend = "../../workDir/toSend.txt";

inline std::string file_to_string(const std::string& path) {
	std::ostringstream buf;
	std::ifstream input(path.c_str());
	buf << input.rdbuf();
	return buf.str();
}

struct Socket
{
	Socket(bool connected_, shared_ptr<WebSock> ws_) :
		connected(connected_), ws(ws_) {}

	Socket(const unique_ptr<Socket> so) :
		connected(so->connected),
		ws(move(so->ws)) {}

	bool connected;
	shared_ptr<WebSock> ws;
};

class Clients : public map<string, unique_ptr<Socket>>
{
public:
	void add_client(const string name)
	{
		(*this)[name] = nullptr;
	}

	void add_client(const string name, unique_ptr<Socket> so)
	{
		(*this)[name] = make_unique<Socket>(move(so));
	}

	bool all_connected()
	{
		for (const auto& cl : *this) {
			if (cl.second == nullptr || !cl.second->connected) {
				return false;
			}
		}
		return size() > 0;
	}

	void send_to_clients(const string& content)
	{
		for (const auto& cl : *this) {
			cl.second->ws->text(cl.second->ws->got_text());
			auto buffer = boost::asio::buffer(content, content.size());
			cl.second->ws->write(buffer);
			//cl.second->ws->write(net::buffer(std::string("essai")));
		}
	}
};

Clients clients;

void read_liste_client()
{
	ifstream liste(listeClients, ios::in | ios::binary);
	if (liste)
	{
		string name;
		cout << "Listeclients ..." << endl;
		while (getline(liste, name))
		{
			name.erase(std::remove(name.begin(), name.end(), '\r'), name.end());
			if (name.size() == 0) continue;
			cout << name << endl;
			clients.add_client(name);
		}
	}
	liste.close();
}

void wait_all_connected_then_send_file()
{
	while (!clients.all_connected()) {}
	cout << "send " << fileToSend << " to clients ... " << endl;
	clients.send_to_clients(file_to_string(fileToSend));
}

void read_forever(shared_ptr<WebSock> ws)
{
	for (;;) {
		beast::flat_buffer buffer;
		ws->read(buffer);
	}
}

//------------------------------------------------------------------------------

// Echoes back all received WebSocket messages
void
do_session(tcp::socket socket, ssl::context& ctx)
{
	try
	{
		// Construct the websocket stream around the socket
		auto ws = make_shared<WebSock>(socket, ctx);

		// Perform the SSL handshake
		ws->next_layer().handshake(ssl::stream_base::server);

		// Set a decorator to change the Server of the handshake
		ws->set_option(websocket::stream_base::decorator(
			[](websocket::response_type& res)
			{
				res.set(http::field::server,
					std::string(BOOST_BEAST_VERSION_STRING) +
					" websocket-server-sync-ssl");
			}));

		// Accept the websocket handshake
		ws->accept();

		// This buffer will hold the incoming message
		beast::flat_buffer buffer;

		// Read a message
		ws->read(buffer);

		// login name
		string clname = boost::beast::buffers_to_string(buffer.cdata());

		// update client  
		auto it = clients.find(clname);
		if (it != clients.end()) {
			ws->text(ws->got_text());
			clients.add_client(clname, make_unique<Socket>(Socket(true, ws)));
			cout << clname << "  connected" << endl;
		}
		else {
			cout << clname << "  not in the list" << endl;
		}

		read_forever(ws);
		//std::this_thread::sleep_until(std::chrono::time_point<std::chrono::system_clock>::max());
	}
	catch (beast::system_error const& se)
	{
		// This indicates that the session was closed
		std::cerr << "Error: " << se.code().message() << std::endl;
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}


//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	try
	{
		// Check command line arguments.
		if (argc != 3)
		{
			std::cerr <<
				"Usage: websocket-server-sync-ssl <address> <port>\n" <<
				"Example:\n" <<
				"    websocket-server-sync-ssl 0.0.0.0 8080\n";
			return EXIT_FAILURE;
		}

		// get the list of allowed clients
		read_liste_client();

		// serveur ip + port tcp
		auto const address = net::ip::make_address(argv[1]);
		auto const port = static_cast<unsigned short>(std::atoi(argv[2]));

		// The io_context is required for all I/O
		net::io_context ioc{ 1 };

		// The SSL context is required, and holds certificates
		//ssl::context ctx{ ssl::context::tlsv12 };
		//auto ctx = make_unique< ssl::context>(ssl::context::tlsv12);
		ssl::context ctx{ ssl::context::tlsv12 };

		// This holds the self-signed certificate used by the server
		load_server_certificate(ctx);

		// The acceptor receives incoming connections
		tcp::acceptor acceptor{ ioc, {address, port} };

		// When all clients from the list will be connected ... send the file to them
		void (*ptr)() = &wait_all_connected_then_send_file;
		std::thread(ptr).detach();

		// Look at new connection forever
		for (;;)
		{
			// This will receive the new connection
			tcp::socket socket{ ioc };

			// Block until we get a connection
			acceptor.accept(socket);

			// Launch the session, transferring ownership of the socket
			std::thread(
				&do_session,
				std::move(socket),
				std::ref(ctx)
			).detach();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}