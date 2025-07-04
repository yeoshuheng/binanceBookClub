//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef ORDERBOOKHANDLER_H
#define ORDERBOOKHANDLER_H

#include "ThreadHandler.h"
#include "../orderbook/OrderBook.h"
#include "boost/lockfree/spsc_queue.hpp"

struct BookUpdate {

    int64_t sys_recv_time;

    int64_t first_update_id; // U
    int64_t last_update_id; // u

    bool is_ask;
    float price;
    float quantity;
};

using update_queue = boost::lockfree::spsc_queue<BookUpdate>;

class OrderBookHandler final : public ThreadHandler {

    std::shared_ptr<OrderBook> order_book;
    update_queue &queue_in;

    std::string snapshot_url;

    int64_t order_book_last_update_id;
    int64_t order_book_last_full_snapshot_id;

    int64_t time_taken_ns = 0;
    int orders_processed = 0;

    std::thread logging_thread;

    bool reload_order_book = true;

    void stop_handler() override;
    void start_logging();

    /**
     * Hot path functions
     */
    void do_stuff() override;
    void update_order_book(const BookUpdate& incoming_update);
    void load_order_book_from_snapshot();

public:
    explicit OrderBookHandler(std::string  snapshot_url, update_queue &queue_in);
    ~OrderBookHandler() override;
    void start_handler() override;
    std::shared_ptr<const OrderBook> get_order_book_view();
};

#endif //ORDERBOOKHANDLER_H
