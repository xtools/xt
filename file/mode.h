#ifndef xt_file_mode_h
#define xt_file_mode_h

enum xt_file_mode_t {
  XT_FILE_MODE_OPEN_FOR_READ = 0,
  XT_FILE_MODE_TRUNCATE_OR_CREATE_FOR_WRITE = 1,
  XT_FILE_MODE_OPEN_OR_CREATE_FOR_WRITE_AT_END = 2,
  XT_FILE_MODE_OPEN_FOR_READ_AND_WRITE = 3,
  XT_FILE_MODE_TRUNCATE_OR_CREATE_FOR_READ_AND_WRITE = 4,
  XT_FILE_MODE_OPEN_OR_CREATE_FOR_READ_AND_WRITE_AT_END = 5
};
typedef enum xt_file_mode_t xt_file_mode_t;

#endif
