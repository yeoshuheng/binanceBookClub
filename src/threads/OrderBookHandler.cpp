//
// Created by Yeo Shu Heng on 3/7/25.
//

#include "OrderBookHandler.h"
#include <cpr/cpr.h>
#include <string>
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

OrderBookHandler::OrderBookHandler(update_queue &queue_in) : queue_in(queue_in) {}

OrderBookHandler::~OrderBookHandler() {
    spdlog::info("closing order book builder.");
}

void OrderBookHandler::do_stuff() {

    BookUpdate incoming_update{};

    while (running.load(std::memory_order_relaxed)) {
        if (queue_in.pop(incoming_update)) {

            // ensure that the update is valid from the initial snapshot used to build the order book.
            if (incoming_update.check_against_snapshot < inital_snapshot_id) {
                spdlog::warn("dropping snapshot with id={}, stale from snapshot");
                continue;
            }

            if (incoming_update.is_ask) {
                order_book.update_ask(incoming_update.price, incoming_update.quantity);
            } else {
                order_book.update_bid(incoming_update.price, incoming_update.quantity);
            }
            spdlog::debug(order_book.to_string());
        }
    }
}

void OrderBookHandler::initialise_order_book(const std::string& url) {
    spdlog::info("Initialising order book with snapshot from {}...", url);
    if (cpr::Response r = cpr::Get(cpr::Url{url}); r.status_code == 200) {
        auto json = nlohmann::json::parse(r.text);
        double quantity;
        double price;

        inital_snapshot_id = json["lastUpdateId"].get<int64_t>();
        spdlog::info("initial order book snapshot id: {}", inital_snapshot_id);
        if (json.contains("bids")) {
            for (const auto& bid : json["bids"]) {
                price = std::stod(bid[0].get<std::string>());
                quantity = std::stod(bid[1].get<std::string>());
                spdlog::debug("update: price={}, qty={}", price, quantity);
                order_book.update_bid(price, quantity);
            }
        }
        if (json.contains("asks")) {
            for (const auto& bid : json["asks"]) {
                price = std::stod(bid[0].get<std::string>());
                quantity = std::stod(bid[1].get<std::string>());
                spdlog::debug("update: price={}, qty={}", price, quantity);
                order_book.update_ask(price, quantity);
            }
        }
        spdlog::info("order book successfully initialised.");
        spdlog::info(order_book.to_string());
    } else {
        throw std::runtime_error("failed to get snapshot to construct initial orderbook, status_code=" + r.status_code);
    }
}
