#pragma once



struct ez_point {
    double x;
    double y;
};

struct ez_edge {
    ez_point source;
    ez_point target;
};

typedef  enum {TOP, BOTTOM, LEFT, RIGHT} TYPE;

struct ElementInfo
{
    int color;
    TYPE type;
    bool reflex = false;
};