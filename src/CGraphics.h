#ifndef CGRAPHICS_H
#define CGRAPHICS_H
#include <gtk/gtk.h>
#include "CGObject.h"

class CPciTree;

class CGraphics
{
public:
    GtkWidget   *main {nullptr};
    GtkWidget   *scrollabe {nullptr};
    GtkWidget   *darea {nullptr};
    GtkWidget   *vbox0  {nullptr};
    GtkWidget   *vbox1  {nullptr};
    GtkWidget   *vbox2 {nullptr};
    GtkWidget   *label0 {nullptr};
    GtkWidget   *label1 {nullptr};
    GtkWidget   *label2 {nullptr};
    GtkWidget   *label3 {nullptr};
    GtkWidget   *label4 {nullptr};
    cairo_t     *ct    {nullptr};
public:
    char tempbuffer[256];
    gint dareaw {0}, dareah {0};
    CGObject actors;

public:
    CPciTree *data {nullptr};
public:
    CGraphics();
public:
    void window_build(GtkApplication *gapp);
public:
    void darea_clear();
    void darea_init(CPciTree &pcis);
    gboolean darea_click(CPciTree &tree, double x, double y);
    gboolean darea_draw(cairo_t *cr, CPciTree &tree);
    void set_labels(CPciTree *tree, int chipid);
};

#endif // CGRAPHICS_H
