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

#include "CFsProvider.h"
#include <dirent.h>
CFsProvider::CFsProvider()
{

}

bool CFsProvider::open_dir(const char *path)
{
    pdir = opendir(path);
    if(!pdir) return false;
    currdir = path;
    return true;
}

void CFsProvider::close_dir()
{
    currdir = "";
    if(!pdir) return;
    closedir(pdir);
    pdir = nullptr;
}

dirent *CFsProvider::get_next_entry()
{
    if(!pdir) return nullptr;
    pentry = readdir(pdir);
    if(!pentry) close_dir();
    return pentry;
}
