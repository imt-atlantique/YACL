#include "YACL.h"

bool test_bool_true()
{
	uint8_t cbor_data[] = {0xF5};
	CBOR cbor = CBOR(cbor_data, 1);

	if (cbor.is_bool() && ((bool)cbor == true)) {
		return true;
	}

	return false;
}

bool test_bool_false()
{
	uint8_t cbor_data[] = {0xF4};
	CBOR cbor = CBOR(cbor_data, 1);

	if (cbor.is_bool() && ((bool)cbor == false)) {
		return true;
	}

	return false;
}

bool test_negint1()
{
	uint8_t cbor_data[] = {0x20};
	CBOR cbor = CBOR(cbor_data, 1);

	if (cbor.is_int8() && ((int8_t)cbor == -1)) {
		return true;
	}

	return false;
}

bool test_negint2()
{
	uint8_t cbor_data[] = {0x39, 0x03, 0xE7};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_int16() && ((int16_t)cbor == -1000)) {
		return true;
	}

	return false;
}

bool test_negint3()
{
	uint8_t cbor_data[] = {0x3A, 0x3B, 0x9A, 0xC9, 0xFF};
	CBOR cbor = CBOR(cbor_data, 5);

	if (cbor.is_int32() && ((int32_t)cbor == -1000000000)) {
		return true;
	}

	return false;
}

bool test_negint4()
{
	uint8_t cbor_data[] = {0x3B, 0x00, 0x03, 0x8D, 0x7E, 0xA4, 0xC6, 0x7F, 0xFF};
	CBOR cbor = CBOR(cbor_data, 9);

	if (cbor.is_int64() && ((int64_t)cbor == -1000000000000000)) {
		return true;
	}

	return false;
}

bool test_int0()
{
	uint8_t cbor_data[] = {0x17};
	CBOR cbor = CBOR(cbor_data, 1);

	if (cbor.is_uint8() && ((uint8_t)cbor == 23)) {
		return true;
	}

	return false;
}

bool test_int1()
{
	uint8_t cbor_data[] = {0x19, 0x03, 0xe8};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_uint16() && ((uint16_t)cbor == 1000)) {
		return true;
	}

	return false;
}

bool test_int2()
{
	uint8_t cbor_data[] = {0x1a, 0x3b, 0x9a, 0xca, 0x00};
	CBOR cbor = CBOR(cbor_data, 5);

	if (cbor.is_uint32() && ((uint32_t)cbor == 1000000000)) {
		return true;
	}

	return false;
}

bool test_int3()
{
	uint8_t cbor_data[] = {0x1a, 0x8d, 0x9f, 0x57, 0x30};
	CBOR cbor = CBOR(cbor_data, 5);

	if (cbor.is_uint32() && ((uint32_t)cbor == 2376030000)) {
		return true;
	}

	return false;
}

bool test_int4()
{
	uint8_t cbor_data[] = {0x1b, 0x00, 0x03, 0x8d, 0x7e, 0xa4, 0xc6, 0x80, 0x00};
	CBOR cbor = CBOR(cbor_data, 9);

	if (cbor.is_uint64() && ((uint64_t)cbor == 1000000000000000)) {
		return true;
	}

	return false;
}

bool test_neghalf()
{

	const uint8_t cbor_data[] = {0xf9, 0x80, 0x00};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && ((float)cbor == -0.0)) {
		return true;
	}

	return false;
}

bool test_neghalf1()
{

	const uint8_t cbor_data[] = {0xf9, 0xC4, 0x00};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && ((float)cbor == -4.0)) {
		return true;
	}

	return false;
}


bool test_half()
{
	const uint8_t cbor_data[] = {0xf9, 0x3c, 0x00};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && ((float)cbor == 1.0)) {
		return true;
	}

	return false;
}

bool test_half1()
{

	const uint8_t cbor_data[] = {0xf9, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && ((float)cbor == 0.0)) {
		return true;
	}

	return false;
}

bool test_half2()
{

	const uint8_t cbor_data[] = {0xf9, 0x7B, 0xFF};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && ((float)cbor == 65504.0)) {
		return true;
	}

	return false;
}

bool test_half3()
{

	const uint8_t cbor_data[] = {0xf9, 0x3E, 0x00};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && ((float)cbor == 1.5)) {
		return true;
	}

	return false;
}

bool test_half4()
{

	const uint8_t cbor_data[] = {0xf9, 0x00, 0x01};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && ((float)cbor == 5.960464477539063e-8)) {
		return true;
	}

	return false;
}

bool test_half5()
{

	const uint8_t cbor_data[] = {0xf9, 0x04, 0x00};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && ((float)cbor == 0.00006103515625)) {
		return true;
	}

	return false;
}

bool test_half_inf()
{

	const uint8_t cbor_data[] = {0xf9, 0x7C, 0x00};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && ((float)cbor == INFINITY)) {
		return true;
	}

	return false;
}

bool test_neghalf_inf()
{

	const uint8_t cbor_data[] = {0xf9, 0xFC, 0x00};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && ((float)cbor == -INFINITY)) {
		return true;
	}

	return false;
}

bool test_half_nan()
{

	const uint8_t cbor_data[] = {0xf9, 0x7E, 0x00};
	CBOR cbor = CBOR(cbor_data, 3);

	if (cbor.is_float16() && isnan((float)cbor)) {
		return true;
	}

	return false;
}

bool test_negfloat()
{
	const uint8_t cbor_data[] = {0xfa, 0xbf, 0xc0, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 5);

	if (cbor.is_float32() && ((float)cbor == -1.5)) {
		return true;
	}

	return false;
}

bool test_negfloat_inf()
{
	const uint8_t cbor_data[] = {0xfa, 0xff, 0x80, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 5);

	if (cbor.is_float64() && ((float)cbor == -INFINITY)) {
		return true;
	}

	return false;
}

bool test_float_nan()
{
	const uint8_t cbor_data[] = {0xfa, 0x7f, 0xc0, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 5);

	if (cbor.is_float64() && isnan((float)cbor)) {
		return true;
	}

	return false;
}

bool test_float()
{
	const uint8_t cbor_data[] = {0xfa, 0x47, 0xc3, 0x50, 0x00};
	CBOR cbor = CBOR(cbor_data, 5);

	if (cbor.is_float32() && ((float)cbor == 100000.0)) {
		return true;
	}

	return false;
}

bool test_float1()
{
	const uint8_t cbor_data[] = {0xfa, 0x3f, 0xc0, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 5);

	if (cbor.is_float64() && ((float)cbor == 1.5)) {
		return true;
	}

	return false;
}

bool test_float2()
{
	const uint8_t cbor_data[] = {0xfa, 0x7f, 0x7f, 0xff, 0xff};
	CBOR cbor = CBOR(cbor_data, 5);

	if (cbor.is_float64() && ((float)cbor == 3.4028234663852886e+38)) {
		return true;
	}

	return false;
}

bool test_float_inf()
{
	const uint8_t cbor_data[] = {0xfa, 0x7f, 0x80, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 5);

	if (cbor.is_float64() && ((float)cbor == INFINITY)) {
		return true;
	}

	return false;
}

bool test_negdouble()
{
	const uint8_t cbor_data[] = {0xfb, 0xbf, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 9);

	if (cbor.is_float64() && ((double)cbor == -1.5)) {
		return true;
	}

	return false;
}

bool test_negdouble1()
{
	const uint8_t cbor_data[] = {0xfb, 0xc0, 0x10, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66};
	CBOR cbor = CBOR(cbor_data, 9);

	if (cbor.is_float64() && ((double)cbor == -4.1)) {
		return true;
	}

	return false;
}

bool test_double()
{
	const uint8_t cbor_data[] = {0xfb, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 9);

	if (cbor.is_float64() && ((double)cbor == 1.5)) {
		return true;
	}

	return false;
}

bool test_double1()
{
	const uint8_t cbor_data[] = {0xfb, 0x3f, 0xf1, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a};
	CBOR cbor = CBOR(cbor_data, 9);

	Serial.println((double)cbor);
	if (cbor.is_float64() && ((double)cbor == 1.1)) {
		return true;
	}

	return false;
}

bool test_double_inf()
{
	const uint8_t cbor_data[] = {0xfb, 0x7f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 9);

	if (cbor.is_float64() && ((double)cbor == INFINITY)) {
		return true;
	}

	return false;
}

bool test_negdouble_inf()
{
	const uint8_t cbor_data[] = {0xfb, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 9);

	if (cbor.is_float64() && ((double)cbor == -INFINITY)) {
		return true;
	}

	return false;
}

bool test_double_nan()
{
	const uint8_t cbor_data[] = {0xfb, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	CBOR cbor = CBOR(cbor_data, 9);

	if (cbor.is_float64() && isnan((double)cbor)) {
		return true;
	}

	return false;
}

bool test_string_char1()
{
	const uint8_t cbor_data[] = {0x68, 0x61, 0x6f, 0x65, 0x75, 0x31, 0x32, 0x33, 0x34};
	CBOR cbor = CBOR(cbor_data, 9);
	const char* ref_str = "aoeu1234";
	char decoded_str[10] = "";

	cbor.get_string(decoded_str);
	if (cbor.is_string() && (cbor.get_string_len() == strlen(ref_str)) \
		 && (strcmp(decoded_str, ref_str) == 0)) {
		return true;
	}

	return false;
}

bool test_string_char2()
{
	const uint8_t cbor_data[] = {0x74, 0xc3, 0xa5, 0xc3, 0xb6, 0xc3, 0xa9, 0xc3, 0xbb, 0xe3, 0x81, 0xae, 0xe3, 0x81, 0x8b, 0xe3, 0x82, 0x81, 0xe4, 0xba, 0x80};
	CBOR cbor = CBOR(cbor_data, 21);
	const char* ref_str = "åöéûのかめ亀";
	char decoded_str[21] = "";

	cbor.get_string(decoded_str);
	if (cbor.is_string() && (cbor.get_string_len() == strlen(ref_str)) \
		 && (strcmp(decoded_str, ref_str) == 0)) {
		return true;
	}

	return false;
}

bool test_string_char3()
{
	const uint8_t cbor_data[] = {0x78, 0x1A, 0x4C, 0x6F, 0x72, 0x65, 0x6D, 0x20, 0x69, 0x70, 0x73, 0x75, 0x6D, 0x20, 0x64, 0x6F, 0x6C, 0x6F, 0x72, 0x20, 0x73, 0x69, 0x74, 0x20, 0x61, 0x6D, 0x65, 0x74};
	CBOR cbor = CBOR(cbor_data, 28);
	const char* ref_str = "Lorem ipsum dolor sit amet";
	char decoded_str[27] = "";

	cbor.get_string(decoded_str);
	if (cbor.is_string() && (cbor.get_string_len() == strlen(ref_str)) \
		 && (strcmp(decoded_str, ref_str) == 0)) {
		return true;
	}

	return false;
}

bool test_string_string1()
{
	const uint8_t cbor_data[] = {0x68, 0x61, 0x6f, 0x65, 0x75, 0x31, 0x32, 0x33, 0x34};
	CBOR cbor = CBOR(cbor_data, 9);
	String ref_str = "aoeu1234";
	String decoded_str;

	cbor.get_string(decoded_str);
	if (cbor.is_string() && (cbor.get_string_len() == ref_str.length()) \
		 && (decoded_str == ref_str)) {
		return true;
	}

	return false;
}

bool test_string_string2()
{
	const uint8_t cbor_data[] = {0x74, 0xc3, 0xa5, 0xc3, 0xb6, 0xc3, 0xa9, 0xc3, 0xbb, 0xe3, 0x81, 0xae, 0xe3, 0x81, 0x8b, 0xe3, 0x82, 0x81, 0xe4, 0xba, 0x80};
	CBOR cbor = CBOR(cbor_data, 21);
	String ref_str = "åöéûのかめ亀";
	String decoded_str;

	cbor.get_string(decoded_str);
	if (cbor.is_string() && (cbor.get_string_len() == ref_str.length()) \
		 && (decoded_str == ref_str)) {
		return true;
	}

	return false;
}

bool test_array2()
{
	const uint8_t cbor_data[] = {0x83, 0x01, 0x02, 0x03};
	CBOR cbor = CBOR(cbor_data, 4);
	int tab[] = {1, 2, 3};

	for (size_t i=0 ; i < cbor.n_elements() ; ++i) {
		if ((int)cbor[i] != tab[i]) {
			return false;
		}
	}

	return true;
}

bool test_array3()
{
	const uint8_t cbor_data[] = {0x83, 0x01, 0xFA, 0x40, 0x4A, 0x3D, 0x71, 0x64, 0x74, 0x65, 0x73, 0x74};
	CBOR cbor = CBOR(cbor_data, 12);

	if ((int)cbor[0] == 1 && (float)cbor[1] == (float)3.16 && cbor[2].to_string() == "test") {
		return true;
	}

	return false;
}

bool test_array4()
{
	const uint8_t cbor_data[] = {0x83, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05};
	CBOR cbor = CBOR(cbor_data, 8);

	if ((int)cbor[0] == 1) {
		if((int)cbor[1][0] != 2 || (int)cbor[1][1] != 3) {
			return false;
		}

		if((int)cbor[2][0] != 4 || (int)cbor[2][1] != 5) {
			return false;
		}

		return true;
	}

	return false;
}

bool test_array5()
{
	const uint8_t cbor_data[] = {0x98, 0x19, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  \
								0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, \
								0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, \
								0x17, 0x18, 0x18, 0x18, 0x19};
	CBOR cbor = CBOR(cbor_data, 29);

	for (int i=0 ; i < 25 ; ++i) {
		if ((int)cbor[i] != (i+1)) {
			return false;
		}
	}

	return true;
}

bool test_pair()
{
	const uint8_t cbor_data[] = {0xa2, 0x01, 0x02, 0x03, 0x04};
	CBOR cbor = CBOR(cbor_data, 5); //Test that

	if ((int)cbor[1] == 2 && (int)cbor[3] == 4) {
		return true;
	}

	return false;
}

bool test_pair1()
{
	const uint8_t cbor_data[] = {0xa2, 0x61, 0x61, 0x01, 0x61, 0x62, 0x82, 0x02, 0x03};
	CBOR cbor = CBOR(cbor_data, 9);

	if ((int)cbor["a"] == 1) {
 		if((int)cbor["b"][0] == 2 && (int)cbor["b"][1] == 3) {
			return true;
		}
	}

	return false;
}

bool test_pair2()
{
	const uint8_t cbor_data[] = {0xa5, 0x61, 0x61, 0x61, 0x41, 0x61, 0x62, 0x61, \
								0x42, 0x61, 0x63, 0x61, 0x43, 0x61, 0x64, 0x61, \
								0x44, 0x61, 0x65, 0x61, 0x45};
	CBOR cbor = CBOR(cbor_data, 21);

	if (cbor["a"].to_string() == "A" && cbor["b"].to_string() == "B" \
		&& cbor["c"].to_string() == "C" && cbor["d"].to_string() == "D" \
		&& cbor["e"].to_string() == "E") {

		return true;
	}

	return false;
}

bool test_tag()
{
	const uint8_t cbor_data[6] = {0xC4, 0x82, 0x21, 0x19, 0x6A, 0xB3};

	CBOR cbor = CBOR(cbor_data, 6);
	CBOR tag_item = cbor.get_tag_item();

	if (!cbor.is_tag()) {
		return false;
	}

	if (cbor.get_tag_value() != 0x04) {
		return false;
	}

	if (tag_item.length() != 5) {
		return false;
	}

	for (int i=0 ; i < 5 ; ++i) {
		if (tag_item.to_CBOR()[i] != cbor_data[i+1]) {
			return false;
		}
	}

	return true;
}

bool test_bytestr()
{
	const uint8_t cbor_data[1] = {0x40};
	CBOR cbor = CBOR(cbor_data, 1);

	if (cbor.is_bytestring() && (cbor.get_bytestring_len() == 0)) {
		return true;
	}

	return false;
}

bool test_bytestr1()
{
	const uint8_t cbor_data[5] = {0x44, 0x01, 0x02, 0x03, 0x04};
	CBOR cbor = CBOR(cbor_data, 5);

	const uint8_t ref_bytestr[4] = {0x01, 0x02, 0x03, 0x04};
	uint8_t decoded_bytestr[4] = {0x00};

	cbor.get_bytestring(decoded_bytestr);
	if (cbor.is_bytestring() && (cbor.get_bytestring_len() == 4)) {
		for (int i=0 ; i<cbor.get_bytestring_len() ; ++i) {
			if (decoded_bytestr[i] != ref_bytestr[i]) {
				return false;
			}
		}
		return true;
	}

	return false;
}

void setup()
{
	//Basic CBOR types
	Serial.begin(115200);
	Serial.println("Begining test sequence!");

	Serial.print("True: ");
	if (test_bool_true()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("False: ");
	if (test_bool_false()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-1: ");
	if (test_negint1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-1000: ");
	if (test_negint2()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-1000000000: ");
	if (test_negint3()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-1000000000000000: ");
	if (test_negint4()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("23: ");
	if (test_int0()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("1000: ");
	if (test_int1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("1000000000: ");
	if (test_int2()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("2376030000: ");
	if (test_int3()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("1000000000000000: ");
	if (test_int4()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-0.0 (half) : ");
	if (test_neghalf()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-4.0 (half) : ");
	if (test_neghalf1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("1.0 (half) : ");
	if (test_half()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("0.0 (half) : ");
	if (test_half1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("65504.0 (half) : ");
	if (test_half2()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("1.5 (half) : ");
	if (test_half3()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("5.960464477539063e-8 (half) : ");
	if (test_half4()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("0.00006103515625 (half) : ");
	if (test_half5()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("Infinity (half) : ");
	if (test_half_inf()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-Infinity (half) : ");
	if (test_neghalf_inf()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("NaN (half) : ");
	if (test_half_nan()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-1.5 : ");
	if (test_negfloat()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("100000.0 : ");
	if (test_float()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("1.5 : ");
	if (test_float1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("3.4028234663852886e+38 : ");
	if (test_float2()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("Infinity : ");
	if (test_float_inf()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-Infinity : ");
	if (test_negfloat_inf()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("NaN : ");
	if (test_float_nan()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-1.5 (double) : ");
	if (test_negdouble()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-4.1 (double) : ");
	if (test_negdouble1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("1.5 (double) : ");
	if (test_double()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("1.1 (double) : ");
	if (test_double()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("Infinity (double) : ");
	if (test_double_inf()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("-Infinity (double) : ");
	if (test_negdouble_inf()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("NaN (double) : ");
	if (test_double_nan()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("aoeu1234 (char[]) : ");
	if (test_string_char1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("åöéûのかめ亀 (char []) : ");
	if (test_string_char2()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("Lorem ipsum... (char []) : ");
	if (test_string_char3()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("aoeu1234 (String) : ");
	if (test_string_string1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("åöéûのかめ亀 (String) : ");
	if (test_string_string2()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("[1, 2, 3] : ");
	if (test_array2()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("[1, 3.16, \"test\"] : ");
	if (test_array3()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("[1, [2, 3], [4, 5]] : ");
	if (test_array4()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("[1, ..., 25] : ");
	if (test_array5()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("{1: 2, 3: 4} : ");
	if (test_pair()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("{\"a\": 1, \"b\": [2, 3]} : ");
	if (test_pair1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("{\"a\": \"A\", \"b\": \"B\", \"c\": \"C\", \"d\": \"D\", \"e\": \"E\"} : ");
	if (test_pair2()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("Custom tag : ");
	if (test_tag()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("h'' : ");
	if (test_bytestr()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("h'01020304' : ");
	if (test_bytestr1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}
}

void loop()
{
}
