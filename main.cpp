#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "src/threads/FeedHandler.h"
#include <iostream>

int main() {

    const auto console = spdlog::stderr_color_mt("console");
    console->set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::info("logger initialised.");

    net::io_context ioc;

    ssl_context soc(net::ssl::context::tlsv12_client);
    soc.load_verify_file("/opt/homebrew/etc/openssl@3/cert.pem");
    soc.set_default_verify_paths();

    const std::string host = "stream.binance.com";
    const std::string port = "9443";
    const std::string target = "/ws";
    const std::string sub_msg = R"({
      "method": "SUBSCRIBE",
      "params": ["btcusdt@depth@100ms"],
      "id": 1
    })";

    boost::lockfree::spsc_queue<BookUpdate> update_queue{1024};

    auto l2_feed = FeedHandler(update_queue, ioc, soc, host, port, target, sub_msg);
    l2_feed.start_handler();

    std::string snapshot_url = "https://api.binance.com/api/v3/depth?symbol=BTCUSDT&limit=" + std::to_string(ORDERBOOK_MAX_DEPTH);
    auto order_book_builder = OrderBookHandler(snapshot_url, update_queue);
    order_book_builder.start_handler();


    spdlog::info("press something to exit...");
    std::cin.get();

    return 0;
}
