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

#include "CGraphics.h"
#include "CPciTree.h"
#include <math.h>

CGraphics::CGraphics()
{

}

void CGraphics::set_labels(CPciTree *tree, int chipid)
{
    char header0[] = "<span foreground=\"#10FF10\" size=\"medium\">ID\t\t\t:<b>";
    char header1[] = "<span foreground=\"#B0B0C0\" size=\"medium\">Type\t\t:<b>";
    char header2[] = "<span foreground=\"#E0C010\" size=\"medium\">Vendor\t\t:<b>";
    char header3[] = "<span foreground=\"#D0D0F0\" size=\"medium\">Chipset\t:<b>";
    char header4[] = "<span foreground=\"#C0C010\" size=\"medium\">Driver\t\t:<b>";
    char footer[] = "</b></span>";
    char blank[] = " ";
    char *pheader = nullptr;
    char *ptext = nullptr;

    ptext = (!tree) ? blank : const_cast<char *>(tree->descs[uint(chipid)].id.c_str());
    pheader = header0;
    sprintf(tempbuffer,"%s %s %s", pheader, ptext, footer);
    gtk_label_set_markup(GTK_LABEL(label0), tempbuffer);

    ptext = (!tree) ? blank : const_cast<char *>(tree->descs[uint(chipid)].pciclass.c_str());
    pheader = header1;
    sprintf(tempbuffer,"%s %s %s", pheader, ptext, footer);
    gtk_label_set_markup(GTK_LABEL(label1), tempbuffer);

    ptext = (!tree) ? blank : const_cast<char *>(tree->descs[uint(chipid)].vendor.c_str());
    pheader = header2;
    sprintf(tempbuffer,"%s %s %s", pheader, ptext, footer);
    gtk_label_set_markup(GTK_LABEL(label2), tempbuffer);

    ptext = (!tree) ? blank : const_cast<char *>(tree->descs[uint(chipid)].device.c_str());
    pheader = header3;
    sprintf(tempbuffer,"%s %s %s", pheader, ptext, footer);
    gtk_label_set_markup(GTK_LABEL(label3), tempbuffer);

    ptext = (!tree) ? blank : const_cast<char *>(tree->descs[uint(chipid)].driver.c_str());
    pheader = header4;
    sprintf(tempbuffer,"%s %s %s", pheader, ptext, footer);
    gtk_label_set_markup(GTK_LABEL(label4), tempbuffer);

    return;
}

void CGraphics::window_build(GtkApplication *gapp)
{
    // (1) CREATING AND SETUP

    // (1.a) Window
    main = gtk_application_window_new(gapp);
    GtkWindow *window = reinterpret_cast<GtkWindow *>(main);
    gtk_window_set_title(window, "PCI architeture viewer");
    gtk_window_set_default_size(window, 600, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 1);

    // (1.b) Vbox
    vbox0 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(vbox2, GTK_ALIGN_START);

    // (1.c) Scrollable
    scrollabe = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_container_set_border_width(GTK_CONTAINER(scrollabe), 1);
//    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollabe),
//                                      GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollabe),
                                      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    // (1.d) Draw area
    darea = gtk_drawing_area_new();
    gtk_widget_add_events(darea, GDK_BUTTON_PRESS_MASK);

    // (1.e) Labels
    label0 = gtk_label_new("ID :");
    label1 = gtk_label_new("Type :");
    label2 = gtk_label_new("Vendor :");
    label3 = gtk_label_new("Chipset :");
    label4 = gtk_label_new("Driver :");
    gtk_widget_set_halign(label0, GTK_ALIGN_START);
    gtk_widget_set_halign(label1, GTK_ALIGN_START);
    gtk_widget_set_halign(label2, GTK_ALIGN_START);
    gtk_widget_set_halign(label3, GTK_ALIGN_START);
    gtk_widget_set_halign(label4, GTK_ALIGN_START);
    set_labels(nullptr, 0);


    // (2) PACKING
    gtk_box_pack_start(GTK_BOX(vbox2), label0, false, false, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), label1, false, false, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), label2, false, false, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), label3, false, false, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), label4, false, false, 0);

    gtk_container_add(GTK_CONTAINER(scrollabe), darea);
    gtk_box_pack_start(GTK_BOX(vbox1), scrollabe, true, true, 0);
    gtk_box_pack_start(GTK_BOX(vbox0), vbox1, true, true, 0);
    gtk_box_pack_start(GTK_BOX(vbox0), vbox2, false, false, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox0);

    gtk_widget_set_size_request(darea, dareaw, dareah);
    gtk_widget_set_size_request(scrollabe, 400, 400);
    gtk_window_set_default_size(window, dareaw+10, dareah+10);
    gtk_widget_show_all(main);
}

gboolean CGraphics::darea_draw(cairo_t *cr, CPciTree &tree)
{
    ct = cr;
    darea_clear();
    actors.draw_busses(cr);
    actors.draw_bus_chips(cr, 0, tree);
    for(size_t i = 1; i < actors.busses.size(); i++)
    {
        actors.recalc_chips_pos(int(i));
        actors.draw_bus_chips(cr, int(i), tree);
        actors.draw_secondary_bus_connections(cr, int(i));
    }
    actors.draw_selection(cr);
    return true;
}


void CGraphics::darea_init(CPciTree &pcis)
{
    CGObject::chip_t chip;
    chip.w = actors.def_chip_w;
    chip.h = actors.def_chip_h;
    for(auto &t : pcis.tree)
    {
        chip.conn0 = t.con1;
        chip.conn1 = t.con2;
        chip.id = t.dev;
        size_t idx = size_t(t.con1);
        if(actors.busses.size() < idx + 1)
        {
            actors.add_bus();
        }
        actors.busses[idx].numchips++;
        actors.chips.push_back(chip);
        actors.busses[size_t(t.con1)].chips.push_back(chip.id);
    }

    actors.busses[0].compact = true;
    actors.recalc_busses_width();
    actors.recalc_chips_pos(0);
    actors.recalc_child_bus_pos(0);
    actors.check_bus(0);

    dareah = gint(actors.busses.size()) * gint(actors.def_bus_spacing) +
            gint(actors.def_chip_w) + 10;
    dareaw = gint(actors.busses[0].w + actors.def_chip_w);

}


gboolean CGraphics::darea_click(CPciTree &tree, double x, double y)
{
    actors.selected = -1;
    for(auto &chip : actors.chips)
    {
        if(x >= chip.x && x <= (chip.x + chip.w))
        {
            if(y >= chip.y && y <= (chip.y + chip.h))
            {
                printf("Clicked on chip %d\n", chip.id);
                actors.selected = chip.id;
                break;
            }
        }
    }
    if(actors.selected < 0)     set_labels(nullptr, -1);
    else                        set_labels(&tree, actors.selected);


    gtk_widget_queue_draw(darea);
    return true;
}
void CGraphics::darea_clear()
{
    cairo_save (ct);
    cairo_set_source_rgba (ct, 0.2, 0.2, 0.2, 1);
    cairo_set_operator (ct, CAIRO_OPERATOR_SOURCE);
    cairo_paint (ct);
    cairo_restore (ct);
}
