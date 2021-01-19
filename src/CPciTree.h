#ifndef CPCITREE_H
#define CPCITREE_H
#include "CFsProvider.h"
#include <string.h>
#include <set>
#include <vector>
#include <fcntl.h>
class CPciTree
{
public:
    enum consts_e {MAX_BUFFER=512};
    struct filectl_t
    {
        FILE *fp {nullptr};
        char buffer[MAX_BUFFER];
        char *pbuffer {buffer};
        size_t max {MAX_BUFFER - 1};

        ssize_t readline()
        {
            ssize_t t = getline(&pbuffer, &max, fp);
            if(t > 0) pbuffer[t] = 0x00;
            return t;
        }
    };

    struct pcidesc_t
    {
        std::string id;
        std::string scopeid;
        std::string busid;
        std::string devid;
        std::string vendor;
        std::string device;
        std::string subvendor;
        std::string subdevice;
        std::string pciclass;
        std::string driver;
    };
    struct strvalues_t
    {
        std::string key;
        std::string value;
    };

    struct uevent_t
    {
        strvalues_t driver, pciclass, sys, subsys;
    };

    struct treerec_t
    {
        int dev {0};
        int con1{-1}, con2{-1};
        bool root {true};
        std::vector<treerec_t> childs;
    };

public:
    std::vector<std::string> ueventkeys;
    uevent_t curruev;

    std::vector<pcidesc_t> descs;
    std::string basedir {"/sys/bus/pci/devices/"};
    std::string descdb {"/usr/share/misc/pci.ids"};
    std::set<std::string> directories;
    CFsProvider fs1;
    struct dirent *entry = nullptr;
    std::vector<treerec_t> tree;
public:
    CPciTree();
    bool read_base_dir();
    bool read_uevents();
    bool read_uevent(char *path);
    bool test_uevent_entry(char *str, uevent_t &uevent, short *mask, uint masksz);
    bool decode(uevent_t &ev, pcidesc_t &desc);
    bool get_desc_from_key(const char *buffer, const char *key, const int tabs, std::string &desc);
    std::string search_class(std::string &classid, filectl_t &file);
    void build_pci_tree();
    void show_rec(size_t idx);

};

#endif // CPCITREE_H
