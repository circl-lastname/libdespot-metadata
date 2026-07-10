#include <despot.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void print_usage(const char* name) {
  fprintf(stderr, "Usage: %s [-v] FILE\n", name);
}

void try_get_metadata(despot_ctx_t* ctx, despot_tag_id_t tag, char* name) {
  const char* value = despot_get_basic_tag(ctx, tag);
  if (value) {
    printf("%s: %s\n", name, value);
  }
}

int main(int argc, char** argv) {
  const char* filename = NULL;
  bool be_verbose = false;
  
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v")) {
      if (!be_verbose) {
        be_verbose = true;
      } else {
        print_usage(argv[0]);
        return 1;
      }
    } else {
      if (!filename) {
        filename = argv[i];
      } else {
        print_usage(argv[0]);
        return 1;
      }
    }
  }
  
  if (!filename) {
    print_usage(argv[0]);
    return 1;
  }
  
  FILE* file = fopen(filename, "rb");
  if (!file) {
    fprintf(stderr, "%s: Failed to open file: %s\n", argv[0], strerror(errno));
    return 1;
  }
  
  despot_ctx_t* ctx;
  despot_result_t result;
  if ((result = despot_read_from_file(&ctx, file)) != DESPOT_RESULT_SUCCESS) {
    fprintf(stderr, "%s: Failed to read metadata: %s\n", argv[0], despot_result_to_string(result));
    return 1;
  }
  
  fclose(file);
  
  try_get_metadata(ctx, DESPOT_TAG_TITLE, "Title");
  try_get_metadata(ctx, DESPOT_TAG_ARTIST, "Artist");
  try_get_metadata(ctx, DESPOT_TAG_ALBUM, "Album");
  try_get_metadata(ctx, DESPOT_TAG_ALBUM_ARTIST, "Album artist");
  try_get_metadata(ctx, DESPOT_TAG_TRACK, "Track");
  try_get_metadata(ctx, DESPOT_TAG_DISC, "Disc");
  try_get_metadata(ctx, DESPOT_TAG_TRACK_AMOUNT, "Total tracks");
  try_get_metadata(ctx, DESPOT_TAG_DISC_AMOUNT, "Total discs");
  try_get_metadata(ctx, DESPOT_TAG_VENDOR, "Vendor");
  
  if (be_verbose) {
    puts("\nRaw tags:");
    
    size_t tags_amount;
    despot_tag_t* tags = despot_get_tags(ctx, &tags_amount);
    
    for (size_t i = 0; i < tags_amount; i++) {
      printf("  %s = %s\n", tags[i].key, tags[i].value);
    }
  }
  
  size_t pictures_amount;
  despot_picture_t* pictures = despot_get_pictures(ctx, &pictures_amount);
  
  for (size_t i = 0; i < pictures_amount; i++) {
    printf("\nPicture %zu:\n", i+1);
    printf("  Type: %s\n", despot_picture_type_to_string(pictures[i].type));
    printf("  Media type: %s\n", pictures[i].media_type);
    printf("  Description: %s\n", pictures[i].description);
    printf("  Width: %u\n", pictures[i].width);
    printf("  Height: %u\n", pictures[i].height);
    printf("  Size: %zu\n", pictures[i].size);
  }
  
  despot_free_ctx(ctx);
  
  return 0;
}
