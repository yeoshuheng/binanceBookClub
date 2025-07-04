//
// Created by Yeo Shu Heng on 3/7/25.
//

#include "OrderBookHandler.h"
#include <cpr/cpr.h>
#include <string>
#include <utility>
#include <simdjson.h>
#include "spdlog/spdlog.h"

OrderBookHandler::OrderBookHandler(std::string snapshot_url, update_queue &queue_in) : queue_in(queue_in),
    snapshot_url(std::move(snapshot_url)), order_book_last_update_id(0), order_book_last_full_snapshot_id(0) {
}

OrderBookHandler::~OrderBookHandler() {
    spdlog::info("closing order book builder.");
}

std::shared_ptr<const OrderBook> OrderBookHandler::get_order_book_view() {
    return order_book;
}

void OrderBookHandler::do_stuff() {

    BookUpdate incoming_update{};

    int64_t time_taken_ns = 0;
    int orders_processed = 0;

    while (running.load(std::memory_order_relaxed)) {

        // we start from a new book, we prepopulate first.
        if (reload_order_book) {
            load_order_book_from_snapshot();
            // ensure the depth snapshot is greater than our first event.
            while (order_book_last_update_id < incoming_update.last_update_id) {
                load_order_book_from_snapshot();
            }
        }

        if (queue_in.pop(incoming_update)) {

            auto start = std::chrono::high_resolution_clock::now();

            // remove incoming data that ends before our snapshot (stale)
            if (incoming_update.last_update_id <= order_book_last_full_snapshot_id) {
                spdlog::warn("stale update: update_first_id={}, update_last_id={}, orderbook_last_full_snap_id={}", incoming_update.first_update_id, incoming_update.last_update_id, order_book_last_full_snapshot_id);
                continue;
            }
            if (incoming_update.last_update_id < order_book_last_update_id) {
                spdlog::warn("stale update: update_first_id={}, update_last_id={}, orderbook_id={}", incoming_update.first_update_id, incoming_update.last_update_id, order_book_last_update_id);
                continue;
            }

            // incoming data starts after our snapshot, we most likely missed something.
            if (incoming_update.first_update_id > order_book_last_update_id + 1) {
                spdlog::warn("missed update, reloading orderbook: update_first_id={}, update_last_id={}, orderbook_id={}", incoming_update.first_update_id, incoming_update.last_update_id, order_book_last_update_id);
                reload_order_book = true;
                continue;
            }

            update_order_book(incoming_update);

            // tracking time
            const int64_t update_finish_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
            orders_processed++;
            const int64_t tick_to_update = update_finish_time - incoming_update.sys_recv_time;
            time_taken_ns += tick_to_update;

            if (orders_processed > 0 && orders_processed % 5000 == 0) {
                spdlog::info("version={}, order_processing_time={}ns, total_orders_processed={}, avg_processing_time={}ns\n{}", order_book_last_update_id, tick_to_update, orders_processed, (time_taken_ns / orders_processed), order_book->to_string());
            }
        }
    }
}

void OrderBookHandler::update_order_book(const BookUpdate& incoming_update) {

    // add price levels.
    if (incoming_update.is_ask) {
        order_book->update_ask(incoming_update.price, incoming_update.quantity);
    } else {
        order_book->update_bid(incoming_update.price, incoming_update.quantity);
    }

    // update current version of order book
    order_book_last_update_id = incoming_update.last_update_id;
}

void OrderBookHandler::load_order_book_from_snapshot() {
    spdlog::info("Initialising order book with snapshot from {}...", snapshot_url);

    // build a new order book.
    order_book = std::make_shared<OrderBook>();

    if (const cpr::Response r = cpr::Get(cpr::Url{snapshot_url}); r.status_code == 200) {

        simdjson::dom::parser parser;
        const simdjson::dom::element json = parser.parse(r.text);
        double quantity;
        double price;

        order_book_last_update_id = order_book_last_full_snapshot_id = json["lastUpdateId"].get<int64_t>();
        reload_order_book = false;

        for (const auto& bid : json["bids"]) {
            price = std::stod(std::string(bid.at(0)));
            quantity = std::stod(std::string(bid.at(1)));
            spdlog::debug("update: price={}, qty={}", price, quantity);
            order_book->update_bid(price, quantity);
        }

        for (const auto& ask : json["asks"]) {
            price = std::stod(std::string(ask.at(0)));
            quantity = std::stod(std::string(ask.at(1)));
            spdlog::debug("update: price={}, qty={}", price, quantity);
            order_book->update_ask(price, quantity);
        }

        spdlog::info("order book successfully initialised with order book id={}", order_book_last_update_id);
    } else {
        throw std::runtime_error("failed to get snapshot to construct initial orderbook, status_code=" + std::to_string(r.status_code));
    }
}
