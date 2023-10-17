/* isom.cpp
Danielle Simon

The user can click to input a set of points and specify isometries using
keyboard letters which are then computed and displayed.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <getopt.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector> 

using namespace std; 

typedef struct _point2d {
  double x,y;
} point2d;


GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};
GLfloat white[3] = {1.0, 1.0, 1.0};


/* global variables */
const int WINDOWSIZE = 750; 
int poly_init_mode = 1;
double k_x;
double k_y;
double lambda;

//the current polygon 
vector<point2d>  poly;

//coordinates of the last mouse click
double mouse_x=-10, mouse_y=-10;  //initialized to a point outside the window


/* forward declarations of functions */
void parse_arguments(int argc, char* const* argv);
void display(void);
void keypress(unsigned char key, int x, int y);
void mousepress(int button, int state, int x, int y);
void set_color();

void initialize_polygon(); 
void print_polygon(vector<point2d>& poly); 

void reflection();
void rotation();
void hyperbolic_transformation();
void parabolic_transformation();


/* ****************************** */
int main(int argc, char** argv) {

  parse_arguments(argc, argv);

  /* initialize GLUT  */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);

  /* register callback functions */
  glutDisplayFunc(display); 
  glutKeyboardFunc(keypress);
  glutMouseFunc(mousepress); 

  /* init GL */
  /* set background color black*/
  glClearColor(0, 0, 0, 0);   
  
  /* give control to event handler */
  glutMainLoop();
  return 0;
}


/* Helper function for validating port number. 
    returns: 1 if the arg passed in is an integer or 0 if not. */
int isNumber(char* str) {
  for (int i = 0; i < strlen(str); i++) {
    if (!isdigit(str[i]) && !(i == 0 && str[i] == 45)) {
      return 0;
    }
  }
  return 1;
}


/*
  Reads command line arguments
*/
void parse_arguments(int argc, char* const* argv) { 
  
  char c;
  while ((c = getopt (argc, argv, "x:y:l:")) != -1){
    switch(c){
      case 'x':
        if (isNumber(optarg)) {
          k_x = atof(optarg);
        }
        else {
          fprintf (stderr, "Option -x requires a numerical argument.\n");
          exit(1);
        }
        break;
      case 'y':
        if (isNumber(optarg)) {
          k_y = atof(optarg);
        }
        else {
          fprintf (stderr, "Option -y requires a numerical argument.\n");
          exit(1);
        }
        break;
      case 'l':
        if (isNumber(optarg) && atof(optarg) >= 0) {
          lambda = atof(optarg);
        }
        else {
          fprintf (stderr, "Option -l requires a non-negative numerical argument.\n");
          exit(1);
        }
        break;
      default:
        fprintf(stderr, "Enter -x [x value of k] -y [y value of k] -l [lambda]");
        exit(1);
    }
  }
  
}


/* our coordinate system is (0,0) x (WINDOWSIZE,WINDOWSIZE) with the
   origin in the lower left corner 
*/
//draw a circle with center = (x,y)
void draw_circle(double x, double y){

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  

  double r = 5;
  glBegin(GL_POLYGON);
  for(double theta = 0; theta < 2*M_PI; theta+=.3){
   glVertex2f(x + + ((double)WINDOWSIZE)/2 + r*cos(theta), y + r*sin(theta));
  }
  glEnd();
}


/* **************************************** */
/* Sets the glColor to the guard's color
 */
void set_color() {

  int rand_num = rand() % 8;

  if (rand_num == 0) {
    glColor3fv(red);
  }
  else if (rand_num == 1) {
    glColor3fv(green);
  }
  else if (rand_num == 2) {
    glColor3fv(blue);
  }
  else if (rand_num == 3) {
    glColor3fv(yellow);
  }
  else if (rand_num == 5) {
    glColor3fv(magenta);
  }
  else if (rand_num == 6) {
    glColor3fv(cyan);
  } 
  else {
    glColor3fv(blue);
  }
}


/* ****************************** */
/* This is the main render function. We registered this function to be
   called by GL to render the window. 
 */
void display(void) {

  glClear(GL_COLOR_BUFFER_BIT);
  //clear all modeling transformations 
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity();

  /* The default GL window is [-1,1]x[-1,1] with the origin in the
     center.  The camera is at (0,0,0) looking down negative
     z-axis.  

     The points are in the range (0,0) to (WINSIZE,WINSIZE), so they
     need to be mapped to [-1,1]x [-1,1] */
  
  //First we scale down to [0,2] x [0,2] */ 
  glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);  
  /* Then we translate so the local origin goes in the middle of teh
     window to (-WINDOWSIZE/2, -WINDOWSIZE/2) */
  glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0); 
  
  // draw y axis
  glBegin(GL_LINES);
  glVertex2f(((double)WINDOWSIZE)/2, 0); 
  glVertex2f(((double)WINDOWSIZE)/2, WINDOWSIZE);
  glEnd();

  // display all points 
  for (int i = 0; i < poly.size(); i++) {
    draw_circle(poly[i].x, poly[i].y);
  }

  /* execute the drawing commands */
  glFlush();
}


/* ****************************** */
void keypress(unsigned char key, int x, int y) {
  switch(key) {
    case 'q':	
      exit(0);
      break;
    case 'c':
      poly.clear();
      // set a and y to outside the window
      mouse_x=-10;
      mouse_y=-10;
      break;
    case 'r':
      poly_init_mode = 0;
      reflection();
      poly_init_mode = 1;
      break;
    case 't':
      poly_init_mode = 0;
      rotation();
      poly_init_mode = 1;
      break;
    case 'h':
      poly_init_mode = 0;
      hyperbolic_transformation();
      poly_init_mode = 1;
      break;
    case 'p':
      poly_init_mode = 0;
      parabolic_transformation();
      poly_init_mode = 1;
      break;
    default:
      printf("Unrecognized command.\n Press 'r' to see a reflection\n Press 't' to see a rotation\n Press 'h' to see a hyperbolic transformation\n Press 'p' to see a parabolic transformation\n"); // list instructions
      return;
  }
  set_color();
  glutPostRedisplay();
}

void reflection() {
  printf("performing reflection...\n");
  return;
}

void rotation() {
  printf("performing rotation...\n");
  return;
}

void hyperbolic_transformation() {
  printf("performing hyperbolic transformation...\n");
  return;
}

void parabolic_transformation() {
  printf("performing parabolic transformation...\n");
  return;
}


/* 
void glutMouseFunc(void (*func)(int button, int state, int x, int y));

glutMouseFunc sets the mouse callback for the current window. When a
user presses and releases mouse buttons in the window, each press and
each release generates a mouse callback. The button parameter is one
of GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON. For
systems with only two mouse buttons, it may not be possible to
generate GLUT_MIDDLE_BUTTON callback. For systems with a single mouse
button, it may be possible to generate only a GLUT_LEFT_BUTTON
callback. The state parameter is either GLUT_UP or GLUT_DOWN
indicating whether the callback was due to a release or press
respectively. The x and y callback parameters indicate the window
relative coordinates when the mouse button state changed. If a
GLUT_DOWN callback for a specific button is triggered, the program can
assume a GLUT_UP callback for the same button will be generated
(assuming the window still has a mouse callback registered) when the
mouse button is released even if the mouse has moved outside the
window.
*/
void mousepress(int button, int state, int x, int y) {

  if(state == GLUT_DOWN) { //mouse click detected 

    //(x,y) are in window coordinates, where the origin is in the upper
    //left corner; our reference system has the origin in lower left
    //corner, this means we have to reflect y
    //must also shift left to allow negative x values
    mouse_x = (double)x - ((double)WINDOWSIZE)/2;
    mouse_y = (double)(WINDOWSIZE - y);  

    if (poly_init_mode) {
      printf("mouse pressed at (%.1f,%.1f)\n", mouse_x, mouse_y);
      point2d p = {mouse_x, mouse_y};
      poly.push_back(p);
    }
  }
  
  glutPostRedisplay();
}
