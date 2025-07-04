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
    double price;
    double quantity;
};

using update_queue = boost::lockfree::spsc_queue<BookUpdate>;

class OrderBookHandler final : public ThreadHandler {

    std::shared_ptr<OrderBook> order_book;
    update_queue &queue_in;

    std::string snapshot_url;

    int64_t order_book_last_update_id;
    int64_t order_book_last_full_snapshot_id;

    bool reload_order_book = true;

    void do_stuff() override;
    void load_order_book_from_snapshot();
    void update_order_book(const BookUpdate& incoming_update);

public:
    explicit OrderBookHandler(std::string  snapshot_url, update_queue &queue_in);
    ~OrderBookHandler() override;

    std::shared_ptr<const OrderBook> get_order_book_view();
};

#endif //ORDERBOOKHANDLER_H
