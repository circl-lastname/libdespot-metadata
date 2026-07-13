#pragma once

#include <stdio.h>

typedef enum {
  DESPOT_RESULT_SUCCESS,
  DESPOT_RESULT_INTERNAL_ERROR,
  DESPOT_RESULT_OUT_OF_BOUNDS,
  DESPOT_RESULT_UNEXPECTED_EOF,
  DESPOT_RESULT_UNRECOGNIZED_FORMAT,
  DESPOT_RESULT_FORMAT_ERROR,
  DESPOT_RESULT_SEE_ERRNO,
} despot_result_t;

typedef enum {
  DESPOT_TAG_TITLE,
  DESPOT_TAG_ARTIST,
  DESPOT_TAG_ALBUM,
  DESPOT_TAG_ALBUM_ARTIST,
  DESPOT_TAG_TRACK,
  DESPOT_TAG_DISC,
  DESPOT_TAG_TRACK_AMOUNT,
  DESPOT_TAG_DISC_AMOUNT,
  DESPOT_TAG_VENDOR,
} despot_tag_id_t;

typedef enum {
  DESPOT_PICTURE_OTHER,
  DESPOT_PICTURE_PNG_FILE_ICON_32_X_32,
  DESPOT_PICTURE_GENERAL_FILE_ICON,
  DESPOT_PICTURE_FRONT_COVER,
  DESPOT_PICTURE_BACK_COVER,
  DESPOT_PICTURE_LINER_NOTES_PAGE,
  DESPOT_PICTURE_MEDIA_LABEL,
  DESPOT_PICTURE_LEAD_ARTIST_PERFORMER_OR_SOLOIST,
  DESPOT_PICTURE_ARTIST_OR_PERFORMER,
  DESPOT_PICTURE_CONDUCTOR,
  DESPOT_PICTURE_BAND_OR_ORCHESTRA,
  DESPOT_PICTURE_COMPOSER,
  DESPOT_PICTURE_LYRICIST_OR_TEXT_WRITER,
  DESPOT_PICTURE_RECORDING_LOCATION,
  DESPOT_PICTURE_DURING_RECORDING,
  DESPOT_PICTURE_DURING_PERFORMANCE,
  DESPOT_PICTURE_SCREEN_CAPTURE,
  DESPOT_PICTURE_BRIGHT_COLORED_FISH,
  DESPOT_PICTURE_ILLUSTRATION,
  DESPOT_PICTURE_BAND_OR_ARTIST_LOGO,
  DESPOT_PICTURE_PUBLISHER_OR_STUDIO_LOGO,
} despot_picture_type_t;

typedef struct {
  const char* key;
  const char* value;
} despot_tag_t;

typedef struct {
  despot_picture_type_t type;
  const char* media_type;
  const char* description;
  unsigned width;
  unsigned height;
  size_t size;
  size_t source_offset;
} despot_picture_t;

typedef struct despot_ctx_s despot_ctx_t;

despot_result_t despot_read_from_file(despot_ctx_t** ctx, FILE* file);

const despot_tag_t* despot_get_tags(despot_ctx_t* ctx, size_t* amount);
const char* despot_get_tag(despot_ctx_t* ctx, const char* tag);
const char* despot_get_basic_tag(despot_ctx_t* ctx, despot_tag_id_t tag);

const despot_picture_t* despot_get_pictures(despot_ctx_t* ctx, size_t* amount);
despot_result_t despot_load_picture(despot_ctx_t* ctx, unsigned index, void** buffer, size_t* size);
int despot_find_preferred_picture(despot_ctx_t* ctx);

const char* despot_picture_type_to_string(despot_picture_type_t type);

void despot_free_ctx(despot_ctx_t* ctx);

const char* despot_result_to_string(despot_result_t result);
