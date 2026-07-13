#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <despot.h>

#include "ctx.h"
#include "shared/misc.h"
#include "shared/stb_ds.h"
#include "vorbis-comment.h"

despot_result_t vorbis_comment_parse(despot_ctx_t* ctx, io_t* io) {
  // Relevant: https://xiph.org/vorbis/doc/v-comment.html
  
  uint32_t vendor_length;
  MUST(io_read_u32_le(io, &vendor_length));
  
  ctx->vendor = malloc(vendor_length+1);
  TRY(io_read(io, ctx->vendor, vendor_length)) CATCH (
    free(ctx->vendor);
    ctx->vendor = NULL;
  );
  ctx->vendor[vendor_length] = '\0';
  
  uint32_t fields_amount;
  MUST(io_read_u32_le(io, &fields_amount));
  
  for (unsigned i = 0; i < fields_amount; i++) {
    uint32_t field_length;
    MUST(io_read_u32_le(io, &field_length));
    
    char* key = malloc(field_length+1);
    TRY(io_read(io, key, field_length)) CATCH (
      free(key);
    );
    key[field_length] = '\0';
    
    char* value = NULL;
    
    for (uint32_t j = 0; j < field_length; j++) {
      if (key[j] == '=') {
        key[j] = '\0';
        value = &key[j+1];
        break;
      }
      
      key[j] = tolower(key[j]);
    }
    
    if (!value) {
      // No '=' sign
      free(key);
      continue;
    }
    
    tag_t* tag = shgetp_null(ctx->metadata, key);
    
    if (!tag) {
      shput(ctx->metadata, key, value);
    } else {
      // Multiple tags of the same type are allowed, but rare and unexpected, so they are merged with ", "
      size_t old_key_length = strlen(tag->key)+1;
      size_t old_field_length = old_key_length+strlen(tag->value)+1;
      size_t value_length = strlen(value)+1;
      
      tag->key = realloc(tag->key, old_field_length+1+value_length);
      tag->value = &tag->key[old_key_length];
      
      tag->key[old_field_length-1] = ',';
      tag->key[old_field_length] = ' ';
      strcpy(&tag->key[old_field_length+1], value);
      
      free(key);
    }
  }
  
  return DESPOT_RESULT_SUCCESS;
}

const char* vorbis_comment_get_basic_tag(despot_ctx_t* ctx, despot_tag_id_t tag) {
  // Relevant: https://wiki.xiph.org/VorbisComment
  
  char* key;
  
  switch (tag) {
    case DESPOT_TAG_TITLE:
      key = "title";
    break;
    case DESPOT_TAG_ARTIST:
      key = "artist";
    break;
    case DESPOT_TAG_ALBUM:
      key = "album";
    break;
    case DESPOT_TAG_ALBUM_ARTIST:
      key = "albumartist";
    break;
    case DESPOT_TAG_TRACK:
      key = "tracknumber";
    break;
    case DESPOT_TAG_DISC:
      key = "discnumber";
    break;
    case DESPOT_TAG_TRACK_AMOUNT:
      key = "tracktotal";
    break;
    case DESPOT_TAG_DISC_AMOUNT:
      key = "disctotal";
    break;
    case DESPOT_TAG_VENDOR:
      return ctx->vendor;
    default:
      return NULL;
  }
  
  return shget(ctx->metadata, key);
}
