//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>

namespace net = boost::asio;
namespace ws = boost::beast::websocket;
namespace beast = boost::beast;

using ssl_context = net::ssl::context;
using io_context = net::io_context;
using tcp = net::ip::tcp;
using sock = ws::stream<beast::ssl_stream<tcp::socket>>;
using sock_buf = beast::flat_buffer;
using msg_handler = std::function<void(const std::string_view&)>;
using error_handler = std::function<void(const beast::error_code&)>;

class Websocket {

    io_context& ioc;
    ssl_context& soc;
    tcp::resolver resolver;

    sock socket;
    sock_buf read_buffer;

    msg_handler handler;
    error_handler e_handler;

    void close();
    void do_read();

public:
    Websocket(io_context& ioc, ssl_context& soc, const msg_handler &handler_func, const ::error_handler &error_func);
    ~Websocket();

    void connect(const std::string& host, const std::string& port, const std::string& target);
    std::string subscribe(const std::string& subscribe_order);
    void send(const std::string& message);
    void start_async_read();
};



#endif //WEBSOCKET_H
