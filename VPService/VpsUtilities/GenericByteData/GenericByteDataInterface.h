#ifndef __GENERICBYTEDATA_INTERFACE_H__
#define __GENERICBYTEDATA_INTERFACE_H__

#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

  typedef unsigned long long gbd_timestamp_t;
  typedef unsigned int gbd_length_t;
  typedef unsigned int gbd_codec_t;
  typedef unsigned int gbd_sequence_number_t;
  typedef unsigned int gbd_flags_t;
  typedef void * gbd_t;

  gbd_t create_genericbytedata(unsigned char * data, gbd_length_t length, bool shouldGenerateHeader, bool shouldCopyData);
  void destroy_genericbytedata(gbd_t p);
  gbd_length_t get_length(gbd_t p);
  unsigned char * get_data(gbd_t p);
  gbd_length_t get_body_length(gbd_t p);
  unsigned char * get_body_data(gbd_t p);
  void set_codec(gbd_t p, gbd_codec_t codec);
  gbd_codec_t get_codec(gbd_t p);
  void set_sequence_number(gbd_t p, gbd_sequence_number_t seqNo);
  gbd_sequence_number_t get_sequence_number(gbd_t p);
  void set_flags(gbd_t p, gbd_flags_t flags);
  gbd_flags_t get_flags(gbd_t p);
  void set_sync_timestamp(gbd_t p, gbd_timestamp_t ts);
  gbd_timestamp_t get_sync_timestamp(gbd_t p);
  void set_timestamp(gbd_t p, gbd_timestamp_t ts);
  gbd_timestamp_t get_timestamp(gbd_t p);

#ifdef __cplusplus
}
#endif

#endif // __GENERICBYTEDATA_INTERFACE_H__
