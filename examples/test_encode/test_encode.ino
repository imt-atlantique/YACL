#include "YACL.h"

bool buffer_equals(const uint8_t* buf1, size_t len_buf1, const uint8_t* buf2, size_t len_buf2, bool verbose=true)
{
	if (len_buf1 != len_buf2) {
		if (verbose) {
			Serial.print("Lengths do not match: ");
			Serial.print(len_buf1);
			Serial.print(" != ");
			Serial.println(len_buf2);
		}

		return false;
	}

	bool flag = true;

	for (size_t i=0 ; i < len_buf2 ; ++i) {
		if (buf1[i] != buf2[i]) {
			if (verbose) {
				flag = false;
				Serial.print((unsigned int)buf1[i], HEX);
				Serial.print(" != ");
				Serial.print((unsigned int)buf2[i], HEX);
				Serial.print(" at index ");
				Serial.println((unsigned int)i);
			}
			else {
				return false;
			}
		}
	}

	if (!flag) {
		Serial.println("Content of buf1:");
		for (size_t i=0 ; i < len_buf1 ; ++i) {
			Serial.print((unsigned int)buf1[i], HEX);
			Serial.print(' ');
		}

		Serial.println("\nContent of buf2:");
		for (size_t i=0 ; i < len_buf2 ; ++i) {
			Serial.print((unsigned int)buf2[i], HEX);
			Serial.print(' ');
		}
		Serial.print('\n');
	}

	return flag;
}

bool test_null()
{
	const uint8_t expected = 0xF6;
	size_t len_expected = 1;
	CBOR cbor = CBOR();

	if (buffer_equals(&expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_bool_true()
{
	const uint8_t expected = 0xF5;
	size_t len_expected = 1;
	CBOR cbor = CBOR(true);

	if (buffer_equals(&expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_bool_false()
{
	const uint8_t expected = 0xF4;
	size_t len_expected = 1;
	CBOR cbor = CBOR(false);

	if (buffer_equals(&expected, len_expected, cbor.to_CBOR(), cbor.length(), true)) {
		return true;
	}

	return false;
}

bool test_negint1()
{
	const uint8_t expected = 0x20;
	size_t len_expected = 1;
	CBOR cbor = CBOR(-1);

	if (buffer_equals(&expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_negint2()
{
	const uint8_t expected[3] = {0x39, 0x03, 0xe7};
	size_t len_expected = 3;
	CBOR cbor = CBOR(-1000);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_negint3()
{
	const uint8_t expected[5] = {0x3a, 0x3b, 0x9a, 0xc9, 0xff};
	size_t len_expected = 5;
	CBOR cbor = CBOR(-1000000000);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_negint4()
{
	const uint8_t expected[9] = {0x3b, 0x00, 0x03, 0x8d, 0x7e, 0xa4, 0xc6, 0x7f, 0xff};
	size_t len_expected = 9;
	CBOR cbor = CBOR(-1000000000000000);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_int0()
{
	const uint8_t expected[1] = {0x17};
	size_t len_expected = 1;
	CBOR cbor = CBOR(23);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_int1()
{
	const uint8_t expected[3] = {0x19, 0x03, 0xe8};
	size_t len_expected = 3;
	CBOR cbor = CBOR(1000);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_int2()
{
	const uint8_t expected[5] = {0x1a, 0x3b, 0x9a, 0xca, 0x00};
	size_t len_expected = 5;
	CBOR cbor = CBOR(1000000000);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_int3()
{
	const uint8_t expected[5] = {0x1a, 0x8d, 0x9f, 0x57, 0x30};
	size_t len_expected = 5;
	CBOR cbor = CBOR(2376030000);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_int4()
{
	const uint8_t expected[9] = {0x1b, 0x00, 0x03, 0x8d, 0x7e, 0xa4, 0xc6, 0x80, 0x00};
	size_t len_expected = 9;
	CBOR cbor = CBOR(1000000000000000);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_negfloat()
{
	const uint8_t expected[5] = {0xfa, 0xbf, 0xc0, 0x00, 0x00};
	size_t len_expected = 5;
	CBOR cbor = CBOR((float)-1.5);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_float()
{
	const uint8_t expected[5] = {0xfa, 0x3f, 0xc0, 0x00, 0x00};
	size_t len_expected = 5;
	CBOR cbor = CBOR((float)1.5);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_negdouble()
{
	const uint8_t expected[9] = {0xfb, 0xbf, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	size_t len_expected = 9;
	CBOR cbor = CBOR((double)-1.5);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_double()
{
	const uint8_t expected[9] = {0xfb, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	size_t len_expected = 9;
	CBOR cbor = CBOR((double)1.5);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_string1()
{
	const uint8_t expected[9] = {0x68, 0x61, 0x6f, 0x65, 0x75, 0x31, 0x32, 0x33, 0x34};
	size_t len_expected = 9;
	CBOR cbor = CBOR("aoeu1234");

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_string2()
{
	const uint8_t expected[21] = {0x74, 0xc3, 0xa5, 0xc3, 0xb6, 0xc3, 0xa9, 0xc3, 0xbb, 0xe3, 0x81, 0xae, 0xe3, 0x81, 0x8b, 0xe3, 0x82, 0x81, 0xe4, 0xba, 0x80};
	size_t len_expected = 21;
	CBOR cbor = CBOR("åöéûのかめ亀");

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_string3()
{
	const uint8_t expected = 0x60;
	size_t len_expected = 1;
	CBOR cbor = CBOR("");

	if (buffer_equals(&expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_array1()
{
	const uint8_t expected = 0x80;
	size_t len_expected = 1;
	uint8_t cbor_buffer[10];
	CBORArray cbor = CBORArray(cbor_buffer, 10, false);

	if (buffer_equals(&expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_array2()
{
	const uint8_t expected[4] = {0x83, 0x01, 0x02, 0x03};
	size_t len_expected = 4;
	uint8_t data[3] = {1, 2, 3};
	uint8_t cbor_buffer[13];
	CBORArray cbor = CBORArray(cbor_buffer, 13, false);
	cbor.append(data, 3);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_array3()
{
	const uint8_t expected[] = {0x83, 0x01, 0xFA, 0x40, 0x4A, 0x3D, 0x71, 0x64, 0x74, 0x65, 0x73, 0x74};
	size_t len_expected = 12;
	uint8_t cbor_buffer[20];
	CBORArray cbor = CBORArray(cbor_buffer, 20, false);
	cbor.append(CBOR(1));
	cbor.append((float)3.16);
	cbor.append("test");

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_pair()
{
	const uint8_t expected[21] = {0xa3, 0x02, 0x02, 0x67, 0x5b, 0x31, 0x2c, 0x32, \
								  0x2c, 0x33, 0x5d, 0x83, 0x01, 0x02, 0x03, 0x64, \
								  0x4e, 0x75, 0x6c, 0x6c, 0xf6};
	size_t len_expected = 21;
	uint8_t cbor_buffer[30];
	uint8_t arr_buffer[12];
	CBORPair cbor = CBORPair(cbor_buffer, 30, false);
	CBORArray arr = CBORArray(arr_buffer, 12, false);

	cbor.append(CBOR(2), 2);
	arr.append(1);
	arr.append(2);
	arr.append(3);
	cbor.append("[1,2,3]", arr);
	cbor.append("Null", CBOR());

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_tag()
{
	const uint8_t expected[6] = {0xC4, 0x82, 0x21, 0x19, 0x6A, 0xB3};
	size_t len_expected = 6;
	uint8_t cbor_buffer[6];
	uint8_t arr_buffer[14]; //14

	CBORArray arr = CBORArray(arr_buffer, 14, false); //14
	arr.append(-2);
	arr.append(27315);

	CBOR cbor = CBOR(cbor_buffer, 6);
	cbor.encode(0x04, arr);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_bytestr()
{
	const uint8_t expected[1] = {0x40};
	size_t len_expected = 1;

	CBOR cbor = CBOR();
	cbor.encode((const uint8_t*)NULL, 0);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_bytestr1()
{
	const uint8_t expected[5] = {0x44, 0x01, 0x02, 0x03, 0x04};
	size_t len_expected = 5;
	
	const uint8_t data[4] = {0x01, 0x02, 0x03, 0x04};

	CBOR cbor = CBOR();
	cbor.encode(data, 4);

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

void setup()
{
	//Basic CBOR types
	Serial.begin(115200);
	Serial.println("Begining test sequence!");

	Serial.print("Null: ");
	if (test_null()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

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

	Serial.print("-1.5 : ");
	if (test_negfloat()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("1.5 : ");
	if (test_float()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	if (sizeof(float) != sizeof(double)) {
		Serial.print("-1.5 (double) : ");
		if (test_negdouble()) {
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
	}

	Serial.print("\"aoeu1234\" : ");
	if (test_string1()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("\"åöéûのかめ亀\" : ");
	if (test_string2()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("\"\" : ");
	if (test_string3()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("[] : ");
	if (test_array1()) {
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

	Serial.print("{2: 2, \"1,2,3\": [1,2,3], \"Null\": NULL} : ");
	if (test_pair()) {
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

	Serial.print("Custom tag encoding : ");
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
