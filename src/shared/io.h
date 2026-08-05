#pragma once

#include <endian.h>
#include <stddef.h>
#include <stdint.h>

#include <despot.h>

#include "shared/misc.h"

typedef struct {
  FILE* file;
  size_t size;
} io_t;

despot_result_t io_init(io_t* io, FILE* file);
despot_result_t io_seek(io_t* io, size_t offset);
despot_result_t io_skip(io_t* io, size_t amount);
size_t io_tell(io_t* io);
despot_result_t io_read(io_t* io, void* buffer, size_t amount);
despot_result_t io_write(io_t* io, const void* buffer, size_t amount);

static inline despot_result_t io_read_u8(io_t* io, uint8_t* integer) {
  return io_read(io, integer, 1);
}

static inline despot_result_t io_read_i8(io_t* io, int8_t* integer) {
  return io_read(io, integer, 1);
}

static inline despot_result_t io_read_u16_le(io_t* io, uint16_t* integer) {
  uint16_t integer_temp;
  MUST(io_read(io, &integer_temp, 2));
  *integer = le16toh(integer_temp);
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_read_i16_le(io_t* io, int16_t* integer) {
  uint16_t integer_temp;
  MUST(io_read(io, &integer_temp, 2));
  *(uint16_t*)integer = le16toh(integer_temp);
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_read_u16_be(io_t* io, uint16_t* integer) {
  uint16_t integer_temp;
  MUST(io_read(io, &integer_temp, 2));
  *integer = be16toh(integer_temp);
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_read_i16_be(io_t* io, int16_t* integer) {
  uint16_t integer_temp;
  MUST(io_read(io, &integer_temp, 2));
  *(uint16_t*)integer = be16toh(integer_temp);
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_read_u24_be(io_t* io, uint32_t* integer) {
  uint8_t integer_temp[3];
  MUST(io_read(io, &integer_temp, 3));
  *integer = integer_temp[2] | integer_temp[1] << 8 | integer_temp[0] << 16;
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_read_u32_le(io_t* io, uint32_t* integer) {
  uint32_t integer_temp;
  MUST(io_read(io, &integer_temp, 4));
  *integer = le32toh(integer_temp);
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_read_i32_le(io_t* io, int32_t* integer) {
  uint32_t integer_temp;
  MUST(io_read(io, &integer_temp, 4));
  *(uint32_t*)integer = le32toh(integer_temp);
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_read_u32_be(io_t* io, uint32_t* integer) {
  uint32_t integer_temp;
  MUST(io_read(io, &integer_temp, 4));
  *integer = be32toh(integer_temp);
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_read_i32_be(io_t* io, int32_t* integer) {
  uint32_t integer_temp;
  MUST(io_read(io, &integer_temp, 4));
  *(uint32_t*)integer = be32toh(integer_temp);
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_read_u64_be(io_t* io, uint64_t* integer) {
  uint64_t integer_temp;
  MUST(io_read(io, &integer_temp, 8));
  *integer = be64toh(integer_temp);
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_read_i64_be(io_t* io, int64_t* integer) {
  uint64_t integer_temp;
  MUST(io_read(io, &integer_temp, 8));
  *(uint64_t*)integer = be64toh(integer_temp);
  return DESPOT_RESULT_SUCCESS;
}

static inline despot_result_t io_write_u8(io_t* io, uint8_t integer) {
  return io_write(io, &integer, 1);
}

static inline despot_result_t io_write_i8(io_t* io, int8_t integer) {
  return io_write(io, &integer, 1);
}

static inline despot_result_t io_write_u16_be(io_t* io, uint16_t integer) {
  uint16_t integer_temp = htobe16(integer);
  return io_write(io, &integer_temp, 2);
}

static inline despot_result_t io_write_i16_be(io_t* io, int16_t integer) {
  uint16_t integer_temp = htobe16(*(uint16_t*)&integer);
  return io_write(io, &integer_temp, 2);
}

static inline despot_result_t io_write_u32_be(io_t* io, uint32_t integer) {
  uint32_t integer_temp = htobe32(integer);
  return io_write(io, &integer_temp, 4);
}

static inline despot_result_t io_write_i32_be(io_t* io, int32_t integer) {
  uint32_t integer_temp = htobe32(*(uint32_t*)&integer);
  return io_write(io, &integer_temp, 4);
}

static inline despot_result_t io_write_u64_be(io_t* io, uint64_t integer) {
  uint64_t integer_temp = htobe64(integer);
  return io_write(io, &integer_temp, 8);
}

static inline despot_result_t io_write_i64_be(io_t* io, int64_t integer) {
  uint64_t integer_temp = htobe64(*(uint64_t*)&integer);
  return io_write(io, &integer_temp, 8);
}
