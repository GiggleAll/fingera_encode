
#include <fingera/encode/compress.h>

#include <string.h>

#include <fingera/encode/hex.h>
#include <gtest/gtest.h>

#define EXPECT_ZERO(x) EXPECT_EQ((x), 0)

TEST(compress, unsigned_integer) {
  struct {
    uint64_t value;
    uint8_t buf[10];
    size_t buf_len;
  } test_unsigned_list[] = {
      {0, {0x00}, 1},
      {0x7F, {0x7F}, 1},
      {0x80, {0x80, 0x00}, 2},
      {0x81, {0x80, 0x01}, 2},
      {0x407F, {0xFF, 0x7F}, 2},
      {0x4080, {0x80, 0x80, 0x00}, 3},
      {0x4081, {0x80, 0x80, 0x01}, 3},
      {0x20407F, {0xFF, 0xFF, 0x7F}, 3},
      {0x204080, {0x80, 0x80, 0x80, 0x00}, 4},
      {0x204081, {0x80, 0x80, 0x80, 0x01}, 4},
      {0x1020407FUL, {0xFF, 0xFF, 0xFF, 0x7F}, 4},
      {0x10204080UL, {0x80, 0x80, 0x80, 0x80, 0x00}, 5},
      {0x10204081UL, {0x80, 0x80, 0x80, 0x80, 0x01}, 5},
      {0x081020407FULL, {0xFF, 0xFF, 0xFF, 0xFF, 0x7F}, 5},
      {0x0810204080ULL, {0x80, 0x80, 0x80, 0x80, 0x80, 0x00}, 6},
      {0x0810204081ULL, {0x80, 0x80, 0x80, 0x80, 0x80, 0x01}, 6},
      {0x04081020407FULL, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F}, 6},
      {0x040810204080ULL, {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00}, 7},
      {0x040810204081ULL, {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01}, 7},
      {0x0204081020407FULL, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F}, 7},
      {0x02040810204080ULL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00},
       8},
      {0x02040810204081ULL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01},
       8},
      {0x010204081020407FULL,
       {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F},
       8},
      {0x0102040810204080ULL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00},
       9},
      {0x0102040810204081ULL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01},
       9},
      {0x810204081020407FULL,
       {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F},
       9},
      {0xFFFFFFFFFFFFFFFFULL,
       {0x80, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x7F},
       10},
  };

  uint8_t out[10];
  uint64_t out_value;

  for (size_t i = 0;
       i < sizeof(test_unsigned_list) / sizeof(test_unsigned_list[0]); i++) {
    size_t len = varint_encode(test_unsigned_list[i].value, out);
    EXPECT_EQ(len, test_unsigned_list[i].buf_len);
    EXPECT_ZERO(memcmp(test_unsigned_list[i].buf, out, len));

    len = varint_decode(out, len, &out_value);
    EXPECT_EQ(len, test_unsigned_list[i].buf_len);
    EXPECT_EQ(out_value, test_unsigned_list[i].value);
  }
}

TEST(compress, signed_integer) {
  struct {
    int64_t value;
    uint8_t buf[10];
    size_t buf_len;
  } test_signed_list[] = {
      {0, {0x00}, 1},
      {0x3F, {0x7E}, 1},
      {0x40, {0x80, 0x00}, 2},
      {0x41, {0x80, 0x02}, 2},
      {0x203F, {0xFF, 0x7E}, 2},
      {0x2040, {0x80, 0x80, 0x00}, 3},
      {0x2041, {0x80, 0x80, 0x02}, 3},
      {0x10203F, {0xFF, 0xFF, 0x7E}, 3},
      {0x102040, {0x80, 0x80, 0x80, 0x00}, 4},
      {0x102041, {0x80, 0x80, 0x80, 0x02}, 4},
      {0x0810203F, {0xFF, 0xFF, 0xFF, 0x7E}, 4},
      {0x08102040, {0x80, 0x80, 0x80, 0x80, 0x00}, 5},
      {0x08102041, {0x80, 0x80, 0x80, 0x80, 0x02}, 5},
      {0x040810203F, {0xFF, 0xFF, 0xFF, 0xFF, 0x7E}, 5},
      {0x0408102040, {0x80, 0x80, 0x80, 0x80, 0x80, 0x00}, 6},
      {0x0408102041, {0x80, 0x80, 0x80, 0x80, 0x80, 0x02}, 6},
      {0x02040810203F, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E}, 6},
      {0x020408102040, {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00}, 7},
      {0x020408102041, {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x02}, 7},
      {0x0102040810203F, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E}, 7},
      {0x01020408102040, {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00}, 8},
      {0x01020408102041, {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x02}, 8},
      {0x08102040810203F, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E}, 8},
      {0x081020408102040,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00},
       9},
      {0x081020408102041,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x02},
       9},
      {0x408102040810203FULL,
       {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E},
       9},
      {0x4081020408102040ULL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00},
       10},
      {0x4081020408102041ULL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x02},
       10},
      {0x7FFFFFFFFFFFFFFFULL,
       {0x80, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x7E},
       10},
      {-0x3FLL, {0x7F}, 1},
      {-0x40LL, {0x80, 0x01}, 2},
      {-0x41LL, {0x80, 0x03}, 2},
      {-0x203FLL, {0xFF, 0x7F}, 2},
      {-0x2040LL, {0x80, 0x80, 0x01}, 3},
      {-0x2041LL, {0x80, 0x80, 0x03}, 3},
      {-0x10203FLL, {0xFF, 0xFF, 0x7F}, 3},
      {-0x102040LL, {0x80, 0x80, 0x80, 0x01}, 4},
      {-0x102041LL, {0x80, 0x80, 0x80, 0x03}, 4},
      {-0x0810203FLL, {0xFF, 0xFF, 0xFF, 0x7F}, 4},
      {-0x08102040LL, {0x80, 0x80, 0x80, 0x80, 0x01}, 5},
      {-0x08102041LL, {0x80, 0x80, 0x80, 0x80, 0x03}, 5},
      {-0x040810203FLL, {0xFF, 0xFF, 0xFF, 0xFF, 0x7F}, 5},
      {-0x0408102040LL, {0x80, 0x80, 0x80, 0x80, 0x80, 0x01}, 6},
      {-0x0408102041LL, {0x80, 0x80, 0x80, 0x80, 0x80, 0x03}, 6},
      {-0x02040810203FLL, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F}, 6},
      {-0x020408102040LL, {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01}, 7},
      {-0x020408102041LL, {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x03}, 7},
      {-0x0102040810203FLL, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F}, 7},
      {-0x01020408102040LL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01},
       8},
      {-0x01020408102041LL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x03},
       8},
      {-0x08102040810203FLL,
       {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F},
       8},
      {-0x081020408102040LL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01},
       9},
      {-0x081020408102041LL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x03},
       9},
      {-0x408102040810203FLL,
       {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F},
       9},
      {-0x4081020408102040LL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01},
       10},
      {-0x4081020408102041LL,
       {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x03},
       10},
      {-0x7FFFFFFFFFFFFFFFLL,
       {0x80, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x7F},
       10},
  };

  uint8_t out[10];
  int64_t out_value;

  for (size_t i = 0; i < sizeof(test_signed_list) / sizeof(test_signed_list[0]);
       i++) {
    size_t len = zigzag_encode(test_signed_list[i].value, out);
    EXPECT_EQ(len, test_signed_list[i].buf_len);
    EXPECT_ZERO(memcmp(test_signed_list[i].buf, out, len));

    len = zigzag_decode(test_signed_list[i].buf, test_signed_list[i].buf_len,
                        &out_value);
    EXPECT_EQ(len, test_signed_list[i].buf_len);
    EXPECT_EQ(out_value, test_signed_list[i].value);
  }
}

TEST(compress, illegal_integer) {
  uint8_t buffer[16] = {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  };
  uint64_t uout;
  int64_t iout;
  for (size_t i = 0; i < 16; i++) {
    EXPECT_ZERO(varint_decode(buffer, i, &uout));
    EXPECT_ZERO(zigzag_decode(buffer, i, &iout));
  }
}
