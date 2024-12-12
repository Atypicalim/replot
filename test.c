



#include "replot.h"

#define WIDTH 600
#define HEIGHT 600

int main(void) {

    Replot *rplt = Replot_new(WIDTH, HEIGHT);
    int leftX = 0;
    int rghtX = WIDTH;
    int topY = 0;
    int btmY = HEIGHT;
    int ctrX = WIDTH / 2;
    int ctrY = HEIGHT / 2;
    //
    Replot_setImage(rplt, "test.png");
    Replot_setRotation(rplt, 45);
    Replot_fillRect(rplt, RPOINT(ctrX, ctrY + 45), RSIZE(150, 150));
    Replot_resetChanges(rplt);
    //
    Replot_setGradient(rplt, 200, 200, RCOLOR(255, 200, 0, 255), RCOLOR(0, 0, 255, 255), RCOLOR(255, 0, 0, 255), RCOLOR(0, 200, 0, 255));
    Replot_fillRect(rplt, RPOINT(ctrX, btmY - 75), RSIZE(200, 100)); 
    Replot_delStencil(rplt);
    //
    Replot_setColor(rplt, RCOLOR(50, 100, 100, 255));
    Replot_drawLine1(rplt, RPOINT(ctrX - 200, topY + 25), RPOINT(ctrX + 200, topY + 25), 10, RPOINT(ctrX, ctrY));
    // 
    Replot_setColor(rplt, RCOLOR(150, 100, 50, 255));
    Replot_printText(rplt, RPOINT(ctrX, topY + 40), 2, "WELCOME!");
    //
    Replot_setColor(rplt, RCOLOR(75, 150, 50, 255));
    Replot_fillTriangle(rplt, RPOINT(ctrX - 75, topY + 75), RPOINT(ctrX, topY + 200), RPOINT(ctrX + 75, topY + 75));
    //
    Replot_setColor(rplt, RCOLOR(255, 200, 100, 255));
    Replot_setRotation(rplt, 17);
    Replot_fillStar(rplt, 5, RPOINT(leftX + 100, btmY - 100), 30, 50);
    Replot_resetChanges(rplt);
    // 
    Replot_setColor(rplt, RCOLOR(75, 0, 125, 255));
    Replot_fillArc(rplt, RPOINT(rghtX - 100, btmY - 100), 65, 45, 360);
    //
    Replot_setColor(rplt, RCOLOR(125, 50, 50, 255));
    Replot_fillEllipse(rplt, RPOINT(ctrX - 200, ctrY), 50, 75);
    //
    Replot_setColor(rplt, RCOLOR(155, 200, 255, 255));
    Replot_fillPolygon(rplt, 6, RPOINT(ctrX + 200, ctrY), 75);
    // 
    // 
    char *_path = "output.png";
    Replot_write(rplt, _path);

    return 0;
}

