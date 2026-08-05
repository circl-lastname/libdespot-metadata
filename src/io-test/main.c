#include "shared/io.h"
#include "shared/misc.h"
#include <despot.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

despot_result_t main2() {
  {
    FILE* file = fopen("test/testfile", "wb");
    if (!file)
      return DESPOT_RESULT_SEE_ERRNO;
    io_t io;
    MUST(io_init(&io, file));
    
    puts("Writing values...");
    
    MUST(io_write_u8(&io, 0x12));
    MUST(io_write_i8(&io, -42));
    MUST(io_write_u16_be(&io, 0x1234));
    MUST(io_write_i16_be(&io, -420));
    MUST(io_write_u32_be(&io, 0x12345678));
    MUST(io_write_i32_be(&io, -69420));
    MUST(io_write_u64_be(&io, 0x123456789abcdefa));
    MUST(io_write_i64_be(&io, -6942067));
    
    fclose(file);
  }
  
  {
    FILE* file = fopen("test/testfile", "rb");
    if (!file)
      return DESPOT_RESULT_SEE_ERRNO;
    io_t io;
    MUST(io_init(&io, file));
    
    puts("Reading values...");
    
    uint8_t u8;
    int8_t i8;
    uint16_t u16;
    int16_t i16;
    uint32_t u32;
    int32_t i32;
    uint64_t u64;
    int64_t i64;
    
    MUST(io_read_u8(&io, &u8));
    MUST(io_read_i8(&io, &i8));
    MUST(io_read_u16_be(&io, &u16));
    MUST(io_read_i16_be(&io, &i16));
    MUST(io_read_u32_be(&io, &u32));
    MUST(io_read_i32_be(&io, &i32));
    MUST(io_read_u64_be(&io, &u64));
    MUST(io_read_i64_be(&io, &i64));
    
    printf("u8: %" PRIx8 "\n"
    "i8: %" PRIi8 "\n"
    "u16: %" PRIx16 "\n"
    "i16: %" PRIi16 "\n"
    "u32: %" PRIx32 "\n"
    "i32: %" PRIi32 "\n"
    "u64: %" PRIx64 "\n"
    "i64: %" PRIi64 "\n",
    u8,
    i8,
    u16,
    i16,
    u32,
    i32,
    u64,
    i64);
    
    fclose(file);
  }
  
  return DESPOT_RESULT_SUCCESS;
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  
  despot_result_t result = main2();
  
  if (result != DESPOT_RESULT_SUCCESS) {
    puts(despot_result_to_string(result));
    return 1;
  } else {
    return 0;
  }
}
