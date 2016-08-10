#include<iostream>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<sstream>
#include<string>

int main() {
	uint32_t hex_ip = 0xc0a83865;
	uint32_t tmp1 = (hex_ip & 0xff000000) >> 24;
	uint32_t tmp2 = (hex_ip & 0xff0000) >> 16;
	uint32_t tmp3 = (hex_ip & 0xff00) >> 8;
	uint32_t tmp4 = hex_ip & 0xff;
	char hex_ip_str[16];
 	sprintf(hex_ip_str, "%d.%d.%d.%d", tmp1, tmp2, tmp3, tmp4);
	std::cout << "ip= " << hex_ip_str << std::endl;
	return 0;
}
