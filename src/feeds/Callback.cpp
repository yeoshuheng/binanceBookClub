//
// Created by Yeo Shu Heng on 4/7/25.
//

#include "Callback.h"
#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"

namespace beast = boost::beast;

std::function<void(const beast::error_code&)> build_error_handler() {
    return [](const beast::error_code& ec) {
        spdlog::error("problem in socket: {}", ec.message());
    };
}

std::function<void(const std::string_view&)> build_resp_handler(boost::lockfree::queue<BookUpdate>& update_queue) {
    return [&update_queue](const std::string_view& msg) {

        spdlog::debug("incoming_msg={}", msg);

        auto json = nlohmann::json::parse(msg);
        double quantity;
        double price;

        const int64_t first_update_id = json["U"].get<int64_t>();
        const int64_t last_update_id = json["u"].get<int64_t>();

        if (json.contains("b")) {
            for (const auto& bid : json["b"]) {
                price = std::stod(bid[0].get<std::string>());
                quantity = std::stod(bid[1].get<std::string>());
                BookUpdate update{first_update_id, last_update_id, false, price, quantity};
                update_queue.push(update);
            }
        }

        if (json.contains("a")) {
            for (const auto& ask : json["a"]) {
                price = std::stod(ask[0].get<std::string>());
                quantity = std::stod(ask[1].get<std::string>());
                BookUpdate update{first_update_id, last_update_id, true, price, quantity};
                update_queue.push(update);
            }
        }
    };
}
