# YACL: Yet Another CBOR Library

YACL is a CBOR library for Arduino, focused towards ease of use.

## Quick start

Import YACL:
```c++
#include "YACL.h"
```
### Simple CBOR objects
Creating simple CBOR objects (int, floats, string):
```c++
CBOR cbor_null = CBOR();
CBOR cbor_bool = CBOR(true);
CBOR cbor_int = CBOR(389);
CBOR cbor_float = CBOR(3.14);
```

Printing/using/converting from a simple CBOR object:
```c++
//Automatic casting
bool decoded_bool = (bool)cbor_bool;
int decoded_int = (int)cbor_int;
float decoded_float = (float)cbor_float;

//Manual casting
Serial.println((bool)cbor_bool);
Serial.println((int)cbor_int);
Serial.println((float)cbor_float);
```

### Strings
CBOR strings are created as simple CBOR objects:
```c++
CBOR cbor_string = CBOR("Hello, world!");
```

Strings can be converted to `char` arrays (C string) or Arduino `String`:
```c++
//Arduino String
String decoded_string;
cbor_string.get_string(decoded_string);
Serial.println(decoded_string);

//C String
char decoded_cstring[50]; //Reserve 50 characters for the string
cbor_string.get_string(decoded_cstring);
Serial.println(decoded_cstring);
```
String length is given by `cbor_string.get_string_len()`.

### Arrays
Arrays are handled by the class `CBORArray`:
- Arrays can be manually created using with `append(value)`, from a C-style array, or from an existing CBOR object.
- You can only add elements to arrays. You cannot delete or modify an existing element in the array.
- Element access is done in the same way as C-style arrays, using square brackets `[index]`.

Using `append(value)`, any CBOR-convertible element or CBOR object can be appened to the array :
```c++
CBORArray cbor_arr = CBORArray();

//CBOR-convertible elements
cbor_arr.append(1);
cbor_arr.append(2);
cbor_arr.append(3.14);
cbor_arr.append("Hello, world");

//CBOR objects
CBOR cbor_obj = CBOR("YACL!");
CBORArray cbor_arr2 = CBORArray(); //Array with 0 elements: []
CBORPair cbor_dict = CBORPair(); //Dictionary with 0 elements: {}

cbor_arr.append(cbor_obj);
cbor_arr.append(cbor_arr2); //Nested array
cbor_arr.append(cbor_pair);

//At this point cbor_arr contains: [1, 2, 3.14, "Hello, world", "YACL!", [], {}]
```
Appending a C-style array also requires `append()`:
```c++
int int_arr[5] = {0, 1, 2, 3, 4};

cbor_arr.append(int_att, 5); //Must specify the number of elements
```

Accessing an element of a CBOR array is done with square brackets:
```c++
//Explore the whole array
for (size_t i=0 ; i < cbor_arr.n_elements() ; ++i) {
	CBOR cbor_ele = cbor_arr[i];
}
```
Note that:
 - Multi-level indexing (like `cbor_arr[x][y][z]`) is possible.
 - `CBORArray` is only required for encoding. Decoding can be performed using regular CBOR objects.


### Dictionaries of key/value pairs
Dictionaries are handled by the class `CBORPair`:
- Dictionaries must be manually created using with `append(key, value)`, or from an existing CBOR object.
- You can only add elements to dictionaries. You cannot delete or modify an existing element in the dictionary.
- Element access is done using square brackets `[key]`.

Using `append(key, value)`, any CBOR-convertible element or CBOR object key and value can be appened to the dictionnary.
```c++
CBORPair cbor_dict = CBORPair();

//CBOR-convertible elements
cbor_dict.append(1, 2);
cbor_dict.append(3.14, 3);
cbor_dict.append("4", "Hello, world");

//CBOR objects
CBOR cbor_obj = CBOR("YACL!");
CBORPair cbor_dict2 = CBORPair(); //Dictionary with 0 elements: {}
CBORArray cbor_arr = CBORArray(); //array with 0 elements: []

cbor_dict.append(cbor_obj, cbor_dict2); //Nested dictionary
cbor_dict.append(-5, cbor_dict2); //Nested dictionary
//At this point cbor_dict contains: {1: 2, 3.14: 3, "4": "Hello, world", "YACL!": {}, -5: []}
```
Accessing an element of a CBOR dictionary is done with square brackets:
```c++
CBOR cbor_ele1 = cbor_dict["YACL!"];
CBOR cbor_ele2 = cbor_dict[-5];
```
A CBOR dictionary can be explored using `key_at(n)` and `at(n)`, to retrieve the n-th element key and value:
```c++
//Explore the whole array
for (size_t i=0 ; i < cbor_dict.n_elements() ; ++i) {
	CBOR cbor_key = cbor_dict.key_at(i);
	CBOR cbor_val = cbor_dict.at(i); //or cbor_val[cbor_dict.key_at(i)] (slower in this context)
}
```

Note that:
 - Multi-level indexing (like `cbor_dict["YACL!"][0]`) is possible.
 - `CBORPair` is only required for encoding. Decoding can be performed using regular CBOR objects.

### Importing and exporting

Import an encoded CBOR message:

```c++
/*CBOR encoding of:
 *{
 *	"lat": 48.12009811401367,
 *	"lon": -1.6286300420761108,
 *	"temperatures": [25, 24, 24, 26]
 *}
 */
uint8_t buffer[41] = {0xA3, 0x63, 0x6C, 0x61, 0x74, 0xFA, 0x42, 0x40,
			0x7A, 0xFB, 0x63, 0x6C, 0x6F, 0x6E, 0xFA, 0xBF,
			0xD0, 0x76, 0xF3, 0x6C, 0x74, 0x65, 0x6D, 0x70,
			0x65, 0x72, 0x61, 0x74, 0x75, 0x72, 0x65, 0x73,
			0x84, 0x18, 0x19, 0x18, 0x18, 0x18, 0x18, 0x18,
			0x1A};
CBOR cbor_data = CBOR(buffer, 41);
```
All decoding operations (including array and dictionnary element access) can be done using only `CBOR` objects. That is: no need for `CBORArray` nor `CBORPair` for decoding.
Primitive types (int, float, etc.) can be converted from their CBOR representations using type-casting.
```c++
//Print lat and lon
Serial.println((float)cbor_data["lat"]);
Serial.println((float)cbor_data["lon"]);

//Print temperatures
for (size_t i=0 ; i < cbor_data["temperatures"].n_elements() ; ++i) {
	Serial.println((int)cbor_data["temperatures"][i]);
}
```

Export a CBOR/CBORPair/CBORArray object to a CBOR-encoded message:
```c++
uint8_t *buffer = cbor_msg.to_CBOR();
size_t buffer_len = cbor_msg.length();
```

## Advanced usage

see [AdvancedUsage.md](AdvancedUsage.md)
