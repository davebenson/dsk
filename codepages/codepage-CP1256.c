#include "../dsk.h"
static const uint8_t CP1256_heap[] = {0xe2,0x82,0xac,0xd9,0xbe,0xe2,0x80,0x9a,0xc6,0x92,0xe2,0x80,0x9e,0xe2,0x80,0xa6,0xe2,0x80,0xa0,0xe2,0x80,0xa1,0xcb,0x86,0xe2,0x80,0xb0,0xd9,0xb9,0xe2,0x80,0xb9,0xc5,0x92,0xda,0x86,0xda,0x98,0xda,0x88,0xda,0xaf,0xe2,0x80,0x98,0xe2,0x80,0x99,0xe2,0x80,0x9c,0xe2,0x80,0x9d,0xe2,0x80,0xa2,0xe2,0x80,0x93,0xe2,0x80,0x94,0xda,0xa9,0xe2,0x84,0xa2,0xda,0x91,0xe2,0x80,0xba,0xc5,0x93,0xe2,0x80,0x8c,0xe2,0x80,0x8d,0xda,0xba,0xc2,0xa0,0xd8,0x8c,0xc2,0xa2,0xc2,0xa3,0xc2,0xa4,0xc2,0xa5,0xc2,0xa6,0xc2,0xa7,0xc2,0xa8,0xc2,0xa9,0xda,0xbe,0xc2,0xab,0xc2,0xac,0xc2,0xad,0xc2,0xae,0xc2,0xaf,0xc2,0xb0,0xc2,0xb1,0xc2,0xb2,0xc2,0xb3,0xc2,0xb4,0xc2,0xb5,0xc2,0xb6,0xc2,0xb7,0xc2,0xb8,0xc2,0xb9,0xd8,0x9b,0xc2,0xbb,0xc2,0xbc,0xc2,0xbd,0xc2,0xbe,0xd8,0x9f,0xdb,0x81,0xd8,0xa1,0xd8,0xa2,0xd8,0xa3,0xd8,0xa4,0xd8,0xa5,0xd8,0xa6,0xd8,0xa7,0xd8,0xa8,0xd8,0xa9,0xd8,0xaa,0xd8,0xab,0xd8,0xac,0xd8,0xad,0xd8,0xae,0xd8,0xaf,0xd8,0xb0,0xd8,0xb1,0xd8,0xb2,0xd8,0xb3,0xd8,0xb4,0xd8,0xb5,0xd8,0xb6,0xc3,0x97,0xd8,0xb7,0xd8,0xb8,0xd8,0xb9,0xd8,0xba,0xd9,0x80,0xd9,0x81,0xd9,0x82,0xd9,0x83,0xc3,0xa0,0xd9,0x84,0xc3,0xa2,0xd9,0x85,0xd9,0x86,0xd9,0x87,0xd9,0x88,0xc3,0xa7,0xc3,0xa8,0xc3,0xa9,0xc3,0xaa,0xc3,0xab,0xd9,0x89,0xd9,0x8a,0xc3,0xae,0xc3,0xaf,0xd9,0x8b,0xd9,0x8c,0xd9,0x8d,0xd9,0x8e,0xc3,0xb4,0xd9,0x8f,0xd9,0x90,0xc3,0xb7,0xd9,0x91,0xc3,0xb9,0xd9,0x92,0xc3,0xbb,0xc3,0xbc,0xe2,0x80,0x8e,0xe2,0x80,0x8f,0xdb,0x92,};
const DskCodepage dsk_codepage_CP1256 = {
  {0,3,5,8,10,13,16,19,22,24,27,29,32,34,36,38,40,42,45,48,51,54,57,60,63,65,68,70,73,75,78,81,83,85,87,89,91,93,95,97,99,101,103,105,107,109,111,113,115,117,119,121,123,125,127,129,131,133,135,137,139,141,143,145,147,149,151,153,155,157,159,161,163,165,167,169,171,173,175,177,179,181,183,185,187,189,191,193,195,197,199,201,203,205,207,209,211,213,215,217,219,221,223,225,227,229,231,233,235,237,239,241,243,245,247,249,251,253,255,257,259,261,263,265,267,269,272,275,277}, CP1256_heap};
