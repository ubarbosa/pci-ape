/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    made by chimpa 2021
*/


#include "CPciTree.h"
#include <fcntl.h>

CPciTree::CPciTree()
{
    curruev.driver.key = "DRIVER";
    curruev.pciclass.key = "PCI_CLASS";
    curruev.sys.key = "PCI_ID";
    curruev.subsys.key = "PCI_SUBSYS_ID";
}

bool CPciTree::read_base_dir()
{
    if(!fs1.open_dir(basedir.c_str())) return false;
    while(true)
    {
        entry = fs1.get_next_entry();
        if(!entry) break;
        if(entry->d_type == 0x08) continue;
        if(strcmp(entry->d_name, ".") == 0) continue;
        if(strcmp(entry->d_name, "..") == 0) continue;
        directories.emplace(entry->d_name);
    }
    fs1.close_dir();

    return true;
}

bool CPciTree::read_uevents()
{
    for(auto &d : directories)
    {
        pcidesc_t desc;
        desc.id = d;
        size_t pos = desc.id.find_first_of(":");
        if(pos != std::string::npos)
        {
            std::string id = desc.id;
            char *tok = strtok(const_cast<char *>(id.c_str()), ":");
            desc.scopeid = tok;
            tok = strtok(nullptr, ":");
            desc.busid = tok;
            tok = strtok(nullptr, ":");
            desc.devid = tok;

//            desc.scopeid = desc.id.substr(0, pos);
//            desc.busid = desc.id.substr(pos+1);
//            printf("%s %s\n", desc.id1.c_str(), desc.id2.c_str());
        }
        std::string fname = basedir + d + "/uevent";
        if(!read_uevent(const_cast<char *>(fname.c_str())))
        {
//            printf("ERROR reading %s\n", fname.c_str());
        }

        uevent_t &ev = curruev;
        decode(ev, desc);
        descs.push_back(desc);
    }

    build_pci_tree();

    return true;
}

bool CPciTree::read_uevent(char *path)
{
    filectl_t file;

    file.fp = fopen(path, "r");
    if(!file.fp) return false;

    char *tnext = nullptr;
    char *tok = nullptr;

    curruev.driver.value = "";
    curruev.sys.value = "";
    curruev.subsys.value = "";

    short mask[4] = { 0, 0, 0, 0 };
    int masksum = 0;
    ssize_t linsz = 0;
    while(true)
    {
        masksum = mask[0] + mask[1] + mask[2] + mask[3];
        if(masksum == 4)                    break;

        tok = tnext = nullptr;

        linsz = file.readline();
        if(linsz < 0)                       break;

        if(file.pbuffer[0] == '#')          continue;

        tok = strtok_r(file.pbuffer, "\n", &tnext);
        if(!tok)                            continue;

        test_uevent_entry(tok, curruev, mask, 4);

    }
    fclose(file.fp);
    if(mask[1])
    {
        std::string temp = "000000" + curruev.pciclass.value;
        ulong r = temp.size() - 6;
        curruev.pciclass.value = temp.substr(r);
    }
    if(masksum < 4) return false;
    return true;
}

bool CPciTree::test_uevent_entry(char *str, uevent_t &uevent, short *mask, uint masksz)
{
    strvalues_t *p[4] = { &uevent.driver, &uevent.pciclass, &uevent.sys, &uevent.subsys };
    if(masksz < 4) return false;
    char *tok = nullptr;

    tok = strtok(str, "=");
    if(!tok) return false;

    for(uint i = 0; i < 4; i++)
    {
        if(!mask[i])
        {
            if(strcmp(tok, p[i]->key.c_str()) == 0)
            {
                tok = strtok(nullptr, "\n");
                p[i]->value = tok;
                mask[i] = 1;
                return true;
            }
        }
    }
    return true;
}

bool CPciTree::decode(CPciTree::uevent_t &ev, pcidesc_t &desc)
{
    filectl_t file;

    file.fp = fopen(descdb.c_str(), "r");
    if(!file.fp) return false;

    char *tnext = nullptr;
    char *tok = nullptr;
    std:: string t = ev.sys.value;
    std::string vendor = strtok(const_cast<char *>(t.c_str()), ":");
    std::string dev = strtok(nullptr, "\n");

    while(true)
    {
        tok = nullptr;
        tnext = nullptr;

        ssize_t sz = file.readline();
        if(sz <= 0)                                  break;
        if(file.pbuffer[0] == '#')                   continue;

        if(desc.vendor.size() == 0) get_desc_from_key(file.pbuffer, vendor.c_str(), 0, desc.vendor);
        else if(desc.device.size() == 0) get_desc_from_key(file.pbuffer, dev.c_str(), 1, desc.device);
        else if(desc.pciclass.size() == 0) desc.pciclass = search_class(ev.pciclass.value, file);
        else break;
    }
    desc.driver = ev.driver.value;

    fclose(file.fp);
    return tok;
}

bool CPciTree::get_desc_from_key(const char *buffer, const char *key, const int tabs, std::string &desc)
{
    std::string line = buffer;
    std::string tkey = key;
    char *tnext = nullptr;

    char *tmp = const_cast<char *>(tkey.c_str());
    int i = 0;
    while(tmp[i] != 0x00)
    {
        tmp[i] = char(tolower(tmp[i]));
        i++;
    }


    char *tok = strtok_r(const_cast<char *>(line.c_str()), " ", &tnext);

    for(i = 0; i < tabs; i++)
    {
        tok = strtok(tok, "\t");
        if(!tok) return false;
    }

    if(strcmp(tok, tkey.c_str()) == 0)
    {
        tok = strtok_r(nullptr, "\n", &tnext);
        if(tok)
        {
            if(*tok == ' ') tok++;
        }
        desc = tok;
        return true;
    }
    return false;
}

std::string CPciTree::search_class(std::string &classid, filectl_t &file)
{
    if(file.buffer[0] != 'C')           return "";

    std::string tclass = classid;

    char *ltemp = const_cast<char *>(tclass.c_str());

    while(*ltemp)
    {
        *ltemp = char(tolower(*ltemp));
        ltemp++;
    }


    std::string cc = tclass.substr(0, 2);
    std::string cs = tclass.substr(2, 2);

    std::string temp = file.buffer;
    std::string desc = "";

    char *tok = strtok(const_cast<char *>(temp.c_str()), " ");
    if(!tok)                            return "";

    tok = strtok(nullptr, " ");
    if(!tok)                            return "";

    if(strcmp(tok, cc.c_str()) != 0)    return "";

    tok = strtok(nullptr, "\n");
    desc = tok;

    ssize_t sz = file.readline();
    while(true)
    {
        if(sz < 0)                          return desc;
        if(file.buffer[0] != '\t')          return desc;

        temp = file.pbuffer+1;
        tok = strtok(const_cast<char *>(temp.c_str()), " ");
        if(!tok)                            return desc;

        if(strcmp(tok, cs.c_str()) == 0)
        {
            tok = strtok(nullptr, "\n");
            if(tok[0] == ' ') tok++;
            return tok;
        }
        sz = file.readline();
    }
}

void CPciTree::build_pci_tree()
{
    treerec_t dummy;
    for(size_t i = 0; i < descs.size(); i++)
    {
        dummy.dev = int(i);
        tree.push_back(dummy);
    }

    for(auto &t : tree)
    {
        t.con1 = std::atoi(descs[uint(t.dev)].busid.c_str());
    }

    treerec_t rec0;


    for(auto &d : tree)
    {
        std::string path = basedir + descs[size_t(d.dev)].id;
//        printf("%s\n", path.c_str());

        if(!fs1.open_dir(path.c_str())) break;
        while(true)
        {
            entry = fs1.get_next_entry();
            if(!entry) break;
            if(entry->d_type == 0x08) continue;
            if(strcmp(entry->d_name, ".") == 0) continue;
            if(strcmp(entry->d_name, "..") == 0) continue;
            if(entry->d_type == 0x04)
            {
                if(entry->d_name[0] > '9') continue;
                for(auto &d2 : tree)
                {
                    if(descs[size_t(d2.dev)].id == entry->d_name)
                    {
                        d2.root = false;
                        d.childs.push_back(d2);
                        d.con2 = d2.con1;
                    }
                }
            }
        }
        fs1.close_dir();
    }
    return;
}

void CPciTree::show_rec(size_t idx)
{
    pcidesc_t &desc = descs[idx];
    printf("<%s> \033[32m%s\033[39m (%s) [\033[1m\033[32m%s\033[0m\033[39m] [%s]\n",
           desc.busid.c_str(),
           desc.vendor.c_str(),
           desc.device.c_str(),
           desc.pciclass.c_str(),
           desc.driver.c_str());
}
