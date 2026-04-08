#include <vector>
#include <list>
#include <fmt/core.h>
#include <stdexcept>
#include <cstdint>

enum class OrderType {
    GoodTillCancel,
    FillAndKill
};

enum class Side {
    Buy,
    Sell
};

using Price = std::int32_t; // think about ticks
using Quantity = std::uint32_t; // think about actual qty's that are in orders
using OrderId = std::uint64_t; // think about reqs/s

class Order {
public:
    Order(OrderType order_type, OrderId order_id, Side side, Price price, Quantity quantity)
        : type_{order_type},
          id_{order_id},
          side_{side},
          price_{price},
          initial_quantity_{quantity},
          remaining_quantity_{quantity} {}
    
    OrderType get_type() const { return type_; }
    OrderId get_id() const { return id_; }
    Side get_side() const { return side_; }
    Price get_price() const { return price_; }
    Quantity get_initial_quantity() const { return initial_quantity_; }
    Quantity get_remaining_quantity() const { return remaining_quantity_; }
    Quantity get_filled_quantity() const { return initial_quantity_ - remaining_quantity_; }
    bool is_filled() const { return remaining_quantity_ == 0; }

    void fill(Quantity quantity) {
        if (quantity > get_remaining_quantity()) {
            throw std::logic_error(fmt::format("ERROR: Order {} CANT be filled. to fill qty: {} more than remaining qty: {}", get_id(), quantity, get_remaining_quantity()));
        }
        remaining_quantity_ -= quantity;
    }

private:
    OrderType type_;
    OrderId id_;
    Side side_;
    Price price_;
    Quantity initial_quantity_;
    Quantity remaining_quantity_;
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

class OrderModfiy {
public:
    OrderModfiy(OrderId order_id, Side side, Price price, Quantity quantity)
        : id_{order_id},
          price_{price},
          side_{side},
          quantity_{quantity} {}
    OrderId get_id() const { return id_; }
    Price get_price() const { return price_; }
    Side get_side() const { return side_; }
    Quantity get_quantity() const { return quantity_; }

    OrderPointer to_order_pointer(OrderType type) const {
        return std::make_shared<Order>(type, get_id(), get_side(), get_price(), get_quantity());
    }

private:
    OrderId id_;
    Price price_;
    Side side_;
    Quantity quantity_;
};

class TradeInfo {
    OrderId id_;
    Price price_;
    Quantity quantity_;
};

class Trade {
public:
    Trade(const TradeInfo& bid_trade, const TradeInfo& ask_trade)
        : bid_trade_{bid_trade},
          ask_trade_{ask_trade} {}
    const TradeInfo& get_bid_trade() const { return bid_trade_; }
    const TradeInfo& get_ask_trade() const { return ask_trade_; }

private:
    TradeInfo bid_trade_;
    TradeInfo ask_trade_;
};

struct LevelInfo {
    Price price_;
    Quantity total_quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderBookLevelInfos {
private:
    LevelInfos bids_; // vector<LevelInfo>
    LevelInfos asks_;

public:
    OrderBookLevelInfos(const LevelInfos& asks, const LevelInfos& bids)
        : bids_(bids),
          asks_(asks) {}
    
    const LevelInfos& get_bids() const { return bids_; }
    const LevelInfos& get_asks() const { return asks_; }
};

class OrderBook {
    // TODO: 
};

int main() {
    // test
    // #include <set>
    // std::set s{1, 2, 3};
    // s.contains(1);
}