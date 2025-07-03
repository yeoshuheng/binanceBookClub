//
// Created by Yeo Shu Heng on 3/7/25.
//

#include <functional>
#include <boost/beast/core/error.hpp>

#include <boost/lockfree/queue.hpp>
#include "../threads/OrderBookHandler.h"
#include <nlohmann/json.hpp>

#include "spdlog/spdlog.h"

std::function<void(const boost::beast::error_code&)> build_error_handler() {
    return [](const boost::beast::error_code& ec) {
        spdlog::error("problem in socket: {}", ec.message());
    };
}

std::function<void(const std::string_view&)> build_resp_handler(boost::lockfree::queue<BookUpdate>& update_queue) {
    return [&update_queue](const std::string_view& msg) {
        auto json = nlohmann::json::parse(msg);
        int64_t quantity;
        int32_t price;

        if (json.contains("bids")) {
            for (const auto& bid : json["bids"]) {
                price = static_cast<int32_t>(std::stod(bid[0].get<std::string>()));
                quantity = static_cast<int64_t>(std::stod(bid[1].get<std::string>()));
                BookUpdate update{false, price, quantity};
                update_queue.push(update);
            }
        }

        if (json.contains("asks")) {
            for (const auto& bid : json["asks"]) {
                price = static_cast<int32_t>(std::stod(bid[0].get<std::string>()));
                quantity = static_cast<int64_t>(std::stod(bid[1].get<std::string>()));
                BookUpdate update{true, price, quantity};
                update_queue.push(update);
            }
        }
    };
}
