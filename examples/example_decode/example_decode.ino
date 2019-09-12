#include "YACL.h"

void setup()
{
	Serial.begin(115200);
	Serial.println("Let say that we received the following CBOR data:");
	uint8_t cbor_data[41] = {0xA3, 0x63, 0x6C, 0x61, 0x74, 0xFA, 0x42, 0x40,
							0x7A, 0xFB, 0x63, 0x6C, 0x6F, 0x6E, 0xFA, 0xBF,
							0xD0, 0x76, 0xF3, 0x6C, 0x74, 0x65, 0x6D, 0x70,
							0x65, 0x72, 0x61, 0x74, 0x75, 0x72, 0x65, 0x73,
							0x84, 0x18, 0x19, 0x18, 0x18, 0x18, 0x18, 0x18,
							0x1A};
	for (size_t i=0 ; i < 41 ; ++i) {
		Serial.print(cbor_data[i], HEX);
	}
	Serial.println("\n Let say taht we know its expected structure:");
	Serial.println("{");
	Serial.println("\t \"lat\": float,");
	Serial.println("\t \"lon\": float,");
	Serial.println("\t \"temperatures\": table of ints");
	Serial.println("}");

	Serial.println("Trying to decode received data:");

	// Let's first check if the received CBOR data is a CBOR dictionary of
	// key/values.
	// Sanity check are very important, as most of YACL operations on CBOR
	// data are not safe (you can, for instance try to decode a string as an
	// int, but the result of this operation is unknown, and may lead your
	// program to crash...).
	if (!CBOR::is_pair(cbor_data)) {
		Serial.println("Data does not encode a CBOR Pair. Aborting.");

		return;
	}
	// Import dictionary into a CBOR object
	CBOR data = CBOR(cbor_data, 41, true);

	Serial.println("{");

	// Check that lat is a float and print it
	if (!data["lat"].is_float16() && !data["lat"].is_float32()) {
		Serial.println("\"lat\" is not a float. Aborting.");

		return;
	}
	Serial.print("\t \"lat\": ");
	Serial.print((float)data["lat"], 4); //Show four digits after point
	Serial.print(",\n");

	// Check that lon is a float and print it
	if (!data["lon"].is_float16() && !data["lon"].is_float32()) {
		Serial.println("\"lon\" is not a float. Aborting.");

		return;
	}
	Serial.print("\t \"lon\": ");
	Serial.print((float)data["lon"], 4); //Show four figits after point
	Serial.print(",\n");

	// Check that temperatures is an array and print it
	if (!data["temperatures"].is_array()) {
		Serial.println("\"temperatures\" is not an array. Aborting.");

		return;
	}

	CBOR array = data["temperatures"];
	Serial.print("\t \"temperatures\": [");
	for (size_t i=0 ; i < array.n_elements() ; ++i) {
		//Check that the value fit in an int (32 bits integer)
		if(!array[i].is_int32()) {
			Serial.print("Does not fit in an int, ");
		}
		else {
			Serial.print((int)array[i]);
			Serial.print(", ");
		}
	}
	Serial.println("]");

	Serial.println("}");

	Serial.println("You can check that this is coherent with the results of http://cbor.me");
}

void loop()
{
}
