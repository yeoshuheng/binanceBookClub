//
// Created by Yeo Shu Heng on 3/7/25.
//

#include "./OrderBook.h"
#include <sstream>

/**
 * Main user functions.
 */

OrderBook::OrderBook(): bid(), ask(), bid_size(0), ask_size(0) {}

void OrderBook::update_bid(const double price, const double quantity) {
    update_side(bid, bid_size, price, quantity, false);
}

void OrderBook::update_ask(const double price, const double quantity) {
    update_side(ask, ask_size, price, quantity, true);
}

double OrderBook::get_top_bid() const {return bid[0].price;};

double OrderBook::get_top_ask() const {return ask[0].price;};

double OrderBook::get_spread() const {return get_top_ask() - get_top_bid();};

double OrderBook::get_mid_price() const {return get_top_ask() + (get_spread() / 2);};

double OrderBook::get_bid_size() const {return bid_size;};

double OrderBook::get_ask_size() const {return ask_size;};

std::span<const BookLevel>  OrderBook::get_ask() const {
    return {ask.data(), ask.size()};
}

std::span<const BookLevel> OrderBook::get_bid() const {
    return {bid.data(), bid.size()};
}

std::string OrderBook::to_string() const {
    std::stringstream ss;
    ss << "l1={" << "bid=" << std::to_string(get_top_bid()) << ", ask=" << std::to_string(get_top_ask()) << ", mid=" << std::to_string(get_mid_price()) << "}\n";
    ss << "asks={" << "\n";
    for (const auto& level : get_ask()) {
        ss << level.to_string() << "\n";
    }
    ss << "}" << "\n";
    ss << "bids={" << "\n";
    for (const auto& level : get_bid()) {
        ss << level.to_string() << "\n";
    }
    ss << "}" << "\n";
    return ss.str();
}

/**
 * Functions for order book update operations.
 */

int OrderBook::search_side(const double target_price, const int side_size, const std::array<BookLevel, ORDERBOOK_MAX_DEPTH> &side, bool is_ask) {
    int left = 0; int right = side_size; int mid = 0;
    while (left < right) {
        mid = left + (right - left) / 2;
        if (side[mid].price == target_price) {
            return mid;
        }
        if (is_ask) { // for ask orders we want the upper bound, ask orders are ascending.
            if (side[mid].price > target_price) {
                right = mid;
            } else {
                left = mid + 1;
            }
        } else { // for bid orders we want the lower bound, bid orders are descending.
            if (side[mid].price > target_price) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
    }
    return left;
}

void OrderBook::found_price_update_side(const int index_to_update, std::array<BookLevel, ORDERBOOK_MAX_DEPTH>& side, int &side_size, const double quantity, const bool is_ask) {
    if (quantity == 0) { // remove the price from the side

        if (index_to_update < side_size - 1) {
            for (int i = index_to_update; i < side_size - 1; ++i) {
                side[i] = side[i + 1];
            }
        }
        side[side_size - 1] = {};

        side_size--;
    } else { // directly update the index
        side[index_to_update].quantity = quantity;
    }
};


bool OrderBook::is_updated_not_needed(const std::array<BookLevel, ORDERBOOK_MAX_DEPTH>& side, const int side_size, const double price, const double quantity, const bool is_ask) {
    if (ORDERBOOK_MAX_DEPTH == side_size) {
        if (is_ask && price > side[side_size - 1].price || !is_ask && price < side[side_size - 1].price) {
            return true;
        }
    }
    return false;
};


void OrderBook::not_found_price_update_side(const int index_to_update, std::array<BookLevel, ORDERBOOK_MAX_DEPTH>& side, int &side_size, const double price, const double quantity, const bool is_ask) {
    if (quantity == 0) return;

    // remove worst book level by isolating it from the memmove
    int index_to_copy_to = side_size;
    if (side_size == ORDERBOOK_MAX_DEPTH) {
        side_size--;
        index_to_copy_to -= 1;
    }

    // add new book level
    for (int i = index_to_copy_to - 1; i >= index_to_update; --i) {
        side[i + 1] = side[i];
    }

    side[index_to_update] = {price, quantity};

    // update side sizes
    side_size++;
};

void OrderBook::update_side(std::array<BookLevel, ORDERBOOK_MAX_DEPTH>& side, int &side_size, const double price, const double quantity, const bool is_ask) {
    if (is_updated_not_needed(side, side_size, price, quantity, is_ask)) return;

    if (const int index_to_update = search_side(price, side_size, side, is_ask);
        index_to_update < side_size && side[index_to_update].price == price) { // found the price in the side
       found_price_update_side(index_to_update, side, side_size, quantity, is_ask);
    } else {
        not_found_price_update_side(index_to_update, side, side_size, price, quantity, is_ask);
    }
};

