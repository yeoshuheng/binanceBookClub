//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include <array>
#include <span>

constexpr size_t ORDERBOOK_MAX_DEPTH = 10;

struct BookLevel {
    double price;
    double quantity;

    [[nodiscard]] std::string to_string() const {
        return "price=" + std::to_string(price) + ", quantity=" + std::to_string(quantity);
    }
};

class OrderBook {

    /**
     * Represents bids, descending in nature.
     */
    std::array<BookLevel, ORDERBOOK_MAX_DEPTH> bid;

    /**
     * Represents asks, ascending in nature.
     */
    std::array<BookLevel, ORDERBOOK_MAX_DEPTH> ask;

    int bid_size;
    int ask_size;

    static void update_side(std::array<BookLevel, 10> &side, int &side_size, double price, double quantity, bool is_ask);

    /**
     * Checks if update can be inserted in the first place.
     */
    static bool is_updated_not_needed(const std::array<BookLevel, 10> &side, int side_size, double price, double quantity, bool is_ask);

    /**
     * Returns the position of the target price or the index, which is the smallest price greater than (ask) or biggest price smaller than (bid).
     * This is the position to apply updates to.
     */
    static int search_side(double target_price, int side_size, const std::array<BookLevel, ORDERBOOK_MAX_DEPTH>& side, bool is_ask);

    /**
     * Updates the order book if incoming price is already a level on the book.
     */
    static void found_price_update_side(int index_to_update, std::array<BookLevel, 10> &side, int &side_size, double quantity, bool is_ask);

    /**
     * Updates the order book if incoming price does not exist on the book.
     */
    static void not_found_price_update_side(int index_to_update, std::array<BookLevel, 10> &side, int &side_size, double price, double quantity, bool is_ask);

public:

    OrderBook();

    /**
     * Update functions for the order book.
     */
    void update_bid(double price, double quantity);
    void update_ask(double price, double quantity);

    /**
     * Display functions
     */
    std::string to_string() const;


    /**
     * Getters for span of bid & ask sides.
     */
    [[nodiscard]] std::span<const BookLevel> get_bid() const;
    [[nodiscard]] std::span<const BookLevel> get_ask() const;

    /**
     * Getters for book-level data like sizes & L1
     */
    double get_bid_size() const;
    double get_ask_size() const;
    double get_spread() const;
    double get_mid_price() const;
    double get_top_bid() const;
    double get_top_ask() const;
};


#endif //ORDERBOOK_H
