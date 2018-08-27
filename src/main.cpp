#if defined(USE_STANDALONE_ASIO)
#undef USE_STANDALONE_ASIO
#endif

#include "blib/bun/bun.hpp"
#include <web/client_http.hpp>
#include <web/server_http.hpp>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <fmt/format.hpp>
#include <sstream>
#include <memory>

namespace ushort {
	struct Users {
		std::string uname;
		std::string edit_key;
		std::string url;
		std::string short_key;
	};
}

SPECIALIZE_BUN_HELPER((ushort::Users, uname, edit_key, url, short_key));

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

int main() {

	// HTTP-server at port 8080 using 1 thread
	// Unless you do more heavy non-threaded processing in the resources,
	// 1 thread is usually faster than several threads
	HttpServer server;
	server.config.port = 9991;

	/*
	HTTP/1.1 200 OK
	Date: Mon, 27 Jul 2009 12:28:53 GMT
	Server: Apache/2.2.14 (Win32)
	Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT
	Content-Length: 88
	Content-Type: text/html
	Connection: Closed
	*/
	// Add resources using path-regex and method-string, and an anonymous function
	// POST-example for the path /string, responds the posted string
	// {'url': 'www.google.com', 'alias': 'google'}
	server.resource["^/create_alias"]["POST"] = [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
		// Retrieve string:
		rapidjson::Document doc;
		const auto json = request->content.string();
		doc.Parse(json.c_str(), json.length());
		const std::string http_version = "HTTP/1.1 ";
		std::string http_code = "200 OK\r\n";
		if (doc.HasParseError() == false) {
			// request->content.string() is a convenience function for:
			// stringstream ss;
			// ss << request->content.rdbuf();
			// auto content=ss.str();

			*response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n"<< json;

			// Alternatively, use one of the convenience functions, for instance:
			// response->write(content);
		}
		else {
			const std::string err = fmt::format("Input Error: {} at Line No: {}", rapidjson::GetParseError_En(doc.GetParseError()), doc.GetErrorOffset());
		}
	};

	// Add resources using path-regex and method-string, and an anonymous function
	// POST-example for the path /string, responds the posted string
	// {'secrete_key': 'www.google.com', 'alias': 'www.google.com', 're_alias': 'www.google.com'}
	server.resource["^/update_alias"]["POST"] = [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
		// Retrieve string:
		rapidjson::Document doc;
		const auto json = request->content.string();
		doc.Parse(json.c_str(), json.length());
		if (doc.HasParseError() == false) {
			// request->content.string() is a convenience function for:
			// stringstream ss;
			// ss << request->content.rdbuf();
			// auto content=ss.str();

			*response << "HTTP/1.1 200 OK\r\nContent-Length: " << json.length() << "\r\n\r\n" << json;

			// Alternatively, use one of the convenience functions, for instance:
			// response->write(content);
		}
		else {
			const std::string err = fmt::format("Input Error: {} at Line No: {}", rapidjson::GetParseError_En(doc.GetParseError()), doc.GetErrorOffset());
		}
	};

	// GET-example for the path /info
	// Responds with request-information
	server.resource["^/info$"]["GET"] = [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
		std::stringstream stream;
		stream << "<h1>Request from " << request->remote_endpoint_address() << ":" << request->remote_endpoint_port() << "</h1>";

		stream << request->method << " " << request->path << " HTTP/" << request->http_version;

		stream << "<h2>Query Fields</h2>";
		auto query_fields = request->parse_query_string();
		for (auto &field : query_fields)
			stream << field.first << ": " << field.second << "<br>";

		stream << "<h2>Header Fields</h2>";
		for (auto &field : request->header)
			stream << field.first << ": " << field.second << "<br>";
		std::string s(10000, 'h');
		stream << s;
		*response << "HTTP/1.1 302 OK\r\nLocation: http://www.google.com/ " << "\r\n\r\n";
		response->write(stream);
	};

	std::thread server_thread([&server]() {
		// Start server
		server.start();
	});

	server_thread.join();
	return 1;
}
