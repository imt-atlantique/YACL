#include "CBORArray.h"

bool CBORArray::init_buffer()
{
	//Reserve begining of buffer to store table length
	ext_buffer_begin = (uint8_t*)malloc(sizeof(uint8_t)*max_buf_len);
	if (ext_buffer_begin == NULL) {
		buffer_data_begin = NULL;
		buffer_begin = NULL;
		buffer = NULL;

		ext_buffer_flag = false;

		return false;
	}

	buffer_data_begin = ext_buffer_begin + 9;
	buffer_begin = ext_buffer_begin + 8; //type_num_len is 1 for array size 0
	buffer = buffer_data_begin;

	ext_buffer_flag = false;

	return true;
}

void CBORArray::init_num_ele(size_t num_ele)
{
	//Save buffer
	uint8_t *old_buffer = buffer;

	//Put buffer pointer in the right position, considering the size of the
	//length field
	buffer = buffer_data_begin - compute_type_num_len(num_ele);
	//Update buffer_begin
	buffer_begin = buffer;

	//Encode num_ele
	encode_type_num(CBOR_ARRAY, num_ele);

	//Put buffer back into its inital position
	buffer = old_buffer;

	return;
}

//Note: number of element cannot exceed (2^64-1)
CBORArray::CBORArray(size_t buf_len)
{
	//Reserve buffer
	max_buf_len = buf_len;
	init_buffer();

	//Initialize num_ele
	init_num_ele(0);
}

CBORArray::CBORArray(const CBORArray &obj)
{
	uint8_t type_num_len = compute_type_num_len(obj.n_elements());

	//Reserve buf_len plus maximum size of a type_num : 9 bytes to encode a
	//table length up to (2^64)-1
	max_buf_len = obj.length() - type_num_len + 9;

	init_buffer();

	//Reserve begining of buffer to store table length
	buffer_begin = ext_buffer_begin + 9 - type_num_len;
	buffer = buffer_begin + obj.length();

	//Copy num_ele and data
	memcpy(buffer_begin, obj.to_CBOR(), obj.length());
}

CBORArray::CBORArray(uint8_t* _buffer, size_t buffer_len, bool has_data)
{
	max_buf_len = buffer_len;
	ext_buffer_flag = true;

	if (has_data) {
		ext_buffer_begin = _buffer;
		buffer_begin = _buffer;

		buffer_data_begin = buffer_begin + compute_type_num_len(n_elements());

		//Jump to the end of the data chunk
		buffer = buffer_data_begin;
		for (size_t i=0 ;  i < n_elements() ; ++i) {
			jump();
		}
	}
	else {
		//Reserve begining of buffer to store table length
		ext_buffer_begin = _buffer;
		buffer_data_begin = _buffer + 9;
		buffer_begin = _buffer + 8; //type_num_len is 1 for array size 0
		buffer = buffer_data_begin;

		//Initialize num_ele
		init_num_ele(0);
	}
}

CBORArray::CBORArray(const CBOR &obj)
{
	max_buf_len = obj.length();
	ext_buffer_flag = true;

	ext_buffer_begin = obj.to_CBOR();
	buffer_begin = ext_buffer_begin;

	buffer_data_begin = buffer_begin + compute_type_num_len(n_elements());

	//Jump to the end of the data chunk
	buffer = buffer_data_begin;
	for (size_t i=0 ;  i < n_elements() ; ++i) {
		jump();
	}
}

CBORArray::~CBORArray()
{
	if(!ext_buffer_flag) {
		free(ext_buffer_begin);
	}
}

bool CBORArray::append()
{
	increment_num_ele();
	return add();
}

CBOR CBORArray::operator[](size_t idx)
{
	//Save buffer position
	uint8_t *old_buffer = buffer;
	uint8_t *ele_begin = NULL;
	size_t ele_size = 0;

	if (!is_array() && idx < n_elements()) {
		return CBOR();
	}

	//Put buffer on the first element of the table
	buffer = buffer_data_begin;

	//Jump to the reffered element
	for (size_t i=0 ; i < idx ; ++i) {
		jump();
	}
	ele_begin = buffer;
	ele_size = jump();

	//Put back buffer in its initial position
	buffer = old_buffer;

	return CBOR(ele_begin, ele_size, true);
}
