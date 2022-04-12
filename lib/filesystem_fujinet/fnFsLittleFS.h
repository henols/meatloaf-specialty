#ifndef _FN_FSLITTLEFS_
#define _FN_FSLITTLEFS_

#include <dirent.h>

#include "fnFS.h"


class FileSystemLittleFS : public FileSystem
{
private:
    DIR * _dir;
public:
    FileSystemLittleFS();
    bool start();
    
    fsType type() override { return FSTYPE_LITTLEFS; };
    const char * typestring() override { return type_to_string(FSTYPE_LITTLEFS); };

    virtual bool is_global() override { return true; };    

    FILE * file_open(const char* path, const char* mode = FILE_READ) override;

    bool exists(const char* path) override;

    bool remove(const char* path) override;

    bool rename(const char* pathFrom, const char* pathTo) override;

    virtual bool mkdir(const char* path) override { return true; };
    virtual bool rmdir(const char* path) override { return true; };
    virtual bool dir_exists(const char* path) override { return true; };

    bool dir_open(const char * path, const char *pattern, uint16_t diropts) override;
    fsdir_entry *dir_read() override;
    void dir_close() override;
    uint16_t dir_tell() override;
    bool dir_seek(uint16_t) override;

    uint64_t total_bytes();
    uint64_t used_bytes();
};

extern FileSystemLittleFS fnLITTLEFS;

#endif // _FN_FSLITTLEFS_
