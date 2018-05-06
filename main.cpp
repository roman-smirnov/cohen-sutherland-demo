/*
 * Cohen-Sutherland Algorithm Demonstration
 */

/* import OpenGL, and some std libs */
#include <iostream>
#include <tuple>
#include <GLUT/glut.h>/**/

/* note to self: this is basically like a static import of functions like cout, cin, etc... */
using namespace std;

/* default window dimensions */
static const GLsizei defaultWindowWidth = 1920;
static const GLsizei defaultWindowHeight = 1080;

static GLsizei viewPortWidth = defaultWindowWidth;
static GLsizei viewPortHeight = defaultWindowHeight;

/* window title text */
static const char *windowTitle = "Cohen Sutherland Algorithm Demonstration";

/* region boundary lines */
static const GLsizei TOP_Y_BOUND = 720;
static const GLsizei RIGHT_X_BOUND = 1280;
static const GLsizei BOTTOM_Y_BOUND = 360;
static const GLsizei LEFT_X_BOUND = 640;


/* region codes */
static const GLubyte CENTER_CODE = 0x0;
static const GLubyte TOP_CODE = 0x1;
static const GLubyte RIGHT_CODE = 0x2;
static const GLubyte BOTTOM_CODE = 0x4;
static const GLubyte LEFT_CODE = 0x8;

/* region names */
static const unsigned char CENTER_TEXT[] = "CENTER";
static const unsigned char TOP_TEXT[] = "TOP";
static const unsigned char RIGHT_TEXT[] = "RIGHT";
static const unsigned char BOTTOM_TEXT[] = "BOTTOM";
static const unsigned char LEFT_TEXT[] = "LEFT";

/* starting position of input line */
static tuple<GLint, GLint> lineSrc;
/* terminal position of input line */
static tuple<GLint, GLint> lineDest;

/* max units font height */
static const GLfloat GLUT_STROKE_ROMAN_HEIGHT = 119.05f;


/* draws region boundary lines */
void drawDividers() {
    GLint vertDiv1 = defaultWindowWidth / 3;
    GLint vertDiv2 = vertDiv1 * 2;
    GLint horizDiv1 = defaultWindowHeight / 3;
    GLint horizDiv2 = horizDiv1 * 2;

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);
    glVertex2i(vertDiv1, 0);
    glVertex2i(vertDiv1, defaultWindowHeight);

    glVertex2i(vertDiv2, 0);
    glVertex2i(vertDiv2, defaultWindowHeight);

    glVertex2i(0, horizDiv1);
    glVertex2i(defaultWindowWidth, horizDiv1);

    glVertex2i(0, horizDiv2);
    glVertex2i(defaultWindowWidth, horizDiv2);

    glEnd();
}

/* draws given text such that it's both horizontally and vertically centered at given position */
void drawTextAtPos(unsigned const char *label, const int posX, const int posY) {
    glColor3f(1.0f, 1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    GLfloat translateX = (float) (posX) - ((float) glutStrokeLength(GLUT_STROKE_ROMAN, label)) / 2.0f;
    GLfloat translateY = (float) (posY) - GLUT_STROKE_ROMAN_HEIGHT / 2.0f;

    glTranslatef(translateX, translateY, 0.0f);

    for (auto *c = label; *c != '\0'; c++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }

    glPopMatrix();
}

/* draws text labels at center of each region */
void drawRegionLabels() {
    drawTextAtPos(CENTER_TEXT, defaultWindowWidth / 2, defaultWindowHeight / 2);
    drawTextAtPos(TOP_TEXT, defaultWindowWidth / 2, defaultWindowHeight * 5 / 6);
    drawTextAtPos(RIGHT_TEXT, defaultWindowWidth * 5 / 6, defaultWindowHeight / 2);
    drawTextAtPos(BOTTOM_TEXT, defaultWindowWidth / 2, defaultWindowHeight / 6);
    drawTextAtPos(LEFT_TEXT, defaultWindowWidth / 6, defaultWindowHeight / 2);
}


/* returns the binary region code for a given point */
GLubyte encode(tuple<int, int> pt) {

    GLubyte code = 0x00;

    if (get<1>(pt) >= TOP_Y_BOUND) {
        code = code | TOP_CODE;
    }
    if (get<0>(pt) >= RIGHT_X_BOUND) {
        code = code | RIGHT_CODE;
    }
    if (get<1>(pt) <= BOTTOM_Y_BOUND) {
        code = code | BOTTOM_CODE;
    }
    if (get<0>(pt) <= LEFT_X_BOUND) {
        code = code | LEFT_CODE;
    }
    return code;
}

/* returns  true if code is isInside clipping window */
bool isInside(GLubyte code) {
    return code==CENTER_CODE;
}

/* returns true if line between points passes through center*/
bool isOutside(GLubyte code1, GLubyte code2) {
    return (code1 & code2) != 0;
}

/* returns true if line is vertical or both points are the same */
bool isVertical(tuple<int, int> pt1, tuple<int, int> pt2) {
    return get<0>(pt1) == get<0>(pt2);
}

/* returns the slope of line segment between two points*/
/* NOTE: don't give it vertical lines! */
GLfloat calcSlope(tuple<int, int> pt1, tuple<int, int> pt2) {
    return (float) (get<1>(pt1) - get<1>(pt2)) / (float) (get<0>(pt1) - get<0>(pt2));
}

/* returns the offset part of the line formula */
GLfloat calcOffset(tuple<int, int> pt1, GLfloat slope) {
    return (float) get<1>(pt1) - slope * (float) get<0>(pt1);
}

/* returns the y coordinate of the intersection of the given line with a vertical line given by parameter x */
GLdouble calcYintersect(GLfloat m, GLfloat b, int x) {
    return (double) m * (double) x + (double) b;
}

/* returns the x coordinate of the intersection of the given line with a horizontal line given by parameter y */
GLdouble calcXintersect(GLfloat m, GLfloat b, int y) {
    return ((double) y - (double)b) / (double) m;
}

/* returns clipped line edge point */
tuple<int,int> clipPoint(tuple<int, int> pt, GLubyte code, GLfloat m, GLfloat b) {

    if (isInside(code)) {
        /* do nothing */
    }else if (code & TOP_CODE) {
        get<0>(pt) = calcXintersect(m, b, TOP_Y_BOUND-1);
        get<1>(pt) = TOP_Y_BOUND-1;

    } else if (code & RIGHT_CODE) {
        get<0>(pt) = RIGHT_X_BOUND-1;
        get<1>(pt) = calcYintersect(m, b, RIGHT_X_BOUND)-1;

    } else if (code & BOTTOM_CODE) {
        get<0>(pt) = calcXintersect(m, b, BOTTOM_Y_BOUND)+1;
        get<1>(pt) = BOTTOM_Y_BOUND+1;

    } else if(code & LEFT_CODE) {
        get<0>(pt) = LEFT_X_BOUND+1;
        get<1>(pt) = calcYintersect(m, b, LEFT_X_BOUND)+1;
    }

    return pt;
}

/* returns true if line was clipped, false otherwise */
bool clipLine(tuple<int, int> pt1, tuple<int, int> pt2) {
    GLubyte code1, code2;
    GLfloat slope, offset;

    /* iterate until the lines is either wholly outside or inside */
    while (true) {

        code1 = encode(pt1);
        code2 = encode(pt2);

        if (isInside(code1) and isInside(code2)) {
            /* both are inside - update lineSrc and lineDest and return */
            lineSrc = pt1;
            lineDest = pt2;
            return true;
        }

        if (isOutside(code1, code2)) {
            /* both are outside and don't pass through the center */
            return false;
        }

        /* can't calc slope for vertical line (div by zero) */
        if (!isVertical(pt1, pt2)) {
            slope = calcSlope(pt1, pt2);
            offset = calcOffset(pt1, slope);
        }else{

        }

        /* clip edge points */
        pt1 = clipPoint(pt1, code1, slope, offset);
        pt2 = clipPoint(pt2, code2, slope, offset);

    }

}

/* draw regilar lines before clipping */
void drawLines() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2i(get<0>(lineSrc), get<1>(lineSrc));
    glVertex2i(get<0>(lineDest), get<1>(lineDest));
    glEnd();
    glFlush();
}

/* draw clipped lines bounded inside clipping window */
void drawClipedLines() {
    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2i(get<0>(lineSrc), get<1>(lineSrc));
    glVertex2i(get<0>(lineDest), get<1>(lineDest));
    glEnd();
    glFlush();
}

/* the main display function */
void display() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    /* clear the color buffer with clearing color */
    glClear(GL_COLOR_BUFFER_BIT);

    /* draw lines dividing the regions*/
    drawDividers();

    /* draw labels on each region position */
    drawRegionLabels();

    /* draw immediately*/
    glFlush();
}


/* handle window resize and rehsape */
void handleResize(GLint newWidth, GLint newHeight) {

//     TODO move this to init
    /* calculate the aspect ratio */
    GLfloat aspectRatio = (float) defaultWindowWidth / (float) defaultWindowHeight;

    /* calculate the current window aspect ratio */
    GLfloat windowAspectRatio = (float) newWidth / (float) newHeight;

    /* resize the viewport height with respect to the aspect ratio */
    if (windowAspectRatio > aspectRatio) {
        newWidth = (GLsizei) ((GLfloat) newHeight * aspectRatio);
    } else {
        newHeight = (GLsizei) ((GLfloat) (newWidth) / aspectRatio);
    }

    viewPortWidth = newWidth;
    viewPortHeight = newHeight;

    /* set the new viewport dimensions */
    glViewport(0, 0, newWidth, newHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /* set the coordinate system */
    gluOrtho2D(0.0, (GLdouble) defaultWindowWidth, 0.0, (GLdouble) defaultWindowHeight);
    glMatrixMode(GL_MODELVIEW);
}

/* calculate the position to place the window such that it's centered vertically */
int getWindowCenterPlacement() { return ((glutGet(GLUT_SCREEN_HEIGHT) - glutGet(GLUT_WINDOW_WIDTH)) / 2); }

/* calculate the position to place the window such that it's centered horizontally */
int getWindowWidthCenter() { return (glutGet(GLUT_SCREEN_WIDTH) - glutGet(GLUT_WINDOW_WIDTH) / 2); }

/* convert window position to world coordinates */
tuple<GLint, GLint> toWorldCoords(int x, int y) {
    auto worldX = (GLint) ((float) x * (float) defaultWindowWidth / (float) viewPortWidth);
    auto worldY = (GLint) ((float) (glutGet(GLUT_WINDOW_HEIGHT) - y) * (float) defaultWindowHeight /
                           (float) viewPortHeight);
    return make_tuple(worldX, worldY);
}

/* handle mouse click events */
void handleMouse(int button, int action, int x, int y) {

    /* don't do anything if clicked outside of viewport */
    if (x > viewPortWidth or glutGet(GLUT_WINDOW_HEIGHT) - y > viewPortHeight) {
        return;
    }

    if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN) {
        lineSrc = toWorldCoords(x, y);
    } else if (button == GLUT_LEFT_BUTTON && action == GLUT_UP) {
        lineDest = toWorldCoords(x, y);
        drawLines();
        if (clipLine(lineSrc, lineDest)) {
            drawClipedLines();
        }
    } else {
        /* do nothing */
        return;
    }
}

/* main function */
int main(int argc, char *argv[]) {
//    TODO: move to init
    lineSrc = make_tuple(0, 0);
    lineDest = make_tuple(0, 0);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    /* initially, the window will fill the screen */
    glutInitWindowSize(defaultWindowWidth, defaultWindowHeight);
    /* set the initial window position at center of screen*/
    glutInitWindowPosition(getWindowWidthCenter(), getWindowCenterPlacement());
    glutCreateWindow(windowTitle);
    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutMouseFunc(handleMouse);
    glutMainLoop();
    return 0;
}
