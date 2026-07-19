#include <iostream>
#include "order_book.h"
#include <map>
#include <cassert>


void error_with_log(std::string_view message){
    std::cerr << "Error: "<< message << std::endl;
    exit(1);
}

void OrderBook::add_order(AddOrder &o){
    Order ord {
        .ref = o.ref,
        .sym = o.stock,
        .add_timestamp = o.timestamp,
        .limit_price = o.price,
        .side = o.side,
        .add_shares = o.shares
    };
    this -> order_map[ord.ref] = ord;

    auto& book = (ord.side == 'B') ? bid : ask;
    auto& node = book[ord.sym][ord.limit_price];
    node.shares += ord.add_shares;
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
    auto &order_book_node = book.at(order_node.sym).at(order_node.limit_price);

    if (order_book_node.shares < e.executed_shares){
        error_with_log("book desynced"); 
    }

    order_book_node.shares -= e.executed_shares;
    if (order_book_node.shares == 0){
        book[order_node.sym].erase(order_node.limit_price);
    }
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
    auto &order_book_node = book.at(order_node.sym).at(order_node.limit_price);

    if (order_book_node.shares < e.executed_shares ){
        error_with_log("book desynced"); 
    }

    order_book_node.shares -= e.executed_shares;

    if (order_book_node.shares == 0){
        book[order_node.sym].erase(order_node.limit_price);
    }
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
    auto &book_node = book.at(order_node.sym).at(order_node.limit_price);

    book_node.shares -= c.cancelled_shares;
    if (book_node.shares == 0){
        book[order_node.sym].erase(order_node.limit_price);
    }
}

void OrderBook::delete_order(DeleteOrder &d){
    auto &order_node = this->order_map.at(d.ref);

    order_node.status = OrderStatus::CANCELLED;
    int shares_to_cancel = order_node.available_shares();
    order_node.cancel_shares += (shares_to_cancel);

    auto &book = (order_node.side == 'B' ? bid : ask);
    auto &book_node = book.at(order_node.sym).at(order_node.limit_price);

    book_node.shares -= (shares_to_cancel);
    if (book_node.shares == 0){
        book[order_node.sym].erase(order_node.limit_price);
    }
    this->order_map.erase(d.ref);
}

void OrderBook::replace_order(ReplaceOrder &r){
    auto &order_node = this->order_map.at(r.ref);

    DeleteOrder d {
        r.timestamp,
        r.ref
    };
    std::string sym = order_node.sym;
    char side = order_node.side;
    this->delete_order(d);

    AddOrder o {
        .timestamp = r.timestamp,
        .stock = sym, 
        .ref = r.new_ref, 
        .price = r.price,
        .shares = r.shares,
        .side = side
    };
    this->add_order(o);
}

book_map OrderBook::get_book(std::string sym, int levels){
    int count = 0;

    book_map bm {};

    auto bid_it = this->bid.find(sym);

    if (bid_it != this->bid.end()){ // for seeing if the sym exists
        int count = 0;
        for(auto it = bid_it->second.rbegin();  // it started from the end, according to the first value (sorted in the list)
                it != bid_it->second.rend() && count < levels; // obviously
                ++it, ++count){ //new syntax, comma seperated count++
            bm.bid.push_back({it->first, static_cast<uint32_t>(it->second.shares)});
        }
    }

    auto ask_it = this->ask.find(sym);

    if (ask_it != this->ask.end()){
        int count = 0;
        for(auto it = ask_it->second.rbegin();
                it != ask_it->second.rend() && count< levels;
                ++it, ++count){
                    bm.ask.push_back({it->first, static_cast<uint32_t>(it->second.shares)});
                }
    }
    return bm;
}