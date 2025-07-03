//
// Created by Yeo Shu Heng on 3/7/25.
//

#include "Websocket.h"

Websocket::Websocket(io_context &ioc, ssl_context &soc, const msg_handler& handler_func, const error_handler& error_func)
    : ioc(ioc), soc(soc), resolver(ioc), socket(ioc, soc), handler(handler_func), e_handler(error_func) {
}

Websocket::~Websocket() {close();}

void Websocket::close() {
    beast::error_code ec;
    socket.close(ws::close_code::normal, ec);
    if (ec) {
        e_handler(ec);
    }
    read_buffer.consume(read_buffer.size());
}

void Websocket::connect(const std::string &host, const std::string &port, const std::string &target) {
    // resolve dns
    auto const results = resolver.resolve(host, port);

    // connect via tcp
    net::connect(socket.next_layer().next_layer(), results);

    // TLS handshake
    socket.next_layer().handshake(net::ssl::stream_base::client);

    // socket handshake
    socket.handshake(host, target);
}

std::string Websocket::subscribe(const std::string &subscribe_order) {
    socket.write(net::buffer(subscribe_order));

    read_buffer.consume(read_buffer.size());
    socket.read(read_buffer);
    std::string ret = boost::beast::buffers_to_string(read_buffer);

    read_buffer.consume(read_buffer.size());
    return ret;
}

void Websocket::start_async_read() {
    do_read();
}

void Websocket::send(const std::string& message) {
    socket.async_write(net::buffer(message), [this](const beast::error_code &ec, std::size_t) {
        if (ec) {return e_handler(ec);}
    });
}

void Websocket::do_read() {
    socket.async_read(read_buffer,
        [this](const beast::error_code &ec, std::size_t bytes_transferred) {
            if (ec) {
                return e_handler(ec);
            }

            // faster i/o here by directly parsing string from the buffer

            // change buffer to const and get reference view of readable bytes.
            const auto& buffer_data = static_cast<const beast::flat_buffer&>(read_buffer).data();

            // get pointer to start of reference view
            const auto data_ptr = static_cast<const char*>(buffer_data.data());
            const size_t data_size = buffer_data.size();

            // construct view using pointer + size
            const std::string_view msg_view(data_ptr, data_size);

            if (handler) {
                handler(msg_view);
            }

            read_buffer.consume(read_buffer.size());
            do_read();
        }
    );
}

