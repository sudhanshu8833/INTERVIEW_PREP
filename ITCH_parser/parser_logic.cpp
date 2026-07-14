#include <iostream>
#include <sys/mman.h>
#include <string_view>
#include <endian.h>
#include <cstdint>
#include <cstring>
#include <sstream>
#include "parser_logic.h"

uint32_t read_be32(const unsigned char* p) {
    uint32_t v;
    std::memcpy(&v, p, 4);
    return be32toh(v);
}

uint64_t read_be64(const unsigned char* p) {
    uint64_t v;
    std::memcpy(&v, p, 8);
    return be64toh(v);
}

uint16_t read_be16(const unsigned char* p){
    uint16_t v;
    std::memcpy(&v, p, 2);
    return be16toh(v);
}

uint64_t read_be48(const unsigned char* p){
    uint64_t v;
    for(int i = 0; i< 6; ++i){
        v = (v << 8) | p[i];
    }
    return v;
}



void AddOrder::print_struct(){
    std::ostringstream ss;
    ss<< "Add Order Information: "<<std::endl;
    ss<< "timestamp: "<< this->timestamp<<std::endl;
    ss<< "stock: "<< this->stock<<std::endl;
    ss<< "ref: " << this->ref << std::endl;
    ss<< "price: "<< this-> price << std::endl;
    ss << "shares: "<< this-> shares << std::endl;
    ss << "side: " << this-> side << std::endl;
    std::cout<< ss.str()<<std::endl;
}



void ExecutedOrder::print_struct(){
    std::ostringstream ss;
    ss<< "Executed Order Information: "<<std::endl;
    ss<< "timestamp: "<< this->timestamp<<std::endl;
    ss<< "ref: " << this->ref << std::endl;
    ss<< "executed_shares: "<< this->executed_shares << std::endl;
    std::cout<< ss.str()<<std::endl;
}

void ExecutedWithPriceOrder::print_struct(){
    std::ostringstream ss;
    ss<< "Executed With Price Order Information: "<<std::endl;
    ss<< "timestamp: "<< this->timestamp<<std::endl;
    ss<< "ref: " << this->ref << std::endl;
    ss<< "executed_shares: "<< this->executed_shares << std::endl;
    ss<< "execution_price: "<< this->execution_price << std::endl;
    std::cout<< ss.str()<<std::endl;
}

void CancelOrder::print_struct(){
    std::ostringstream ss;
    ss<< "Cancel Order Information: "<<std::endl;
    ss<< "timestamp: "<< this->timestamp<<std::endl;
    ss<< "ref: " << this->ref << std::endl;
    ss<< "cancelled_shares: "<< this->cancelled_shares << std::endl;
    std::cout<< ss.str()<<std::endl;
}

void DeleteOrder::print_struct(){
    std::ostringstream ss;
    ss<< "Delete Order Information: "<<std::endl;
    ss<< "timestamp: "<< this->timestamp<<std::endl;
    ss<< "ref: " << this->ref << std::endl;
    std::cout<< ss.str()<<std::endl;
}


void ReplaceOrder::print_struct(){
    std::ostringstream ss;
    ss<< "Replace Order Information: "<<std::endl;
    ss<< "timestamp: "<< this->timestamp<<std::endl;
    ss<< "ref: " << this->ref << std::endl;
    ss<< "new_ref: "<< this->new_ref << std::endl;
    ss<< "shares: "<< this->shares << std::endl;
    ss<< "price: "<< this->price << std::endl;
    std::cout<< ss.str()<<std::endl;
}

AddOrder ParseAddOrderMessage(const unsigned char* message){
    std::string_view sym(reinterpret_cast<const char*>(&message[0] + 24), 8);
    sym = sym.substr(0, sym.find_last_not_of(' ') + 1);
    AddOrder o{
        read_be48(&message[5]),
        std::string(sym),
        read_be64(&message[11]),
        static_cast<float>(read_be32(&message[32]) / 10000.0),
        read_be32(&message[20]),
        message[19]
    };
    return o;
}

ExecutedOrder ParseExecutedOrderMessage(const unsigned char* message){
    ExecutedOrder o{
        read_be48(&message[5]),
        read_be64(&message[11]),
        read_be32(&message[19])
    };
    return o;
}

ExecutedWithPriceOrder ParseExecutedWithPriceOrderMessage(const unsigned char* message){
    ExecutedWithPriceOrder o{
        read_be48(&message[5]),
        read_be64(&message[11]),
        read_be32(&message[19]),
        static_cast<float>(read_be32(&message[32]) / 10000.0)
    };
    return o;
}

CancelOrder ParseCancelOrder(const unsigned char* message){
    CancelOrder c{
        read_be48(&message[5]),
        read_be64(&message[11]),
        read_be32(&message[19])
    };
    return c;
}
DeleteOrder ParseDeleteOrder(const unsigned char* message){
    DeleteOrder d{
        read_be48(&message[5]),
        read_be64(&message[11])
    };
    return d;
}

ReplaceOrder ParseReplaceOrder(const unsigned char* message){
    ReplaceOrder o{
        read_be48(&message[5]),
        read_be64(&message[11]),
        read_be64(&message[19]),
        read_be32(&message[27]),
        static_cast<float>(read_be32(&message[31]) / 10000.0)
    };
    return o;
}







