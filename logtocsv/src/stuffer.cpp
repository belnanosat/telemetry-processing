#include "stuffer.h"

#include <iostream>

using namespace std;

int unstuff_data(uint8_t *input_data, int length, uint8_t *output_data) {
	while (*input_data) {
		uint8_t *cur_data = input_data + 1;
		int block_len = *input_data;
		input_data += block_len;
		while (cur_data < input_data) {
			*output_data++ = *cur_data++;
		}
		if (*input_data) {
			if (block_len < 0xFF) {
			    *output_data++ = 0;
			} else {
				--length;
			}
		}
	}
	return length - 2;
}

// NOTE: This function assumees that length <= 255
int stuff_data(uint8_t *input_data, int length, uint8_t *output_data) {
	uint8_t *input_data_end = input_data + length;
	uint8_t *block_start = output_data++;
	uint8_t cur_code = 0x01;
	while (input_data < input_data_end) {
		if (*input_data == 0) {
			*block_start = cur_code;
			block_start = output_data++;
			cur_code = 0x01;
		} else {
			*(output_data++) = *input_data;
			++cur_code;
			if (cur_code == 0xFF) {
				++length;
				*block_start = cur_code;
				block_start = output_data++;
				cur_code = 0x01;
			}
		}
		++input_data;
	}
	*block_start++ = cur_code;
	*output_data = 0;
	return length + 2;
}
