//
// Created by Yeo Shu Heng on 3/7/25.
//

#include "./OrderBook.h"
#include <sstream>

/**
 * Main user functions.
 */

OrderBook::OrderBook(): bid_price(), ask_price(), bid_qty(), ask_qty(), bid_size(0), ask_size(0) {}

void OrderBook::update_bid(const float price, const float quantity) {
    update_side(bid_price, bid_qty, bid_size, price, quantity, false);
}

void OrderBook::update_ask(const float price, const float quantity) {
    update_side(ask_price, ask_qty, ask_size, price, quantity, true);
}

float OrderBook::get_top_bid() const {return bid_price[0];};

float OrderBook::get_top_ask() const {return ask_price[0];};

float OrderBook::get_spread() const {return get_top_ask() - get_top_bid();};

float OrderBook::get_mid_price() const {return get_top_ask() + (get_spread() / 2);};

float OrderBook::get_bid_size() const {return bid_size;};

float OrderBook::get_ask_size() const {return ask_size;};


std::string OrderBook::to_string() const {
    std::stringstream ss;
    ss << "l1={" << "bid=" << std::to_string(get_top_bid()) << ", ask=" << std::to_string(get_top_ask()) << ", mid=" << std::to_string(get_mid_price()) << "}\n";
    ss << "asks={" << "\n";
    for (int i = 0; i < ask_size; i++) {
     ss << "{" << "price=" << std::to_string(ask_price[i]) << ", qty=" << std::to_string(ask_qty[i]) << "},\n";
    }
    ss << "}" << "\n";
    ss << "bids={" << "\n";
    for (int i = 0; i < bid_size; i++) {
        ss << "{" << "price=" << std::to_string(bid_price[i]) << ", qty=" << std::to_string(bid_qty[i]) << "},\n";
    }
    ss << "}" << "\n";
    return ss.str();
}

/**
 * Functions for order book update operations.
 */

int OrderBook::search_side_linear(const float target_price, const int side_size, const std::array<float, ORDERBOOK_MAX_DEPTH>& __restrict side_price, bool is_ask) {
    if (is_ask) {
        for (int i = 0; i < side_size; ++i) {
            if (side_price[i] >= target_price) {
                return i;
            }
        }
    } else {
        for (int i = 0; i < side_size; ++i) {
            if (side_price[i] <= target_price) {
                return i;
            }
        }
    }
    return side_size;
}

int OrderBook::search_side(const float target_price, const int side_size, const std::array<float, ORDERBOOK_MAX_DEPTH>& __restrict side_price, bool is_ask) {
    int left = 0; int right = side_size; int mid = 0;
    while (left < right) {
        mid = left + (right - left) / 2;
        if (side_price[mid] == target_price) {
            return mid;
        }
        if (is_ask) { // for ask orders we want the upper bound, ask orders are ascending.
            if (side_price[mid] > target_price) {
                right = mid;
            } else {
                left = mid + 1;
            }
        } else { // for bid orders we want the lower bound, bid orders are descending.
            if (side_price[mid] > target_price) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
    }
    return left;
}

void OrderBook::found_price_update_side(const int index_to_update, std::array<float, ORDERBOOK_MAX_DEPTH>& __restrict side_price, std::array<float, ORDERBOOK_MAX_DEPTH>& __restrict side_qty, int &side_size, const float quantity, const bool is_ask) {
    if (quantity == 0) { // remove the price from the side

        if (index_to_update < side_size - 1) {
            std::memmove(&side_price[index_to_update], &side_price[index_to_update + 1], sizeof(float) * (side_size - 1 - index_to_update));
            std::memmove(&side_qty[index_to_update], &side_qty[index_to_update + 1], sizeof(float) * (side_size - 1 - index_to_update));
        }

        side_price[side_size - 1] = 0;
        side_qty[side_size - 1] = 0;

        side_size--;
    } else { // directly update the index
        side_qty[index_to_update] = quantity;
    }
};


bool OrderBook::is_updated_not_needed(const std::array<float, ORDERBOOK_MAX_DEPTH>& __restrict side_price, const int side_size, const float price, const float quantity, const bool is_ask) {
    if (ORDERBOOK_MAX_DEPTH == side_size) {
        if (is_ask && price > side_price[side_size - 1] || !is_ask && price < side_price[side_size - 1]) {
            return true;
        }
    }
    return false;
};


void OrderBook::not_found_price_update_side(const int index_to_update, std::array<float, ORDERBOOK_MAX_DEPTH>& __restrict side_price, std::array<float, ORDERBOOK_MAX_DEPTH>& __restrict side_qty, int &side_size, const float price, const float quantity, const bool is_ask) {
    if (quantity == 0) return;

    // remove worst book level by isolating it from the memmove
    int index_to_copy_to = side_size;
    if (side_size == ORDERBOOK_MAX_DEPTH) {
        side_size--;
        index_to_copy_to -= 1;
    }

    // add new book level
    std::memmove(&side_price[index_to_update + 1], &side_price[index_to_update], sizeof(float) * (index_to_copy_to - index_to_update));
    std::memmove(&side_qty[index_to_update + 1], &side_qty[index_to_update], sizeof(float) * (index_to_copy_to - index_to_update));

    side_price[index_to_update] = price;
    side_qty[index_to_update] = quantity;

    // update side sizes
    side_size++;
};

void OrderBook::update_side(std::array<float, ORDERBOOK_MAX_DEPTH>& __restrict side_price, std::array<float, ORDERBOOK_MAX_DEPTH>& __restrict side_qty, int &side_size, const float price, const float quantity, const bool is_ask) {
    if (is_updated_not_needed(side_price, side_size, price, quantity, is_ask)) return;

    if (int index_to_update = search_side_linear(price, side_size, side_price, is_ask);
        index_to_update < side_size && side_price[index_to_update] == price) { // found the price in the side
       found_price_update_side(index_to_update, side_price, side_qty, side_size, quantity, is_ask);
    } else {
        not_found_price_update_side(index_to_update, side_price, side_qty, side_size, price, quantity, is_ask);
    }
};

