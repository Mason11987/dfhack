#ifndef RENDERER_LIGHT_INCLUDED
#define RENDERER_LIGHT_INCLUDED
#include "renderer_opengl.hpp"
#include "Types.h"

struct renderer_light : public renderer_wrap {
private:
    void colorizeTile(int x,int y)
    {
        const int tile = x*(df::global::gps->dimy) + y;
        old_opengl* p=reinterpret_cast<old_opengl*>(parent);
        float *fg = p->fg + tile * 4 * 6;
        float *bg = p->bg + tile * 4 * 6;
        float *tex = p->tex + tile * 2 * 6;
        lightCell light=lightGrid[tile];
        for (int i = 0; i < 6; i++) {
            *(fg++) *= light.r;
            *(fg++) *= light.g;
            *(fg++) *= light.b;
            *(fg++) = 1;

            *(bg++) *= light.r;
            *(bg++) *= light.g;
            *(bg++) *= light.b;
            *(bg++) = 1;
        }
    }
    void reinitLightGrid(int w,int h)
    {
        tthread::lock_guard<tthread::fast_mutex> guard(dataMutex);
        lightGrid.resize(w*h);
    }
    void reinitLightGrid()
    {
        reinitLightGrid(df::global::gps->dimy,df::global::gps->dimx);
    }
public:
    tthread::fast_mutex dataMutex;
    std::vector<lightCell> lightGrid;
    renderer_light(renderer* parent):renderer_wrap(parent)
    {
        reinitLightGrid();
    }
    virtual void update_tile(int32_t x, int32_t y) { 
        renderer_wrap::update_tile(x,y);
        tthread::lock_guard<tthread::fast_mutex> guard(dataMutex);
        colorizeTile(x,y);
    };
    virtual void update_all() { 
        renderer_wrap::update_all();
        tthread::lock_guard<tthread::fast_mutex> guard(dataMutex);
        for (int x = 0; x < df::global::gps->dimx; x++)
            for (int y = 0; y < df::global::gps->dimy; y++)
                colorizeTile(x,y);
    };
    virtual void grid_resize(int32_t w, int32_t h) { 
        renderer_wrap::grid_resize(w,h);
        reinitLightGrid(w,h);
    };
    virtual void resize(int32_t w, int32_t h) {
        renderer_wrap::resize(w,h);
        reinitLightGrid(w,h);
    }
};
class lightingEngine
{
public:
    lightingEngine(renderer_light* target):myRenderer(target){}

    virtual void reinit()=0;
    virtual void calculate()=0;

    virtual void updateWindow()=0;

protected:
    renderer_light* myRenderer;
};
struct lightSource
{
    lightCell power;
    int radius;
    df::coord2d pos;
};
class lightingEngineViewscreen:public lightingEngine
{
public:
    lightingEngineViewscreen(renderer_light* target);

    void reinit();
    void calculate();

    void updateWindow();

private:
    void doOcupancyAndLights();
    void doRay(lightCell power,int cx,int cy,int tx,int ty);
    void doFovs();
    bool lightUpCell(lightCell& power,int dx,int dy,int tx,int ty);
    size_t inline getIndex(int x,int y)
    {
        return x*h+y;
    }
    std::vector<lightCell> lightMap;
    std::vector<lightCell> ocupancy;
    std::vector<lightSource> lights;
    int w,h;
    DFHack::rect2d mapPort;
};
#endif