#include <iostream>
#include "order_book.h"
#include <map>
#include <cassert>


void error_with_log(std::string_view message){
    std::cerr << "Error: "<< message << std::endl;
    exit(1);
}

void OrderBook::add_to_book(std::vector<OrderBookNode> &book_array, uint32_t limit_price, uint32_t shares){
    int i = 0;
    for(auto &p: book_array){
        if (p.price == limit_price){
            p.shares += shares;
        } else if (p.price > limit_price){
            OrderBookNode node {
                .shares = shares,
                .price = limit_price,
            };
            book_array.insert(book_array.begin() + i, node);
        }
        i++;
    }
}

void OrderBook::delete_from_book(std::vector<OrderBookNode> &book_array, uint32_t limit_price, uint32_t shares){
    for(auto p = book_array.begin(); p != book_array.end();){
        if (p->price == limit_price){
            p->shares -= shares;

            if (p->shares == 0){
                book_array.erase(p);
                continue;
            }
            if (p->shares < 0){
                error_with_log("book desynced");
            }
        } else if (p->price > limit_price){
            error_with_log("limit price not present");
        }
        ++p;
    }
}

void OrderBook::add_order(AddOrder &o){
    Order ord {
        .stock_locate = o.stock_locate,
        .ref = o.ref,
        .sym = o.stock,
        .add_timestamp = o.timestamp,
        .limit_price = o.price,
        .side = o.side,
        .add_shares = o.shares
    };
    this -> order_map[ord.ref] = ord;

    auto& book = (ord.side == 'B') ? bid : ask;
    auto& book_array = book[ord.stock_locate];
    add_to_book(book_array, ord.limit_price, ord.add_shares);
}

void OrderBook::execute_order(ExecutedOrder &e){
    auto &order_node = this->order_map.at(e.ref);
    assert(order_node.status != OrderStatus::EXECUTED && "Got execute order for already executed order");

    order_node.execute_shares += e.executed_shares;
    order_node.execute_timestamp = e.timestamp;

    if (order_node.execute_shares == (order_node.add_shares - order_node.cancel_shares)) {
        order_node.price_executed = order_node.limit_price;
        order_node.status = OrderStatus::EXECUTED;
    } else{
        order_node.status = OrderStatus::PARTIAL_EXECUTED;
    }

    auto &book = (order_node.side == 'B'? bid : ask);
    auto &book_array = book.at(order_node.stock_locate);

    delete_from_book(book_array, order_node.limit_price, e.executed_shares);
}

void OrderBook::execute_order_with_price(ExecutedWithPriceOrder &e){
    auto &order_node = this->order_map.at(e.ref);
    assert(order_node.status != OrderStatus::EXECUTED && "Got execute order for already executed order");

    order_node.execute_shares += e.executed_shares;
    order_node.execute_timestamp = e.timestamp;


    if (order_node.execute_shares == (order_node.add_shares - order_node.cancel_shares)) {
        order_node.price_executed = e.execution_price;
        order_node.status = OrderStatus::EXECUTED;
    } else{
        order_node.status = OrderStatus::PARTIAL_EXECUTED;
    }


    auto &book = (order_node.side == 'B'? bid : ask);
    auto &book_array = book.at(order_node.stock_locate);

    delete_from_book(book_array, order_node.limit_price, e.executed_shares);
}

void OrderBook::cancel_order(CancelOrder &c){

    auto &order_node = this->order_map.at(c.ref);

    if (order_node.status == OrderStatus::PARTIAL_EXECUTED) {
        if (c.cancelled_shares > order_node.available_shares()) {
            error_with_log("cancelling more then available shares");
        }
        order_node.cancel_shares += c.cancelled_shares;
    } else if (order_node.status == OrderStatus::EXECUTED){
        error_with_log("Cancel request for the order which is already completely executed");
    } else if (order_node.status == OrderStatus::CANCELLED){
        error_with_log("Cancel request for the order which is already completely cancelled");
    } else {
        order_node.cancel_shares += c.cancelled_shares;
        if (order_node.add_shares == order_node.cancel_shares){
            order_node.status = OrderStatus::CANCELLED;
        }
    }

    auto &book = (order_node.side == 'B' ? bid : ask);
    auto &book_array = book.at(order_node.stock_locate);

    delete_from_book(book_array, order_node.limit_price, c.cancelled_shares);
}

void OrderBook::delete_order(DeleteOrder &d){
    auto &order_node = this->order_map.at(d.ref);

    order_node.status = OrderStatus::CANCELLED;
    int shares_to_cancel = order_node.available_shares();
    order_node.cancel_shares += (shares_to_cancel);

    auto &book = (order_node.side == 'B' ? bid : ask);
    auto &book_array = book.at(order_node.stock_locate);

    delete_from_book(book_array, order_node.limit_price, order_node.available_shares());
    this->order_map.erase(d.ref);
}

void OrderBook::replace_order(ReplaceOrder &r){
    auto &order_node = this->order_map.at(r.ref);

    DeleteOrder d {
        .stock_locate = order_node.stock_locate,
        .timestamp = r.timestamp,
        .ref = r.ref
    };
    std::string sym = order_node.sym;
    char side = order_node.side;
    this->delete_order(d);

    AddOrder o {
        .stock_locate = r.stock_locate,
        .timestamp = r.timestamp,
        .stock = sym, 
        .ref = r.new_ref, 
        .price = r.price,
        .shares = r.shares,
        .side = side
    };
    this->add_order(o);
}

book_map OrderBook::get_book(uint16_t stock_locate, int levels){
    book_map bm {};

    auto bid_book = this->bid.find(stock_locate);

    if (bid_book != this->bid.end()){
        int count = 0;
        auto &bid_book_array = bid_book -> second;
        for(auto it = bid_book_array.rbegin(); it != bid_book_array.rend() && count< levels; ++it, ++count){
            bm.bid.push_back({it->price, it->shares});
        }    
    }

    auto ask_book = this->ask.find(stock_locate);
    if (ask_book != this->ask.end()){
        int count = 0;
        auto &ask_book_array = ask_book -> second;
        for(auto it = ask_book_array.rbegin(); it != ask_book_array.rend() && count< levels; ++it, ++count){
            bm.ask.push_back({it->price, it->shares});
        }    
    }
    return bm;
}