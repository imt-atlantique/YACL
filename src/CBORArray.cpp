#include "CBORArray.h"

CBORArray::CBORArray(uint8_t* _buffer, size_t buffer_len, bool has_data)
{
	max_buf_len = buffer_len;
	buffer_type = BUFFER_EXTERNAL;

	if (has_data) {
		ext_buffer_begin = _buffer;
		buffer_begin = _buffer;

		buffer_data_begin = buffer_begin + compute_type_num_len(n_elements());

		//Jump to the end of the data chunk
		w_ptr = buffer_data_begin;
		for (size_t i=0 ;  i < n_elements() ; ++i) {
			w_ptr += element_size(w_ptr);
		}
	}
	else {
		//Reserve begining of buffer to store table length
		ext_buffer_begin = _buffer;
		buffer_data_begin = _buffer + NUM_ELE_PROVISION;
		buffer_begin = _buffer + NUM_ELE_PROVISION - 1; //type_num_len is 1 for array size 0
		w_ptr = buffer_data_begin;

		//Initialize num_ele
		init_num_ele(0);
	}
}

CBORArray::CBORArray(const uint8_t* _buffer, size_t buf_len)
{
	size_t _num_ele = decode_abs_num(_buffer);

	//Init buffer
	max_buf_len = buf_len + NUM_ELE_PROVISION;
	init_buffer();

	//Initialize number of elements, and copy data
	init_num_ele(_num_ele);
	memcpy(w_ptr, _buffer + compute_type_num_len(_num_ele),
			buf_len - compute_type_num_len(_num_ele));

	//Jump to the end of the data chunk
	for (size_t i=0 ;  i < n_elements() ; ++i) {
		w_ptr += element_size(w_ptr);
	}
}

bool CBORArray::append()
{
	increment_num_ele();
	return add();
}
