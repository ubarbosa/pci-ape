#ifndef CGOBJECT_H
#define CGOBJECT_H
#include <gtk/gtk.h>
#include<vector>

class CPciTree;

class CGObject
{
public:
    struct gobject_t
    {
        double x {0}, y{0};
        double w {0}, h {0};
        double r{0}, g{0}, b{0}, a{0};
    };

    struct chip_t : public gobject_t
    {
        int id {-1};
        int side {1};
        int conn0{-1}, conn1{-1};
    };

    struct bus_t : public gobject_t
    {
        bool compact {false};
        int numchips {0};
        std::vector<int> chips;
    };

public:
    double def_bus_h = 4;
    double def_chip_h = 80;
    double def_chip_w = 100;
    double def_bus_spacing = 140;
    double def_bus0_y = 120;

    double def_chip_color[4] = {0.2, 0.2, 0.2, 1};

public:
    char tbuffer[512];
    std::vector<chip_t> chips;
    std::vector<bus_t> busses;
    int selected {-1};
public:
    CGObject();
    void draw_busses(cairo_t *cr);
    void draw_bus(cairo_t *cr, uint id);
    void draw_bus_chips(cairo_t *cr, int busid, CPciTree &tree);
    void draw_bus_chip_class(cairo_t *cr, chip_t &chip, CPciTree &tree);
    void draw_bus_chip_vendor(cairo_t *cr, chip_t &chip, CPciTree &tree);
    void draw_bus_chip_driver(cairo_t *cr, chip_t &chip, CPciTree &tree);
    void draw_bus_name(cairo_t *cr, int busid, bool left=true);
    void draw_bus_connection(cairo_t *cr, bus_t &bus, chip_t &chip);
    void draw_secondary_bus_connections(cairo_t *cr, int busid);
    void draw_selection(cairo_t *cr);
public:
    chip_t *get_chip(int busid, int chipid);
public:
    void add_bus();
    void recalc_busses_width();
    void recalc_bus_width(int busid);
    void recalc_chips_pos(int busid);
    void recalc_child_bus_pos(int busid);
public:
    void check_bus(int busid);
    void move_side0_to_side1(int busid);
    void space_side1(int busid);
    void swap_chips_on_bus(int busid, int chipid1, int chipid2);
    bool test_side0_with_childs(chip_t &chip);
    int get_free_side1_chip(int busid);
    int get_suitable_pos(int busid, int pos);
    void get_suitable_text(cairo_t *cr, char *dest, const char *src, double size, double *height);

};

#endif // CGOBJECT_H
