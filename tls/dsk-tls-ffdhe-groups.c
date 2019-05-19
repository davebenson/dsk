/*
 * Machine-generated by dsk-make-ffdhe-montgomery-info.
 *
 * DO NOT HAND EDIT!
 */


#include "../dsk.h"


static const uint32_t tls_ffdhe2048_value[] = {
  0xffffffff,
  0xffffffff,
  0x61285c97,
  0x886b4238,
  0xc1b2effa,
  0xc6f34a26,
  0x7d1683b2,
  0xc58ef183,
  0x2ec22005,
  0x3bb5fcbc,
  0x4c6fad73,
  0xc3fe3b1b,
  0xeef28183,
  0x8e4f1232,
  0xe98583ff,
  0x9172fe9c,
  0x28342f61,
  0xc03404cd,
  0xcdf7e2ec,
  0x9e02fce1,
  0xee0a6d70,
  0x0b07a7c8,
  0x6372bb19,
  0xae56ede7,
  0xde394df4,
  0x1d4f42a3,
  0x60d7f468,
  0xb96adab7,
  0xb2c8e3fb,
  0xd108a94b,
  0xb324fb61,
  0xbc0ab182,
  0x483a797a,
  0x30acca4f,
  0x36ade735,
  0x1df158a1,
  0xf3efe872,
  0xe2a689da,
  0xe0e68b77,
  0x984f0c70,
  0x7f57c935,
  0xb557135e,
  0x3ded1af3,
  0x85636555,
  0x5f066ed0,
  0x2433f51f,
  0xd5fd6561,
  0xd3df1ed5,
  0xaec4617a,
  0xf681b202,
  0x630c75d8,
  0x7d2fe363,
  0x249b3ef9,
  0xcc939dce,
  0x146433fb,
  0xa9e13641,
  0xce2d3695,
  0xd8b9c583,
  0x273d3cf1,
  0xafdc5620,
  0xa2bb4a9a,
  0xadf85458,
  0xffffffff,
  0xffffffff,
};

static const uint32_t tls_ffdhe2048_barrett_mu = {
  0xd38a4fa1,
  0x187be36b,
  0x253e1750,
  0x3c735a45,
  0x45b5d710,
  0x9a7e29cb,
  0x74607977,
  0xac085057,
  0x840e525e,
  0x325cdf13,
  0xdd7fb905,
  0xce37341a,
  0x09604461,
  0x69332adc,
  0x29a4bc69,
  0x473b96fc,
  0x658d9daf,
  0x988e0e6e,
  0x417a439f,
  0x6f89f447,
  0xdfce5aae,
  0xc4012d2f,
  0x288d4142,
  0xd0b8a259,
  0xcb532208,
  0x4391e833,
  0xb91541f8,
  0xdcd0a9e0,
  0x77291a82,
  0xf11e919a,
  0xab15e3ca,
  0xf408a9bb,
  0xaabe35a7,
  0x9cbd41d6,
  0xea0fdb44,
  0x5366088f,
  0x193d06cf,
  0xf74429c1,
  0x1c41db7f,
  0x124826fb,
  0x433cfb77,
  0x847da8b2,
  0x5c3895bb,
  0x9fc4f20f,
  0x81e47b50,
  0xf65dc3a3,
  0x4ebd5ae0,
  0x40e9414d,
  0x82120a6c,
  0x6de3b48f,
  0x96ce7702,
  0xc63311c8,
  0xc999b95c,
  0xb6d719bc,
  0x6248c624,
  0xc26987a2,
  0xe4692c67,
  0x9e889b5f,
  0xe6c2e509,
  0x6a6c9411,
  0x5d44b565,
  0x5207aba7,
  0x00000000,
  0x00000000,
  0x00000001,
};

DskTlsFFDHE dsk_tls_ffdhe2048 =
{
  "ffdhe2048",
  {
    64,
    tls_ffdhe2048_value,
    0x1,    /* Nprime */
    tls_ffdhe2048_barrett_mu
  },
  225
};
static const uint32_t tls_ffdhe3072_value[] = {
  0xffffffff,
  0xffffffff,
  0x66c62e37,
  0x25e41d2b,
  0x3fd59d7c,
  0x3c1b20ee,
  0xfa53ddef,
  0x0abcd06b,
  0xd5c4484e,
  0x1dbf9a42,
  0x9b0deada,
  0xabc52197,
  0x22363a0d,
  0xe86d2bc5,
  0x9c9df69e,
  0x5cae82ab,
  0x71f54bff,
  0x64f2e21e,
  0xe2d74dd3,
  0xf4fd4452,
  0xbc437944,
  0xb4130c93,
  0x85139270,
  0xaefe1309,
  0xc186d91c,
  0x598cb0fa,
  0x91f7f7ee,
  0x7ad91d26,
  0xd6e6c907,
  0x61b46fc9,
  0xf99c0238,
  0xbc34f4de,
  0x6519035b,
  0xde355b3b,
  0x611fcfdc,
  0x886b4238,
  0xc1b2effa,
  0xc6f34a26,
  0x7d1683b2,
  0xc58ef183,
  0x2ec22005,
  0x3bb5fcbc,
  0x4c6fad73,
  0xc3fe3b1b,
  0xeef28183,
  0x8e4f1232,
  0xe98583ff,
  0x9172fe9c,
  0x28342f61,
  0xc03404cd,
  0xcdf7e2ec,
  0x9e02fce1,
  0xee0a6d70,
  0x0b07a7c8,
  0x6372bb19,
  0xae56ede7,
  0xde394df4,
  0x1d4f42a3,
  0x60d7f468,
  0xb96adab7,
  0xb2c8e3fb,
  0xd108a94b,
  0xb324fb61,
  0xbc0ab182,
  0x483a797a,
  0x30acca4f,
  0x36ade735,
  0x1df158a1,
  0xf3efe872,
  0xe2a689da,
  0xe0e68b77,
  0x984f0c70,
  0x7f57c935,
  0xb557135e,
  0x3ded1af3,
  0x85636555,
  0x5f066ed0,
  0x2433f51f,
  0xd5fd6561,
  0xd3df1ed5,
  0xaec4617a,
  0xf681b202,
  0x630c75d8,
  0x7d2fe363,
  0x249b3ef9,
  0xcc939dce,
  0x146433fb,
  0xa9e13641,
  0xce2d3695,
  0xd8b9c583,
  0x273d3cf1,
  0xafdc5620,
  0xa2bb4a9a,
  0xadf85458,
  0xffffffff,
  0xffffffff,
};

static const uint32_t tls_ffdhe3072_barrett_mu = {
  0x14ba1560,
  0xfa1861ec,
  0xd88833dc,
  0xca384649,
  0xc40fb367,
  0x2ced1268,
  0x95566f37,
  0x44ac076f,
  0x78cae137,
  0x55837415,
  0x4746dd5a,
  0x0be8a0b0,
  0xda7a8e31,
  0xf7a0d007,
  0xc73e283f,
  0x578714ab,
  0xe02761c1,
  0x7e05e2f8,
  0x22243c7c,
  0x1bfc7132,
  0x1f16a2ae,
  0xa2b3a892,
  0xad720d89,
  0x1cc2f9e4,
  0xbc718f51,
  0xd9478704,
  0x33d6708a,
  0xb3f87d9c,
  0xfd19d0d6,
  0x5b9c0d07,
  0x6124ea99,
  0x84085f7f,
  0x6e76c6ef,
  0x3a468830,
  0x2546a40b,
  0x3c735a45,
  0x45b5d710,
  0x9a7e29cb,
  0x74607977,
  0xac085057,
  0x840e525e,
  0x325cdf13,
  0xdd7fb905,
  0xce37341a,
  0x09604461,
  0x69332adc,
  0x29a4bc69,
  0x473b96fc,
  0x658d9daf,
  0x988e0e6e,
  0x417a439f,
  0x6f89f447,
  0xdfce5aae,
  0xc4012d2f,
  0x288d4142,
  0xd0b8a259,
  0xcb532208,
  0x4391e833,
  0xb91541f8,
  0xdcd0a9e0,
  0x77291a82,
  0xf11e919a,
  0xab15e3ca,
  0xf408a9bb,
  0xaabe35a7,
  0x9cbd41d6,
  0xea0fdb44,
  0x5366088f,
  0x193d06cf,
  0xf74429c1,
  0x1c41db7f,
  0x124826fb,
  0x433cfb77,
  0x847da8b2,
  0x5c3895bb,
  0x9fc4f20f,
  0x81e47b50,
  0xf65dc3a3,
  0x4ebd5ae0,
  0x40e9414d,
  0x82120a6c,
  0x6de3b48f,
  0x96ce7702,
  0xc63311c8,
  0xc999b95c,
  0xb6d719bc,
  0x6248c624,
  0xc26987a2,
  0xe4692c67,
  0x9e889b5f,
  0xe6c2e509,
  0x6a6c9411,
  0x5d44b565,
  0x5207aba7,
  0x00000000,
  0x00000000,
  0x00000001,
};

DskTlsFFDHE dsk_tls_ffdhe3072 =
{
  "ffdhe3072",
  {
    96,
    tls_ffdhe3072_value,
    0x1,    /* Nprime */
    tls_ffdhe3072_barrett_mu
  },
  275
};
static const uint32_t tls_ffdhe4096_value[] = {
  0xffffffff,
  0xffffffff,
  0x5e655f6a,
  0xc68a007e,
  0xf44182e1,
  0x4db5a851,
  0x7f88a46b,
  0x8ec9b55a,
  0xcec97dcf,
  0x0a8291cd,
  0xf98d0acc,
  0x2a4ecea9,
  0x7140003c,
  0x1a1db93d,
  0x33cb8b7a,
  0x092999a3,
  0x71ad0038,
  0x6dc778f9,
  0x918130c4,
  0xa907600a,
  0x2d9e6832,
  0xed6a1e01,
  0xefb4318a,
  0x7135c886,
  0x7e31cc7a,
  0x87f55ba5,
  0x55034004,
  0x7763cf1d,
  0xd69f6d18,
  0xac7d5f42,
  0xe58857b6,
  0x7930e9e4,
  0x164df4fb,
  0x6e6f52c3,
  0x669e1ef1,
  0x25e41d2b,
  0x3fd59d7c,
  0x3c1b20ee,
  0xfa53ddef,
  0x0abcd06b,
  0xd5c4484e,
  0x1dbf9a42,
  0x9b0deada,
  0xabc52197,
  0x22363a0d,
  0xe86d2bc5,
  0x9c9df69e,
  0x5cae82ab,
  0x71f54bff,
  0x64f2e21e,
  0xe2d74dd3,
  0xf4fd4452,
  0xbc437944,
  0xb4130c93,
  0x85139270,
  0xaefe1309,
  0xc186d91c,
  0x598cb0fa,
  0x91f7f7ee,
  0x7ad91d26,
  0xd6e6c907,
  0x61b46fc9,
  0xf99c0238,
  0xbc34f4de,
  0x6519035b,
  0xde355b3b,
  0x611fcfdc,
  0x886b4238,
  0xc1b2effa,
  0xc6f34a26,
  0x7d1683b2,
  0xc58ef183,
  0x2ec22005,
  0x3bb5fcbc,
  0x4c6fad73,
  0xc3fe3b1b,
  0xeef28183,
  0x8e4f1232,
  0xe98583ff,
  0x9172fe9c,
  0x28342f61,
  0xc03404cd,
  0xcdf7e2ec,
  0x9e02fce1,
  0xee0a6d70,
  0x0b07a7c8,
  0x6372bb19,
  0xae56ede7,
  0xde394df4,
  0x1d4f42a3,
  0x60d7f468,
  0xb96adab7,
  0xb2c8e3fb,
  0xd108a94b,
  0xb324fb61,
  0xbc0ab182,
  0x483a797a,
  0x30acca4f,
  0x36ade735,
  0x1df158a1,
  0xf3efe872,
  0xe2a689da,
  0xe0e68b77,
  0x984f0c70,
  0x7f57c935,
  0xb557135e,
  0x3ded1af3,
  0x85636555,
  0x5f066ed0,
  0x2433f51f,
  0xd5fd6561,
  0xd3df1ed5,
  0xaec4617a,
  0xf681b202,
  0x630c75d8,
  0x7d2fe363,
  0x249b3ef9,
  0xcc939dce,
  0x146433fb,
  0xa9e13641,
  0xce2d3695,
  0xd8b9c583,
  0x273d3cf1,
  0xafdc5620,
  0xa2bb4a9a,
  0xadf85458,
  0xffffffff,
  0xffffffff,
};

static const uint32_t tls_ffdhe4096_barrett_mu = {
  0xcfb2cc2d,
  0xa7c622b7,
  0xc4f61acf,
  0xdc36e515,
  0xf9084fd9,
  0xe4e65ded,
  0x75a7bd06,
  0x1ee59fc5,
  0x175279fd,
  0x0b28e484,
  0xed6faf55,
  0x637e65c2,
  0xac88a2c1,
  0xca98eb3a,
  0xd1f5d2e1,
  0x600b387a,
  0x77cbf1e0,
  0xcb195f04,
  0xc5d7fa23,
  0xb787823a,
  0xfd090342,
  0xb3881c77,
  0xfd2172d2,
  0x46e1cdf6,
  0x5fd5a8a0,
  0x20e6c160,
  0x1443de1f,
  0x1925e3c5,
  0xd41e2ce7,
  0x98ff1866,
  0x830a2fc4,
  0xec11278e,
  0xfe85cc93,
  0x8ba90f28,
  0xd8b04323,
  0xca384649,
  0xc40fb367,
  0x2ced1268,
  0x95566f37,
  0x44ac076f,
  0x78cae137,
  0x55837415,
  0x4746dd5a,
  0x0be8a0b0,
  0xda7a8e31,
  0xf7a0d007,
  0xc73e283f,
  0x578714ab,
  0xe02761c1,
  0x7e05e2f8,
  0x22243c7c,
  0x1bfc7132,
  0x1f16a2ae,
  0xa2b3a892,
  0xad720d89,
  0x1cc2f9e4,
  0xbc718f51,
  0xd9478704,
  0x33d6708a,
  0xb3f87d9c,
  0xfd19d0d6,
  0x5b9c0d07,
  0x6124ea99,
  0x84085f7f,
  0x6e76c6ef,
  0x3a468830,
  0x2546a40b,
  0x3c735a45,
  0x45b5d710,
  0x9a7e29cb,
  0x74607977,
  0xac085057,
  0x840e525e,
  0x325cdf13,
  0xdd7fb905,
  0xce37341a,
  0x09604461,
  0x69332adc,
  0x29a4bc69,
  0x473b96fc,
  0x658d9daf,
  0x988e0e6e,
  0x417a439f,
  0x6f89f447,
  0xdfce5aae,
  0xc4012d2f,
  0x288d4142,
  0xd0b8a259,
  0xcb532208,
  0x4391e833,
  0xb91541f8,
  0xdcd0a9e0,
  0x77291a82,
  0xf11e919a,
  0xab15e3ca,
  0xf408a9bb,
  0xaabe35a7,
  0x9cbd41d6,
  0xea0fdb44,
  0x5366088f,
  0x193d06cf,
  0xf74429c1,
  0x1c41db7f,
  0x124826fb,
  0x433cfb77,
  0x847da8b2,
  0x5c3895bb,
  0x9fc4f20f,
  0x81e47b50,
  0xf65dc3a3,
  0x4ebd5ae0,
  0x40e9414d,
  0x82120a6c,
  0x6de3b48f,
  0x96ce7702,
  0xc63311c8,
  0xc999b95c,
  0xb6d719bc,
  0x6248c624,
  0xc26987a2,
  0xe4692c67,
  0x9e889b5f,
  0xe6c2e509,
  0x6a6c9411,
  0x5d44b565,
  0x5207aba7,
  0x00000000,
  0x00000000,
  0x00000001,
};

DskTlsFFDHE dsk_tls_ffdhe4096 =
{
  "ffdhe4096",
  {
    128,
    tls_ffdhe4096_value,
    0x1,    /* Nprime */
    tls_ffdhe4096_barrett_mu
  },
  325
};
static const uint32_t tls_ffdhe6144_value[] = {
  0xffffffff,
  0xffffffff,
  0xd0e40e65,
  0xa40e329c,
  0x7938dad4,
  0xa41d570d,
  0xd43161c1,
  0x62a69526,
  0x9adb1e69,
  0x3fdd4a8e,
  0xdc6b80d6,
  0x5b3b71f9,
  0xc6272b04,
  0xec9d1810,
  0xcacef403,
  0x8ccf2dd5,
  0xc95b9117,
  0xe49f5235,
  0xb854338a,
  0x505dc82d,
  0x1562a846,
  0x62292c31,
  0x6ae77f5e,
  0xd72b0374,
  0x462d538c,
  0xf9c9091b,
  0x47a67cbe,
  0x0ae8db58,
  0x22611682,
  0xb3a739c1,
  0x2a281bf6,
  0xeeaac023,
  0x77caf992,
  0x94c6651e,
  0x94b2bbc1,
  0x763e4e4b,
  0x0077d9b4,
  0x587e38da,
  0x183023c3,
  0x7fb29f8c,
  0xf9e3a26e,
  0x0abec1ff,
  0x350511e3,
  0xa00ef092,
  0xdb6340d8,
  0xb855322e,
  0xa9a96910,
  0xa52471f7,
  0x4cfdb477,
  0x388147fb,
  0x4e46041f,
  0x9b1f5c3e,
  0xfccfec71,
  0xcdad0657,
  0x4c701c3a,
  0xb38e8c33,
  0xb1c0fd4c,
  0x917bdd64,
  0x9b7624c8,
  0x3bb45432,
  0xcaf53ea6,
  0x23ba4442,
  0x38532a3a,
  0x4e677d2c,
  0x45036c7a,
  0x0bfd64b6,
  0x5e0dd902,
  0xc68a007e,
  0xf44182e1,
  0x4db5a851,
  0x7f88a46b,
  0x8ec9b55a,
  0xcec97dcf,
  0x0a8291cd,
  0xf98d0acc,
  0x2a4ecea9,
  0x7140003c,
  0x1a1db93d,
  0x33cb8b7a,
  0x092999a3,
  0x71ad0038,
  0x6dc778f9,
  0x918130c4,
  0xa907600a,
  0x2d9e6832,
  0xed6a1e01,
  0xefb4318a,
  0x7135c886,
  0x7e31cc7a,
  0x87f55ba5,
  0x55034004,
  0x7763cf1d,
  0xd69f6d18,
  0xac7d5f42,
  0xe58857b6,
  0x7930e9e4,
  0x164df4fb,
  0x6e6f52c3,
  0x669e1ef1,
  0x25e41d2b,
  0x3fd59d7c,
  0x3c1b20ee,
  0xfa53ddef,
  0x0abcd06b,
  0xd5c4484e,
  0x1dbf9a42,
  0x9b0deada,
  0xabc52197,
  0x22363a0d,
  0xe86d2bc5,
  0x9c9df69e,
  0x5cae82ab,
  0x71f54bff,
  0x64f2e21e,
  0xe2d74dd3,
  0xf4fd4452,
  0xbc437944,
  0xb4130c93,
  0x85139270,
  0xaefe1309,
  0xc186d91c,
  0x598cb0fa,
  0x91f7f7ee,
  0x7ad91d26,
  0xd6e6c907,
  0x61b46fc9,
  0xf99c0238,
  0xbc34f4de,
  0x6519035b,
  0xde355b3b,
  0x611fcfdc,
  0x886b4238,
  0xc1b2effa,
  0xc6f34a26,
  0x7d1683b2,
  0xc58ef183,
  0x2ec22005,
  0x3bb5fcbc,
  0x4c6fad73,
  0xc3fe3b1b,
  0xeef28183,
  0x8e4f1232,
  0xe98583ff,
  0x9172fe9c,
  0x28342f61,
  0xc03404cd,
  0xcdf7e2ec,
  0x9e02fce1,
  0xee0a6d70,
  0x0b07a7c8,
  0x6372bb19,
  0xae56ede7,
  0xde394df4,
  0x1d4f42a3,
  0x60d7f468,
  0xb96adab7,
  0xb2c8e3fb,
  0xd108a94b,
  0xb324fb61,
  0xbc0ab182,
  0x483a797a,
  0x30acca4f,
  0x36ade735,
  0x1df158a1,
  0xf3efe872,
  0xe2a689da,
  0xe0e68b77,
  0x984f0c70,
  0x7f57c935,
  0xb557135e,
  0x3ded1af3,
  0x85636555,
  0x5f066ed0,
  0x2433f51f,
  0xd5fd6561,
  0xd3df1ed5,
  0xaec4617a,
  0xf681b202,
  0x630c75d8,
  0x7d2fe363,
  0x249b3ef9,
  0xcc939dce,
  0x146433fb,
  0xa9e13641,
  0xce2d3695,
  0xd8b9c583,
  0x273d3cf1,
  0xafdc5620,
  0xa2bb4a9a,
  0xadf85458,
  0xffffffff,
  0xffffffff,
};

static const uint32_t tls_ffdhe6144_barrett_mu = {
  0x4a5c0ef7,
  0x3fa9b7ff,
  0x489059ac,
  0x67d8683b,
  0xdb2d6fa3,
  0x750727c6,
  0xd3c28a84,
  0xd0b7842d,
  0xab1fd8f9,
  0x8a23ba4b,
  0x670c33ce,
  0x10099139,
  0xed017a12,
  0xa665d518,
  0x196274b8,
  0xf6928678,
  0x7c31bedc,
  0xd8542eac,
  0x92258c85,
  0xa6e4710c,
  0x37fbc7b1,
  0xbdf364d5,
  0x4ce91422,
  0x3650415a,
  0xdd6bc832,
  0x94917464,
  0x8f80c9d9,
  0x3bf5f056,
  0xc4a438c7,
  0x91896b44,
  0x55b2393a,
  0xe1065d5a,
  0xd3dcbbe0,
  0x424aa85c,
  0x9d5a7570,
  0x4e6c8932,
  0x8b7d5c9b,
  0xe9214b85,
  0xf07f0222,
  0xf94b408f,
  0x72eed224,
  0x1fde9e1b,
  0x53a95a3b,
  0x31e7e4a8,
  0xd6005836,
  0x084a2a9d,
  0x85c03095,
  0xcd051cbd,
  0xcb44086a,
  0x01fad276,
  0xeed027a3,
  0x7f974072,
  0xe9c281fc,
  0x7c4f2cac,
  0xeb1ff86b,
  0x0c9bc3ee,
  0xc6b371fb,
  0xc6f1dd58,
  0xf673ea8d,
  0xdcfd52ea,
  0xa40662db,
  0x59325a22,
  0x1ab07985,
  0x448e9198,
  0x8ae7770c,
  0x9bc8be01,
  0xc54da138,
  0xdc36e515,
  0xf9084fd9,
  0xe4e65ded,
  0x75a7bd06,
  0x1ee59fc5,
  0x175279fd,
  0x0b28e484,
  0xed6faf55,
  0x637e65c2,
  0xac88a2c1,
  0xca98eb3a,
  0xd1f5d2e1,
  0x600b387a,
  0x77cbf1e0,
  0xcb195f04,
  0xc5d7fa23,
  0xb787823a,
  0xfd090342,
  0xb3881c77,
  0xfd2172d2,
  0x46e1cdf6,
  0x5fd5a8a0,
  0x20e6c160,
  0x1443de1f,
  0x1925e3c5,
  0xd41e2ce7,
  0x98ff1866,
  0x830a2fc4,
  0xec11278e,
  0xfe85cc93,
  0x8ba90f28,
  0xd8b04323,
  0xca384649,
  0xc40fb367,
  0x2ced1268,
  0x95566f37,
  0x44ac076f,
  0x78cae137,
  0x55837415,
  0x4746dd5a,
  0x0be8a0b0,
  0xda7a8e31,
  0xf7a0d007,
  0xc73e283f,
  0x578714ab,
  0xe02761c1,
  0x7e05e2f8,
  0x22243c7c,
  0x1bfc7132,
  0x1f16a2ae,
  0xa2b3a892,
  0xad720d89,
  0x1cc2f9e4,
  0xbc718f51,
  0xd9478704,
  0x33d6708a,
  0xb3f87d9c,
  0xfd19d0d6,
  0x5b9c0d07,
  0x6124ea99,
  0x84085f7f,
  0x6e76c6ef,
  0x3a468830,
  0x2546a40b,
  0x3c735a45,
  0x45b5d710,
  0x9a7e29cb,
  0x74607977,
  0xac085057,
  0x840e525e,
  0x325cdf13,
  0xdd7fb905,
  0xce37341a,
  0x09604461,
  0x69332adc,
  0x29a4bc69,
  0x473b96fc,
  0x658d9daf,
  0x988e0e6e,
  0x417a439f,
  0x6f89f447,
  0xdfce5aae,
  0xc4012d2f,
  0x288d4142,
  0xd0b8a259,
  0xcb532208,
  0x4391e833,
  0xb91541f8,
  0xdcd0a9e0,
  0x77291a82,
  0xf11e919a,
  0xab15e3ca,
  0xf408a9bb,
  0xaabe35a7,
  0x9cbd41d6,
  0xea0fdb44,
  0x5366088f,
  0x193d06cf,
  0xf74429c1,
  0x1c41db7f,
  0x124826fb,
  0x433cfb77,
  0x847da8b2,
  0x5c3895bb,
  0x9fc4f20f,
  0x81e47b50,
  0xf65dc3a3,
  0x4ebd5ae0,
  0x40e9414d,
  0x82120a6c,
  0x6de3b48f,
  0x96ce7702,
  0xc63311c8,
  0xc999b95c,
  0xb6d719bc,
  0x6248c624,
  0xc26987a2,
  0xe4692c67,
  0x9e889b5f,
  0xe6c2e509,
  0x6a6c9411,
  0x5d44b565,
  0x5207aba7,
  0x00000000,
  0x00000000,
  0x00000001,
};

DskTlsFFDHE dsk_tls_ffdhe6144 =
{
  "ffdhe6144",
  {
    192,
    tls_ffdhe6144_value,
    0x1,    /* Nprime */
    tls_ffdhe6144_barrett_mu
  },
  375
};
static const uint32_t tls_ffdhe8192_value[] = {
  0xffffffff,
  0xffffffff,
  0xc5c6424c,
  0xd68c8bb7,
  0x838ff88c,
  0x011e2a94,
  0xa9f4614e,
  0x0822e506,
  0xf7a8443d,
  0x97d11d49,
  0x30677f0d,
  0xa6bbfde5,
  0xc1fe86fe,
  0x2f741ef8,
  0x5d71a87e,
  0xfafabe1c,
  0xfbe58a30,
  0xded2fbab,
  0x72b0a66e,
  0xb6855dfe,
  0xba8a4fe8,
  0x1efc8ce0,
  0x3f2fa457,
  0x83f81d4a,
  0xa577e231,
  0xa1fe3075,
  0x88d9c0a0,
  0xd5b80194,
  0xad9a95f9,
  0x624816cd,
  0x50c1217b,
  0x99e9e316,
  0x0e423cfc,
  0x51aa691e,
  0x3826e52c,
  0x1c217e6c,
  0x09703fee,
  0x51a8a931,
  0x6a460e74,
  0xbb709987,
  0x9c86b022,
  0x541fc68c,
  0x46fd8251,
  0x59160cc0,
  0x35c35f5c,
  0x2846c0ba,
  0x8b758282,
  0x54504ac7,
  0xd2af05e4,
  0x29388839,
  0xc01bd702,
  0xcb2c0f1c,
  0x7c932665,
  0x555b2f74,
  0xa3ab8829,
  0x86b63142,
  0xf64b10ef,
  0x0b8cc3bd,
  0xedd1cc5e,
  0x687feb69,
  0xc9509d43,
  0xfdb23fce,
  0xd951ae64,
  0x1e425a31,
  0xf600c838,
  0x36ad004c,
  0xcff46aaa,
  0xa40e329c,
  0x7938dad4,
  0xa41d570d,
  0xd43161c1,
  0x62a69526,
  0x9adb1e69,
  0x3fdd4a8e,
  0xdc6b80d6,
  0x5b3b71f9,
  0xc6272b04,
  0xec9d1810,
  0xcacef403,
  0x8ccf2dd5,
  0xc95b9117,
  0xe49f5235,
  0xb854338a,
  0x505dc82d,
  0x1562a846,
  0x62292c31,
  0x6ae77f5e,
  0xd72b0374,
  0x462d538c,
  0xf9c9091b,
  0x47a67cbe,
  0x0ae8db58,
  0x22611682,
  0xb3a739c1,
  0x2a281bf6,
  0xeeaac023,
  0x77caf992,
  0x94c6651e,
  0x94b2bbc1,
  0x763e4e4b,
  0x0077d9b4,
  0x587e38da,
  0x183023c3,
  0x7fb29f8c,
  0xf9e3a26e,
  0x0abec1ff,
  0x350511e3,
  0xa00ef092,
  0xdb6340d8,
  0xb855322e,
  0xa9a96910,
  0xa52471f7,
  0x4cfdb477,
  0x388147fb,
  0x4e46041f,
  0x9b1f5c3e,
  0xfccfec71,
  0xcdad0657,
  0x4c701c3a,
  0xb38e8c33,
  0xb1c0fd4c,
  0x917bdd64,
  0x9b7624c8,
  0x3bb45432,
  0xcaf53ea6,
  0x23ba4442,
  0x38532a3a,
  0x4e677d2c,
  0x45036c7a,
  0x0bfd64b6,
  0x5e0dd902,
  0xc68a007e,
  0xf44182e1,
  0x4db5a851,
  0x7f88a46b,
  0x8ec9b55a,
  0xcec97dcf,
  0x0a8291cd,
  0xf98d0acc,
  0x2a4ecea9,
  0x7140003c,
  0x1a1db93d,
  0x33cb8b7a,
  0x092999a3,
  0x71ad0038,
  0x6dc778f9,
  0x918130c4,
  0xa907600a,
  0x2d9e6832,
  0xed6a1e01,
  0xefb4318a,
  0x7135c886,
  0x7e31cc7a,
  0x87f55ba5,
  0x55034004,
  0x7763cf1d,
  0xd69f6d18,
  0xac7d5f42,
  0xe58857b6,
  0x7930e9e4,
  0x164df4fb,
  0x6e6f52c3,
  0x669e1ef1,
  0x25e41d2b,
  0x3fd59d7c,
  0x3c1b20ee,
  0xfa53ddef,
  0x0abcd06b,
  0xd5c4484e,
  0x1dbf9a42,
  0x9b0deada,
  0xabc52197,
  0x22363a0d,
  0xe86d2bc5,
  0x9c9df69e,
  0x5cae82ab,
  0x71f54bff,
  0x64f2e21e,
  0xe2d74dd3,
  0xf4fd4452,
  0xbc437944,
  0xb4130c93,
  0x85139270,
  0xaefe1309,
  0xc186d91c,
  0x598cb0fa,
  0x91f7f7ee,
  0x7ad91d26,
  0xd6e6c907,
  0x61b46fc9,
  0xf99c0238,
  0xbc34f4de,
  0x6519035b,
  0xde355b3b,
  0x611fcfdc,
  0x886b4238,
  0xc1b2effa,
  0xc6f34a26,
  0x7d1683b2,
  0xc58ef183,
  0x2ec22005,
  0x3bb5fcbc,
  0x4c6fad73,
  0xc3fe3b1b,
  0xeef28183,
  0x8e4f1232,
  0xe98583ff,
  0x9172fe9c,
  0x28342f61,
  0xc03404cd,
  0xcdf7e2ec,
  0x9e02fce1,
  0xee0a6d70,
  0x0b07a7c8,
  0x6372bb19,
  0xae56ede7,
  0xde394df4,
  0x1d4f42a3,
  0x60d7f468,
  0xb96adab7,
  0xb2c8e3fb,
  0xd108a94b,
  0xb324fb61,
  0xbc0ab182,
  0x483a797a,
  0x30acca4f,
  0x36ade735,
  0x1df158a1,
  0xf3efe872,
  0xe2a689da,
  0xe0e68b77,
  0x984f0c70,
  0x7f57c935,
  0xb557135e,
  0x3ded1af3,
  0x85636555,
  0x5f066ed0,
  0x2433f51f,
  0xd5fd6561,
  0xd3df1ed5,
  0xaec4617a,
  0xf681b202,
  0x630c75d8,
  0x7d2fe363,
  0x249b3ef9,
  0xcc939dce,
  0x146433fb,
  0xa9e13641,
  0xce2d3695,
  0xd8b9c583,
  0x273d3cf1,
  0xafdc5620,
  0xa2bb4a9a,
  0xadf85458,
  0xffffffff,
  0xffffffff,
};

static const uint32_t tls_ffdhe8192_barrett_mu = {
  0xbb7a1708,
  0x87e50bba,
  0xc3ab607c,
  0x9e5c82d8,
  0xc5787eb0,
  0x203c6560,
  0xc18e7892,
  0x453c48b4,
  0x3c543be6,
  0x8a6e4b74,
  0xa40aa1c4,
  0xcc8b2ffa,
  0xf3fb1c5a,
  0x0a675b3c,
  0x1967c346,
  0xdd95117e,
  0x60373dd7,
  0x49de3e3f,
  0x5a266dd3,
  0x75fe5a3b,
  0x9ac9c09f,
  0x5fb6aad7,
  0x71229148,
  0x4707ef79,
  0xdc95a19e,
  0x438a2137,
  0xcdfd06d5,
  0x873185e1,
  0xb1178c00,
  0x246fbe31,
  0x78753655,
  0x9b00d272,
  0x9df6fa05,
  0xaf8b6360,
  0x472b4dd4,
  0x5dff157f,
  0x0e687d3f,
  0x3521a01a,
  0xbe261c73,
  0xcd22e149,
  0xe4c735e3,
  0x14e761b2,
  0x0ab69157,
  0x5673c948,
  0xb82438ac,
  0xce92e69e,
  0xe472794d,
  0x5211b955,
  0x4aaad5ae,
  0x1ea4afe4,
  0xf0620e6f,
  0x3d77d106,
  0x4d6da4ed,
  0x8c83c608,
  0x4e468a95,
  0xdc14771e,
  0x88e113bf,
  0x9004e2e0,
  0x8ddd548b,
  0x0252b799,
  0x0c5e920a,
  0x1f2b5977,
  0xdbb08412,
  0x1fdf3d6e,
  0x54f4d9ff,
  0x08fcb7b2,
  0x497ffd68,
  0x67d8683b,
  0xdb2d6fa3,
  0x750727c6,
  0xd3c28a84,
  0xd0b7842d,
  0xab1fd8f9,
  0x8a23ba4b,
  0x670c33ce,
  0x10099139,
  0xed017a12,
  0xa665d518,
  0x196274b8,
  0xf6928678,
  0x7c31bedc,
  0xd8542eac,
  0x92258c85,
  0xa6e4710c,
  0x37fbc7b1,
  0xbdf364d5,
  0x4ce91422,
  0x3650415a,
  0xdd6bc832,
  0x94917464,
  0x8f80c9d9,
  0x3bf5f056,
  0xc4a438c7,
  0x91896b44,
  0x55b2393a,
  0xe1065d5a,
  0xd3dcbbe0,
  0x424aa85c,
  0x9d5a7570,
  0x4e6c8932,
  0x8b7d5c9b,
  0xe9214b85,
  0xf07f0222,
  0xf94b408f,
  0x72eed224,
  0x1fde9e1b,
  0x53a95a3b,
  0x31e7e4a8,
  0xd6005836,
  0x084a2a9d,
  0x85c03095,
  0xcd051cbd,
  0xcb44086a,
  0x01fad276,
  0xeed027a3,
  0x7f974072,
  0xe9c281fc,
  0x7c4f2cac,
  0xeb1ff86b,
  0x0c9bc3ee,
  0xc6b371fb,
  0xc6f1dd58,
  0xf673ea8d,
  0xdcfd52ea,
  0xa40662db,
  0x59325a22,
  0x1ab07985,
  0x448e9198,
  0x8ae7770c,
  0x9bc8be01,
  0xc54da138,
  0xdc36e515,
  0xf9084fd9,
  0xe4e65ded,
  0x75a7bd06,
  0x1ee59fc5,
  0x175279fd,
  0x0b28e484,
  0xed6faf55,
  0x637e65c2,
  0xac88a2c1,
  0xca98eb3a,
  0xd1f5d2e1,
  0x600b387a,
  0x77cbf1e0,
  0xcb195f04,
  0xc5d7fa23,
  0xb787823a,
  0xfd090342,
  0xb3881c77,
  0xfd2172d2,
  0x46e1cdf6,
  0x5fd5a8a0,
  0x20e6c160,
  0x1443de1f,
  0x1925e3c5,
  0xd41e2ce7,
  0x98ff1866,
  0x830a2fc4,
  0xec11278e,
  0xfe85cc93,
  0x8ba90f28,
  0xd8b04323,
  0xca384649,
  0xc40fb367,
  0x2ced1268,
  0x95566f37,
  0x44ac076f,
  0x78cae137,
  0x55837415,
  0x4746dd5a,
  0x0be8a0b0,
  0xda7a8e31,
  0xf7a0d007,
  0xc73e283f,
  0x578714ab,
  0xe02761c1,
  0x7e05e2f8,
  0x22243c7c,
  0x1bfc7132,
  0x1f16a2ae,
  0xa2b3a892,
  0xad720d89,
  0x1cc2f9e4,
  0xbc718f51,
  0xd9478704,
  0x33d6708a,
  0xb3f87d9c,
  0xfd19d0d6,
  0x5b9c0d07,
  0x6124ea99,
  0x84085f7f,
  0x6e76c6ef,
  0x3a468830,
  0x2546a40b,
  0x3c735a45,
  0x45b5d710,
  0x9a7e29cb,
  0x74607977,
  0xac085057,
  0x840e525e,
  0x325cdf13,
  0xdd7fb905,
  0xce37341a,
  0x09604461,
  0x69332adc,
  0x29a4bc69,
  0x473b96fc,
  0x658d9daf,
  0x988e0e6e,
  0x417a439f,
  0x6f89f447,
  0xdfce5aae,
  0xc4012d2f,
  0x288d4142,
  0xd0b8a259,
  0xcb532208,
  0x4391e833,
  0xb91541f8,
  0xdcd0a9e0,
  0x77291a82,
  0xf11e919a,
  0xab15e3ca,
  0xf408a9bb,
  0xaabe35a7,
  0x9cbd41d6,
  0xea0fdb44,
  0x5366088f,
  0x193d06cf,
  0xf74429c1,
  0x1c41db7f,
  0x124826fb,
  0x433cfb77,
  0x847da8b2,
  0x5c3895bb,
  0x9fc4f20f,
  0x81e47b50,
  0xf65dc3a3,
  0x4ebd5ae0,
  0x40e9414d,
  0x82120a6c,
  0x6de3b48f,
  0x96ce7702,
  0xc63311c8,
  0xc999b95c,
  0xb6d719bc,
  0x6248c624,
  0xc26987a2,
  0xe4692c67,
  0x9e889b5f,
  0xe6c2e509,
  0x6a6c9411,
  0x5d44b565,
  0x5207aba7,
  0x00000000,
  0x00000000,
  0x00000001,
};

DskTlsFFDHE dsk_tls_ffdhe8192 =
{
  "ffdhe8192",
  {
    256,
    tls_ffdhe8192_value,
    0x1,    /* Nprime */
    tls_ffdhe8192_barrett_mu
  },
  400
};