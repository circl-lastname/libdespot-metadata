#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <despot.h>

#include "ctx.h"
#include "flac.h"
#include "shared/misc.h"
#include "shared/stb_ds.h"

static const uint8_t picture_type_score[] = {
  [DESPOT_PICTURE_FRONT_COVER] = 6,
  [DESPOT_PICTURE_MEDIA_LABEL] = 5,
  [DESPOT_PICTURE_ILLUSTRATION] = 5,
  [DESPOT_PICTURE_BACK_COVER] = 4,
  [DESPOT_PICTURE_LINER_NOTES_PAGE] = 4,
  [DESPOT_PICTURE_BAND_OR_ARTIST_LOGO] = 3,
  [DESPOT_PICTURE_PUBLISHER_OR_STUDIO_LOGO] = 3,
  [DESPOT_PICTURE_LEAD_ARTIST_PERFORMER_OR_SOLOIST] = 2,
  [DESPOT_PICTURE_ARTIST_OR_PERFORMER] = 2,
  [DESPOT_PICTURE_CONDUCTOR] = 2,
  [DESPOT_PICTURE_BAND_OR_ORCHESTRA] = 2,
  [DESPOT_PICTURE_COMPOSER] = 2,
  [DESPOT_PICTURE_LYRICIST_OR_TEXT_WRITER] = 2,
  [DESPOT_PICTURE_RECORDING_LOCATION] = 2,
  [DESPOT_PICTURE_DURING_RECORDING] = 2,
  [DESPOT_PICTURE_DURING_PERFORMANCE] = 2,
  [DESPOT_PICTURE_OTHER] = 1,
  [DESPOT_PICTURE_SCREEN_CAPTURE] = 1,
  [DESPOT_PICTURE_PNG_FILE_ICON_32_X_32] = 0,
  [DESPOT_PICTURE_GENERAL_FILE_ICON] = 0,
  [DESPOT_PICTURE_BRIGHT_COLORED_FISH] = 0,
};

static despot_result_t parse(despot_ctx_t* ctx, io_t* io) {
  uint32_t magic_number;
  despot_result_t read_result = io_read_u32_be(io, &magic_number);
  
  if (read_result == DESPOT_RESULT_UNEXPECTED_EOF) {
    return DESPOT_RESULT_UNRECOGNIZED_FORMAT;
  } else if (read_result != DESPOT_RESULT_SUCCESS) {
    return read_result;
  }
  
  switch (magic_number) {
    // FLAC
    // Relevant: https://www.rfc-editor.org/rfc/rfc9639.html
    case 0x664c6143:
      return flac_parse(ctx, io);
  }
  
  return DESPOT_RESULT_UNRECOGNIZED_FORMAT;
}

EXPORT despot_result_t despot_read_from_fd(despot_ctx_t** ctx, int fd) {
  *ctx = calloc(1, sizeof(despot_ctx_t));
  
  (*ctx)->io.source = IO_SOURCE_FD;
  (*ctx)->io.fd = dup(fd);
  
  if ((*ctx)->io.fd < 0) {
    free(*ctx);
    *ctx = NULL;
    return DESPOT_RESULT_SEE_ERRNO;
  }
  
  if (lseek((*ctx)->io.fd, 0, SEEK_SET) < 0) {
    free(*ctx);
    *ctx = NULL;
    return DESPOT_RESULT_SEE_ERRNO;
  }
  
  return parse(*ctx, &(*ctx)->io);
}

EXPORT despot_result_t despot_read_from_file(despot_ctx_t** ctx, FILE* file) {
  int fd = fileno(file);
  if (fd < 0) {
    return DESPOT_RESULT_SEE_ERRNO;
  }
  
  return despot_read_from_fd(ctx, fd);
}

EXPORT despot_result_t despot_read_from_mem(despot_ctx_t** ctx, void* buffer, size_t size) {
  *ctx = calloc(1, sizeof(despot_ctx_t));
  
  (*ctx)->io.source = IO_SOURCE_MEM;
  (*ctx)->io.mem.buffer = buffer;
  (*ctx)->io.mem.size = size;
  
  return parse(*ctx, &(*ctx)->io);
}

EXPORT despot_tag_t* despot_get_tags(despot_ctx_t* ctx, size_t* amount) {
  *amount = shlenu(ctx->metadata);
  return ctx->metadata;
}

EXPORT const char* despot_get_tag(despot_ctx_t* ctx, const char* tag) {
  return shget(ctx->metadata, tag);
}

EXPORT const char* despot_get_basic_tag(despot_ctx_t* ctx, despot_tag_id_t tag) {
  return ctx->get_basic_tag(ctx, tag);
}

EXPORT despot_picture_t* despot_get_pictures(despot_ctx_t* ctx, size_t* amount) {
  *amount = arrlenu(ctx->pictures);
  return ctx->pictures;
}

EXPORT despot_result_t despot_load_picture(despot_ctx_t* ctx, unsigned index, void** buffer, size_t* size) {
  if (index >= arrlenu(ctx->pictures)) {
    return DESPOT_RESULT_OUT_OF_BOUNDS;
  }
  
  size_t picture_size = ctx->pictures[index].size;
  
  if (size) {
    *size = picture_size;
  }
  
  // TODO: Figure out if ID3 unsynchronisation is needed nowadays, then call into an format specific function
  *buffer = malloc(picture_size);
  MUST(io_seek(&ctx->io, ctx->pictures[index].source_offset));
  MUST(io_read(&ctx->io, *buffer, picture_size));
  
  return DESPOT_RESULT_SUCCESS;
}

EXPORT int despot_find_preferred_picture(despot_ctx_t* ctx) {
  size_t pictures = arrlenu(ctx->pictures);
  
  if (pictures == 0) {
    return -1;
  } else if (pictures == 1) {
    return 0;
  }
  
  size_t index = 0;
  int current_score = -1;
  
  for (size_t i = 0; i < pictures; i++) {
    int score = picture_type_score[ctx->pictures[i].type];
    if (score > current_score) {
      index = i;
      current_score = score;
    }
  }
  
  return index;
}

EXPORT const char* despot_picture_type_to_string(despot_picture_type_t type) {
  // Relevant: https://www.rfc-editor.org/rfc/rfc9639.html#table-13
  
  switch (type) {
    case DESPOT_PICTURE_OTHER:
      return "Other";
    case DESPOT_PICTURE_PNG_FILE_ICON_32_X_32:
      return "PNG file icon of 32x32 pixels";
    case DESPOT_PICTURE_GENERAL_FILE_ICON:
      return "General file icon";
    case DESPOT_PICTURE_FRONT_COVER:
      return "Front cover";
    case DESPOT_PICTURE_BACK_COVER:
      return "Back cover";
    case DESPOT_PICTURE_LINER_NOTES_PAGE:
      return "Liner notes page";
    case DESPOT_PICTURE_MEDIA_LABEL:
      return "Media label";
    case DESPOT_PICTURE_LEAD_ARTIST_PERFORMER_OR_SOLOIST:
      return "Lead artist, lead performer, or soloist";
    case DESPOT_PICTURE_ARTIST_OR_PERFORMER:
      return "Artist or performer";
    case DESPOT_PICTURE_CONDUCTOR:
      return "Conductor";
    case DESPOT_PICTURE_BAND_OR_ORCHESTRA:
      return "Band or orchestra";
    case DESPOT_PICTURE_COMPOSER:
      return "Composer";
    case DESPOT_PICTURE_LYRICIST_OR_TEXT_WRITER:
      return "Lyricist or text writer";
    case DESPOT_PICTURE_RECORDING_LOCATION:
      return "Recording location";
    case DESPOT_PICTURE_DURING_RECORDING:
      return "During recording";
    case DESPOT_PICTURE_DURING_PERFORMANCE:
      return "During performance";
    case DESPOT_PICTURE_SCREEN_CAPTURE:
      return "Movie or video screen capture";
    case DESPOT_PICTURE_BRIGHT_COLORED_FISH:
      return "A bright colored fish";
    case DESPOT_PICTURE_ILLUSTRATION:
      return "Illustration";
    case DESPOT_PICTURE_BAND_OR_ARTIST_LOGO:
      return "Band or artist logo";
    case DESPOT_PICTURE_PUBLISHER_OR_STUDIO_LOGO:
      return "Publisher or studio logo";
    default:
      return "Unknown";
  }
}

EXPORT void despot_free_ctx(despot_ctx_t* ctx) {
  if (ctx->io.source == IO_SOURCE_FD) {
    close(ctx->io.fd);
  }
  
  if (ctx->vendor) {
    free(ctx->vendor);
  }
  
  size_t metadata_keys = shlenu(ctx->metadata);
  for (size_t i = 0; i < metadata_keys; i++) {
    free(ctx->metadata[i].key);
  }
  
  shfree(ctx->metadata);
  
  size_t pictures = arrlenu(ctx->pictures);
  for (size_t i = 0; i < pictures; i++) {
    free((char*)ctx->pictures[i].media_type);
    free((char*)ctx->pictures[i].description);
  }
  
  arrfree(ctx->pictures);
  
  free(ctx);
}

EXPORT const char* despot_result_to_string(despot_result_t result) {
  switch (result) {
    case DESPOT_RESULT_SUCCESS:
      return "Success";
    case DESPOT_RESULT_INTERNAL_ERROR:
      return "Internal error, please report a bug to despot";
    case DESPOT_RESULT_OUT_OF_BOUNDS:
      return "Attempted to index out of bounds";
    case DESPOT_RESULT_UNEXPECTED_EOF:
      return "Unexpected end of file";
    case DESPOT_RESULT_UNRECOGNIZED_FORMAT:
      return "Unrecognized format";
    case DESPOT_RESULT_FORMAT_ERROR:
      return "File format error";
    case DESPOT_RESULT_SEE_ERRNO:
      return strerror(errno);
    default:
      return "Unknown";
  }
}
