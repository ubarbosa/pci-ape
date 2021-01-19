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


#include "CGObject.h"
#include "CPciTree.h"
#include <string.h>
#include <stdio.h>

CGObject::CGObject()
{

}

void CGObject::draw_bus(cairo_t *cr, uint id)
{
    gobject_t &bus = busses[id];
    cairo_set_source_rgba(cr, bus.r, bus.g, bus.b, bus.a);
    cairo_rectangle(cr, bus.x, bus.y, bus.w, bus.h);
    cairo_fill(cr);
}

void CGObject::draw_busses(cairo_t *cr)
{

    draw_bus(cr, 0);
    draw_bus_name(cr, 0, false);

    for(uint i = 1; i < busses.size(); i++)
    {
        draw_bus(cr, i);
        draw_bus_name(cr, int(i));
    }
}

void CGObject::draw_bus_connection(cairo_t *cr, bus_t &bus, chip_t &chip)
{
    double x, y, w, h;
    x = chip.h/2 + chip.x + (bus.h/2);
    w = bus.h;

    y = (bus.y < chip.y) ? bus.y + bus.h : chip.y + chip.h;
    h = (bus.y > chip.y) ? bus.y - (chip.y + chip.h) : chip.y - (bus.y + bus.h);

    cairo_rectangle(cr, x, y, w, h);
}

void CGObject::draw_bus_chips(cairo_t *cr, int busid, CPciTree &tree)
{
    cairo_set_source_rgba(cr, 0.4, 0.4, 0.4, 1);

    auto &bus = busses[size_t(busid)];

    // Draw dice
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        auto &chip = chips[size_t(bus.chips[i])];
        cairo_rectangle(cr, chip.x, chip.y, chip.w, chip.h);
    }
    cairo_fill(cr);


    cairo_set_line_width(cr, 2);
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        auto &chip = chips[size_t(bus.chips[i])];

        cairo_set_source_rgba(cr, 1, 1, 1, 0.2);
        cairo_move_to(cr, chip.x, chip.y + chip.h);
        cairo_line_to(cr, chip.x, chip.y);
        cairo_line_to(cr, chip.x + chip.w, chip.y);
        cairo_stroke(cr);

        cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 0.6);
        cairo_move_to(cr, chip.x + chip.w, chip.y);
        cairo_line_to(cr, chip.x + chip.w, chip.y + chip.h);
        cairo_line_to(cr, chip.x, chip.y + chip.h);
        cairo_stroke(cr);
    }



    // Draw connections
    cairo_set_source_rgba(cr,bus.r, bus.g, bus.b, bus.a);
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        auto &chip = chips[size_t(bus.chips[i])];
        draw_bus_connection(cr, bus, chip);
    }
    cairo_fill(cr);


    // Draw chips class
    cairo_select_font_face(cr, "Sans",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 12);
    cairo_set_source_rgba(cr, 0.8, 0.8, 1, 1);
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        auto &chip = chips[size_t(bus.chips[i])];
        draw_bus_chip_class(cr, chip, tree);
    }

    // Draw chips device driver
    cairo_set_source_rgba(cr, 1, 0.8, 0.2, 1);
    cairo_set_font_size(cr, 10);
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        auto &chip = chips[size_t(bus.chips[i])];
        draw_bus_chip_driver(cr, chip, tree);
    }

    // Draw chips device vendor
    cairo_select_font_face(cr, "Sans",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 12);
    cairo_set_source_rgba(cr, 1, 0.8, 0, 1);
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        auto &chip = chips[size_t(bus.chips[i])];
        draw_bus_chip_vendor(cr, chip, tree);

    }
}

void CGObject::draw_bus_chip_class(cairo_t *cr, chip_t &chip, CPciTree &tree)
{
    char del[2] = {0x01, 0x00};
    double height = 0;

    cairo_move_to(cr, chip.x + 2, chip.y +12);
    get_suitable_text(cr, tbuffer, tree.descs[uint(chip.id)].pciclass.c_str(), chip.w + 2, &height);
    double ty = chip.y + 12;
    char *tok = strtok(tbuffer, del);
    while(tok)
    {
        cairo_show_text(cr, tok);
        tok = strtok(nullptr, del);
        ty += height + 2;
        cairo_move_to(cr, chip.x + 2, ty);
    }
}

void CGObject::draw_bus_chip_vendor(cairo_t *cr, CGObject::chip_t &chip, CPciTree &tree)
{
    char del[2] = {0x01, 0x00};
    double height = 0;
    cairo_move_to(cr, chip.x + 2, chip.y + chip.h - 16);
    get_suitable_text(cr, tbuffer, tree.descs[uint(chip.id)].vendor.c_str(), chip.w, &height);
    char *tok = strtok(tbuffer, del);
    cairo_show_text(cr, tok);

}

void CGObject::draw_bus_chip_driver(cairo_t *cr, CGObject::chip_t &chip, CPciTree &tree)
{
    cairo_text_extents_t te;
    sprintf(tbuffer, "%s", tree.descs[uint(chip.id)].driver.c_str());
    cairo_text_extents(cr, tbuffer, &te);
    cairo_move_to(cr, chip.x + chip.w - te.width - 2, chip.y + chip.h - 4);
    char *tok = strtok(tbuffer, " ");
    cairo_show_text(cr, tok);
}

void CGObject::draw_bus_name(cairo_t *cr, int busid, bool left)
{
    char buffer[128];
    auto &bus = busses[size_t(busid)];
    double posx = 0;
    cairo_select_font_face(cr, "Sans",
                           CAIRO_FONT_SLANT_ITALIC,
                           CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 12);
    cairo_set_source_rgba(cr, 0, 0.7, 0, 0.5);

    posx = (left) ? bus.x - 50 : bus.x + bus.w + 6;
    cairo_move_to(cr, posx, bus.y + bus.h + 2);
    sprintf(buffer, "Bus %d", busid);
    cairo_show_text(cr, buffer);
}

void CGObject::draw_secondary_bus_connections(cairo_t *cr, int busid)
{
    auto &bus = busses[size_t(busid)];

    cairo_set_source_rgba(cr, bus.r, bus.g, bus.b, bus.a);
    for(size_t i = 0; i < chips.size(); i++)
    {
        auto &chip = chips[i];
        if(chip.conn1 == busid)
        {
            double x, y, w, h;
            x = chip.h/4 + chip.x + (bus.h/2);
            w = bus.h;

            y = (bus.y < chip.y) ? bus.y + bus.h : chip.y + chip.h;
            h = (bus.y > chip.y) ? bus.y - (chip.y + chip.h) : chip.y - (bus.y + bus.h);

            cairo_rectangle(cr, x, y, w, h);
        }
    }
    cairo_fill(cr);
}

void CGObject::draw_selection(cairo_t *cr)
{
    if(selected < 0) return;
    auto &chip = chips[uint(selected)];
    cairo_set_source_rgba(cr, 0.6, 0.8, 0.6, 0.4);
    cairo_rectangle(cr, chip.x, chip.y, chip.w, chip.h);
    cairo_fill(cr);
}

CGObject::chip_t *CGObject::get_chip(int busid, int chipid)
{
    return &chips[uint(busses[uint(busid)].chips[uint(chipid)])];
}

void CGObject::add_bus()
{
    size_t pos = busses.size();
    bus_t bus;
    bus.w = 100;
    bus.h = def_bus_h;
    bus.g = bus.a = 1 - double(pos) * 0.1;
    bus.x = 10;
    bus.y = def_bus0_y + double(pos) * def_bus_spacing;

    busses.push_back(bus);
}

void CGObject::recalc_busses_width()
{
    for(size_t i = 0; i < busses.size(); i++)
    {
        for(auto &c : busses[i].chips)
        {
            if(busses[i].compact)
            {
                chips[size_t(c)].side = (c%2) ? 1 : 0;
            }
            else chips[size_t(c)].side = 1;
        }
        recalc_bus_width(int(i));
    }
}

void CGObject::recalc_bus_width(int busid)
{
    auto &bus = busses[size_t(busid)];
    bus.w = 10;
    for(auto &c : bus.chips)
    {
        size_t idx = size_t(c);
        bus.w += (bus.compact) ? chips[idx].w/2 : chips[idx].w;
        bus.w += 5;
    }
}

void CGObject::recalc_chips_pos(int busid)
{
    auto &bus = busses[size_t(busid)];
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        auto &chip = chips[size_t(bus.chips[i])];
        double disp = (bus.compact) ? chip.w/2 : chip.w;
        disp += 5;

        chip.y = (chip.side) ? bus.y + 20 : bus.y - 20 - chip.h;
        chip.x = bus.x + chip.w/2 + double(i) * disp - chip.w/2;
    }
}

void CGObject::recalc_child_bus_pos(int busid)
{
    auto &bus = busses[size_t(busid)];
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        auto &chip = chips[size_t(bus.chips[i])];
        if(chip.conn1 != -1)
        {
            auto &childbus = busses[size_t(chip.conn1)];
            childbus.x = chip.x;
        }
    }
}

void CGObject::check_bus(int busid)
{
    move_side0_to_side1(busid);
    space_side1(busid);
}

void CGObject::move_side0_to_side1(int busid)
{
    auto &bus = busses[size_t(busid)];
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        size_t cidx = size_t(bus.chips[i]);
        if(test_side0_with_childs(chips[cidx]))
        {
            int candidate = get_free_side1_chip(int(i));
            if(candidate < 0)
            {
                printf("ERROR : chip %d of bus %d can not expand.\n",
                       int(cidx), busid);
            }
            else
            {
                swap_chips_on_bus(busid, candidate, int(cidx));
            }
        }
    }
}

void CGObject::space_side1(int busid)
{
    int spos = -1;
    auto &bus = busses[size_t(busid)];
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        auto &chip = chips[size_t(bus.chips[i])];
        if(chip.side == 1 && chip.conn1 > 0)
        {
            spos = get_suitable_pos(busid, int(i));
            if(spos < 0)
            {
                printf("ERROR : chip %d of bus %d can not expand.\n",
                       int(i), busid);
                return;
            }
            if(spos > int(i))
            {
                swap_chips_on_bus(busid, int(i), spos);
            }
        }
    }
}

void CGObject::swap_chips_on_bus(int busid, int chipid1, int chipid2)
{
    auto &bus = busses[uint(busid)];
    auto &chip1 = bus.chips[uint(chipid1)];
    auto &chip2 = bus.chips[uint(chipid2)];
    int t = chip1;
    chip1 = chip2;
    chip2 = t;
}

bool CGObject::test_side0_with_childs(CGObject::chip_t &chip)
{
    return (chip.side == 0 && chip.conn1 > 0) ? true : false;
}

int CGObject::get_free_side1_chip(int busid)
{
    auto &bus = busses[size_t(busid)];
    for(size_t i = 0; i < bus.chips.size(); i++)
    {
        auto &chip = chips[size_t(bus.chips[i])];
        if(chip.side == 1 && chip.conn1 == -1) return int(i);
    }
    return -1;
}

int CGObject::get_suitable_pos(int busid, int pos)
{
    auto &bus = busses[uint(busid)];

    int cpos = (bus.compact) ? pos/2 : pos;
    int ppos = 0;
    int suitable = 0;

    for(int i = pos - 1; i >= 0; i--)
    {
        ppos = (bus.compact) ? i/2 : i;
        auto &pchip = *get_chip(busid, i);
        // point to previous with child and add childs size
        if(pchip.conn1 > 0 && pchip.side == 1)
        {
            int childsz = int(busses[uint(pchip.conn1)].chips.size());
            suitable = ppos + childsz;

            if(suitable <= cpos) return pos;

            for(int j = pos; j < int(bus.chips.size()); j++)
            {
                cpos = (bus.compact) ? j/2 : j;
                auto &chip2 = chips[size_t(bus.chips[size_t(j)])];

                if(chip2.conn1 == -1 && chip2.side == 1) return j;
            }
        }
    }
    if(suitable > 0) return -1;
    return pos;
}

void CGObject::get_suitable_text(cairo_t *cr, char *dest, const char *src, double size, double *height)
{
    cairo_text_extents_t te;
    sprintf(dest, "%s", src);
    std::string tempstr = dest;
    std::vector<double> toksizes;
    std::vector<long> tokpos;

    char *strstart = const_cast<char *>(tempstr.c_str());
    char *ntok = nullptr;
    char *tok = strtok_r(strstart, " ", &ntok);
    while(tok)
    {
        cairo_text_extents(cr, tok, &te);
        *height = te.height;
        tokpos.push_back(tok - strstart);
        toksizes.push_back(te.width);
        tok = strtok_r(nullptr, " ", &ntok);
    }

    size_t numtokens = toksizes.size();

    if(numtokens < 1) return;

    double accsz = toksizes[0];
    for(size_t i = 1; i < numtokens; i++)
    {
        if(size < (accsz + toksizes[i]))
        {
            if(toksizes[i-1] > size)
            {
                // cant show
            }
            dest[tokpos[i]-1] = 0x01;
            accsz = 0;
        }
        accsz += toksizes[i];
    }
}
