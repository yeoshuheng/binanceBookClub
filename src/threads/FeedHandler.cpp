//
// Created by Yeo Shu Heng on 3/7/25.
//

#include "FeedHandler.h"
#include "../feeds/Callbacks.cpp"

FeedHandler::FeedHandler(queue &update_queue, io_context &ioc, ssl_context &soc, const std::string &host, const std::string &port, const std::string &target, const std::string& sub_msg) : update_queue(update_queue), ioc(ioc), soc(soc), host(host), port(port), target(target), sub_msg(sub_msg) {
    ws = std::make_unique<Websocket>(ioc, soc, build_resp_handler(update_queue), build_error_handler());
}

FeedHandler::~FeedHandler() {
    ThreadHandler::~ThreadHandler();
    spdlog::info("feed handler closed.");
}

void FeedHandler::do_stuff() {
    try {
        // handles synchronously
        ws->connect(host, port, target);
        std::string sub_results = ws->subscribe(sub_msg);
        spdlog::info("subscribed to stream, resp: {}", sub_results);

        ws->start_async_read();
    } catch (const std::exception &e) {
        spdlog::error("error in feed: {}", e.what());
    }
}

void FeedHandler::stop_handler() {
    ioc.stop();
    ThreadHandler::stop_handler();
}



