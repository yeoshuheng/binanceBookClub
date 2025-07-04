//
// Created by Yeo Shu Heng on 4/7/25.
//

#include "Callback.h"

#include <simdjson.h>
#include "spdlog/spdlog.h"

namespace beast = boost::beast;

std::function<void(const beast::error_code&)> build_error_handler() {
    return [](const beast::error_code& ec) {
        spdlog::error("problem in socket: {}", ec.message());
    };
}

std::function<void(const std::string_view&)> build_resp_handler(boost::lockfree::spsc_queue<BookUpdate>& update_queue) {
    return [&update_queue](const std::string_view& msg) {

        spdlog::debug("incoming_msg={}", msg);

        const int64_t recv_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

        simdjson::dom::parser parser;
        const simdjson::dom::element json = parser.parse(msg);

        double quantity;
        double price;

        const int64_t first_update_id = json["U"].get<int64_t>();
        const int64_t last_update_id = json["u"].get<int64_t>();

        for (const auto& bid : json["b"]) {
            price = std::stod(std::string(bid.at(0)));
            quantity = std::stod(std::string(bid.at(1)));
            BookUpdate update{recv_time, first_update_id, last_update_id, false, price, quantity};
            update_queue.push(update);
        }

        for (const auto& ask : json["a"]) {
            price = std::stod(std::string(ask.at(0)));
            quantity = std::stod(std::string(ask.at(1)));
            BookUpdate update{recv_time, first_update_id, last_update_id, true, price, quantity};
            update_queue.push(update);
        }

    };
}
