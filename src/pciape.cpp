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
#include "CGraphics.h"
#include <string.h>
#include <set>

struct resources_t
{
    CPciTree *tree {nullptr};
    CGraphics *scr {nullptr};
};

void show_rec(size_t idx, CPciTree &tree)
{
    CPciTree::pcidesc_t &desc = tree.descs[idx];
    printf("<%s:%s> \033[32m%s\033[39m (%s) [\033[1m\033[32m%s\033[0m\033[39m] [%s]\n",
           desc.busid.c_str(),
           desc.devid.c_str(),
           desc.vendor.c_str(),
           desc.device.c_str(),
           desc.pciclass.c_str(),
           desc.driver.c_str());
}
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer udata)
{
    resources_t *res = reinterpret_cast<resources_t *>(udata);
    return res->scr->darea_draw(cr, *res->tree);
}

gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer udata)
{
    resources_t *res = reinterpret_cast<resources_t *>(udata);
    return res->scr->darea_click(*res->tree, event->x, event->y);
}
void activate(GtkApplication* app, gpointer udata)
{
    resources_t *res = reinterpret_cast<resources_t *>(udata);
    CGraphics &graphic = *res->scr;

    graphic.window_build(app);
    g_signal_connect(G_OBJECT(graphic.darea), "draw", G_CALLBACK(on_draw), udata);
    g_signal_connect(G_OBJECT(graphic.darea), "button-press-event", G_CALLBACK(clicked), udata);

}

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

int go_text_mode(CPciTree &tree)
{
    for(auto &t : tree.tree)
    {
        if(!t.root) continue;
        show_rec(size_t(t.dev), tree);
        for(auto &c : t.childs)
        {
            printf("   └─");
            show_rec(size_t(c.dev), tree);
        }
    }
    return 0;
}
int main(int argc, char *argv[])
{
    CPciTree tree;
    CGraphics scr;

    resources_t res;
    res.scr = &scr;
    res.tree = &tree;

    if(!tree.read_base_dir()) return -1;
    tree.read_uevents();

    if(argc > 1)
    {
        char *tok = strtok(argv[1], " ");
        if(strcmp(tok, "-t") == 0) return go_text_mode(tree);
        printf("format:\n\tpciape [-t]\n");
    }

    GtkApplication *gapp;

    scr.darea_init(tree);

    gapp = gtk_application_new("org.gtk.pci_viewer", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(gapp, "activate", G_CALLBACK(activate), &res);
    int status = g_application_run(G_APPLICATION(gapp), argc, argv);

    g_object_unref(gapp);

    return status;
}

