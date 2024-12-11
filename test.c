



#include "replot.h"

#define WIDTH 600
#define HEIGHT 600

int main(void) {

    Replot *rplt = Replot_new(WIDTH, HEIGHT);
    //
    Replot_setImage(rplt, "test.png");
    Replot_setRotation(rplt, 45);
    Replot_fillRect(rplt, RPOINT(300, 300), RSIZE(200, 200)); 
    Replot_resetChanges(rplt);
    //
    Replot_setColor(rplt, RCOLOR(125, 50, 50, 255));
    Replot_fillEllipse(rplt, RPOINT(525, 100), 50, 75);
    //
    Replot_setGradient(rplt, 200, 200, RCOLOR(255, 200, 0, 255), RCOLOR(0, 0, 255, 255), RCOLOR(255, 0, 0, 255), RCOLOR(0, 200, 0, 255));
    Replot_fillRect(rplt, RPOINT(125, 475), RSIZE(200, 200)); 
    Replot_delStencil(rplt);
    //
    Replot_setColor(rplt, RCOLOR(155, 200, 255, 255));
    Replot_fillPolygon(rplt, RPOINT(125, 475), 75, 6);
    //
    Replot_setColor(rplt, RCOLOR(50, 100, 100, 255));
    Replot_drawLine1(rplt, RPOINT(50, 50), RPOINT(550, 550), 10, RPOINT(550, 50));
    // 
    Replot_setColor(rplt, RCOLOR(75, 0, 125, 255));
    Replot_fillArc(rplt, RPOINT(450, 450), 50, 45, 270);
    //
    Replot_setColor(rplt, RCOLOR(75, 150, 50, 255));
    Replot_fillTriangle(rplt, RPOINT(100, 100), RPOINT(200, 100), RPOINT(150, 200));
    // 
    Replot_setColor(rplt, RCOLOR(150, 100, 50, 255));
    Replot_printText(rplt, RPOINT(300, 35), 2, "WELCOME!");
    // 
    // 
    char *_path = "output.png";
    Replot_write(rplt, _path);

    return 0;
}

