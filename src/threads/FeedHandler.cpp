//
// Created by Yeo Shu Heng on 3/7/25.
//

#include "FeedHandler.h"
#include "../feeds/Callback.h"
#include "spdlog/spdlog.h"

FeedHandler::FeedHandler(queue &update_queue, io_context &ioc, ssl_context &soc, const std::string &host, const std::string &port, const std::string &target, const std::string& sub_msg)
    : update_queue(update_queue), ioc(ioc), soc(soc), ws(ioc, soc, build_resp_handler(update_queue), build_error_handler()), sub_msg(sub_msg), host(host), port(port), target(target) {}

FeedHandler::~FeedHandler() {
    spdlog::info("feed handler closed.");
}

void FeedHandler::do_stuff() {
    try {
        // handles connection + subscribe synchronously
        ws.connect(host, port, target);
        std::string sub_results = ws.subscribe(sub_msg);
        spdlog::info("subscribed to stream, resp: {}", sub_results);

        // handles streaming asynchronously
        ws.start_async_read();
        ioc.run();

    } catch (const std::exception &e) {
        spdlog::error("error in feed: {}", e.what());
    }
}

void FeedHandler::stop_handler() {
    ioc.stop();
    ThreadHandler::stop_handler();
}



