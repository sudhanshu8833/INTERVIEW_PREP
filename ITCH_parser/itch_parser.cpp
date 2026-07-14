#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <endian.h>
#include <cstring>
#include <sstream>
#include <cstdint>
#include <cstdio>
#include "parser_logic.h"
#include <string_view>

int main(){
    int fd = open("sample_itch50.bin", O_RDONLY);
    if (fd < 0){
        std::cout<<"File Reading Failed"<<std::endl;
        exit(1);
    }

    struct stat st;
    fstat(fd, &st);
    size_t size = st.st_size;
    
    const unsigned char* data = static_cast<const unsigned char*>(
        mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (data == MAP_FAILED){
        std::cout<<"data conversion failed"<<std::endl;
        exit(1);
    }

    int i =0;
    while(i < size){
        int length = read_be16(&data[i]);
        i+=2;
        switch(data[i]){
            case 'A':
            case 'F': {
                AddOrder o = ParseAddOrderMessage(&data[i]);
                o.print_struct();
                break;
            }
            case 'E': {
                ExecutedOrder e = ParseExecutedOrderMessage(&data[i]);
                e.print_struct();
                break;
            }
            case 'C': {
                ExecutedWithPriceOrder e = ParseExecutedWithPriceOrderMessage(&data[i]);
                e.print_struct();
                break;
            }
            case 'X': {
                CancelOrder c = ParseCancelOrder(&data[i]);
                c.print_struct();
                break;
            }
            case 'D': {
                DeleteOrder d = ParseDeleteOrder(&data[i]);
                d.print_struct();
                break;
            }
            case 'U': {
                ReplaceOrder r = ParseReplaceOrder(&data[i]);
                r.print_struct();
                break;
            }
        }
        i+=length;
    }

    return 0;
}