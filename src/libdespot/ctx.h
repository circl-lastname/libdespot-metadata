#pragma once

#include <despot.h>

#include "shared/io.h"

typedef const char* (*ctx_get_basic_tag_t)(despot_ctx_t* ctx, despot_tag_id_t tag);

typedef struct {
  char* key;
  char* value;
} tag_t;

struct despot_ctx_s {
  io_t io;
  char* vendor; // Only used by Vorbis comments
  tag_t* metadata;
  despot_picture_t* pictures;
  ctx_get_basic_tag_t get_basic_tag;
};
