//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef ORDERBOOKHANDLER_H
#define ORDERBOOKHANDLER_H
#include <memory>

#include "ThreadHandler.h"
#include "../orderbook/OrderBook.h"

struct BookUpdate {
    bool is_ask;
    int32_t price;
    int64_t quantity;
};

class OrderBookHandler : public ThreadHandler {
    std::shared_ptr<OrderBook> order_book;
};



#endif //ORDERBOOKHANDLER_H
