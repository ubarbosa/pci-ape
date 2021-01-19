#ifndef CFSPROVIDER_H
#define CFSPROVIDER_H
#include <string>
#include <dirent.h>

class CFsProvider
{
private:
    DIR             *pdir = nullptr;
    struct dirent   *pentry = nullptr;
    std::string currdir {""};
public:
    CFsProvider();
    bool open_dir(const char* path);
    void close_dir();
    struct dirent *get_next_entry();

};

#endif // CFSPROVIDER_H
