#include <iostream>
#include <bitset>
#include <string>
#include <bitset>
using namespace std;



int main(){

	// bitset<8> a("11001010");
	std::bitset<8> a("00000001");
	unsigned long val = a.to_ulong();      // or a.to_ullong()
	char c = static_cast<char>(a.to_ulong()); // now this works — to_ulong() returns a real integer
	std::string s = a.to_string();          // "11001010" the string, not the numeric value
	cout<<c<<endl;
	return 0;
}