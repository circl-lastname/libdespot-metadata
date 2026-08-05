#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <despot.h>

#include "io.h"

despot_result_t io_init(io_t* io, FILE* file) {
  io->file = file;
  
  if (fseek(file, 0, SEEK_END) < 0) {
    return DESPOT_RESULT_SEE_ERRNO;
  }
  
  io->size = ftell(file);
  
  if (fseek(file, 0, SEEK_SET) < 0) {
    return DESPOT_RESULT_SEE_ERRNO;
  }
  
  return DESPOT_RESULT_SUCCESS;
}

despot_result_t io_seek(io_t* io, size_t offset) {
  if (offset >= io->size) {
    return DESPOT_RESULT_UNEXPECTED_EOF;
  }
  
  if (fseek(io->file, offset, SEEK_SET) < 0) {
    return DESPOT_RESULT_SEE_ERRNO;
  }
  
  return DESPOT_RESULT_SUCCESS;
}

despot_result_t io_skip(io_t* io, size_t amount) {
  if (amount == 0) {
    return DESPOT_RESULT_SUCCESS;
  }
  
  if (ftell(io->file)+amount >= io->size) {
    return DESPOT_RESULT_UNEXPECTED_EOF;
  }
  
  if (fseek(io->file, amount, SEEK_CUR) < 0) {
    return DESPOT_RESULT_SEE_ERRNO;
  }
  
  return DESPOT_RESULT_SUCCESS;
}

size_t io_tell(io_t* io) {
  return ftell(io->file);
}

despot_result_t io_read(io_t* io, void* buffer, size_t amount) {
  if (amount == 0) {
    return DESPOT_RESULT_SUCCESS;
  }
  
  size_t amount_read = 0;
  
  while (amount_read < amount) {
    size_t read_result = fread(buffer+amount_read, 1, amount-amount_read, io->file);
    
    if (read_result < amount-amount_read) {
      if (feof(io->file)) {
        return DESPOT_RESULT_UNEXPECTED_EOF;
      }
      
      if (ferror(io->file)) {
        // Can't find a manpage that says fread or ferror set errno but whatever
        return DESPOT_RESULT_SEE_ERRNO;
      }
    }
    
    amount_read += read_result;
  }
  
  return DESPOT_RESULT_SUCCESS;
}

despot_result_t io_write(io_t* io, const void* buffer, size_t amount) {
  if (amount == 0) {
    return DESPOT_RESULT_SUCCESS;
  }
  
  size_t amount_written = 0;
  
  while (amount_written < amount) {
    size_t write_result = fwrite(buffer+amount_written, 1, amount-amount_written, io->file);
    
    if (write_result < amount-amount_written) {
      if (ferror(io->file)) {
        // Can't find a manpage that says fwrite or ferror set errno but whatever
        return DESPOT_RESULT_SEE_ERRNO;
      }
    }
    
    amount_written += write_result;
  }
  
  return DESPOT_RESULT_SUCCESS;
}
