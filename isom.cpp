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

#define NUM_RAND_POINTS 20

#define PI 3.14159

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
int poly_init_mode = 0;
double a;
double b;
double c;
double d;

//the current polygon 
vector<point2d>  poly;
vector<point2d> input_poly;

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
void perform_isom();
void print_commands();


/* ****************************** */
int main(int argc, char** argv) {

  parse_arguments(argc, argv);
  double adbc = a*d - b*c;
  if (adbc < .999999 || adbc > 1.000001) {
    printf("ad - bc must equal 1. ad - bc = %f. Please try again.\n", adbc);
    exit(1);
  }
  print_commands();

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
    if (!isdigit(str[i]) && !(i == 0 && str[i] == 45) && !(str[i] == 46)) {
      return 0;
    }
  }
  return 1;
}


/*
  Reads command line arguments
*/
void parse_arguments(int argc, char* const* argv) { 
  
  bool inputted_a = false;
  bool inputted_b = false;
  bool inputted_c = false;
  bool inputted_d = false;

  char o;
  while ((o = getopt (argc, argv, "a:b:c:d:")) != -1){
    switch(o){
      case 'a':
        if (isNumber(optarg)) {
          a = atof(optarg);
          inputted_a = true;
        }
        else {
          fprintf (stderr, "Option -a requires a numerical argument.\n");
          exit(1);
        }
        break;
      case 'b':
        if (isNumber(optarg)) {
          b = atof(optarg);
          inputted_b = true;
        }
        else {
          fprintf (stderr, "Option -b requires a numerical argument.\n");
          exit(1);
        }
        break;
      case 'c':
        if (isNumber(optarg)) {
          c = atof(optarg);
          inputted_c = true;
        }
        else {
          fprintf (stderr, "Option -c requires a numerical argument.\n");
          exit(1);
        }
        break;
      case 'd':
        if (isNumber(optarg)) {
          d = atof(optarg);
          inputted_d = true;
        }
        else {
          fprintf (stderr, "Option -d requires a numerical argument.\n");
          exit(1);
        }
        break;
      default:
        fprintf(stderr, "Enter -a [real number] -b [real number] -c [real number] -d [real number]\n");
        exit(1);
    }
  }

  if (!inputted_a || !inputted_b || !inputted_c || !inputted_d) {
    fprintf(stderr, "Enter -a [real number] -b [real number] -c [real number] -d [real number]\n");
    exit(1);
  }
  
}

/* Prints all command options */
void print_commands() {
  printf("Command menu:\n");
  printf("Press s to enter add points mode\n");
  printf("Press e to exit add points mode\n");
  printf("Press i to see the isometry\n");
  printf("Press r to revert to the initial input points\n");
  printf("Press c to revert to clear the points\n");
  printf("Press g to generate %d random points\n", NUM_RAND_POINTS);
  printf("Press h to see the help menu\n\n");
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
   glVertex2f(x + ((double)WINDOWSIZE)/2 + r*cos(theta), y + r*sin(theta));
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


/* Performs the user entered isometry on each point */
void perform_isom() {

  // Determine the type of isometry
  double discriminant = pow(a + d, 2);
  if (discriminant > 4) {
    printf("Performing a hyperbolic isometry...\n");
  }
  else if (discriminant < 4) {
    printf("Performing a elliptic isometry...\n");
  }
  else {
    printf("Performing a parabolic isometry...\n");
  }

  // Perform the isometry on each point
  double x;
  double y;
  double magnatude_sqd;
  double denominator;
  for (int i = 0; i < poly.size(); i++) {
    x = poly[i].x;
    y = poly[i].y;
    magnatude_sqd = pow(x, 2) + pow(y, 2);
    denominator = pow(c, 2)*magnatude_sqd + c*d*2*x + pow(d, 2);
    printf("(%.3f, %.3f) --> ", x, y);
    poly[i].x = (a*c*magnatude_sqd + a*d*x + b*c*x + b*d) / denominator;
    poly[i].y = (a*d*y - b*c*y) / denominator;
    printf("(%.3f, %.3f)\n", poly[i].x, poly[i].y);
  }
  glutPostRedisplay();

}


/* Initializes random points */
void generate_random_points() {
  point2d p;
  for (int i = 0; i < NUM_RAND_POINTS; i++) {
    p.x = (rand() % WINDOWSIZE) - (WINDOWSIZE/2);
    p.y = rand() % WINDOWSIZE;
    poly.push_back(p);
    input_poly.push_back(p);
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
    case 'h':
      print_commands();
    case 'q':	
      exit(0);
      break;
    case 's':
      printf("Add points mode is activate - click in the window to add points\n");
      poly_init_mode = 1;
      break;
    case 'e':
      printf("Exited add points mode\n");
      poly_init_mode = 0;
      break;
    case 'c':
      printf("Points cleared\n");
      printf("Add points mode is activate - click in the window to add points\n");
      poly.clear();
      // set a and y to outside the window
      mouse_x=-10;
      mouse_y=-10;
      poly_init_mode = 0;
      break;
    case 'i':
      perform_isom();
      set_color();
      break;
    case 'r':
      printf("Resetting to inital input points\n");
      poly = input_poly;
      glColor3fv(white);
      break;
    case 'g':
      printf("Generating %d random points.\n", NUM_RAND_POINTS);
      generate_random_points();
      glColor3fv(white);
      break;
    default:
      printf("Unrecognized command.\n");
      print_commands();
      return;
  }
  glutPostRedisplay();
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
      point2d p = {mouse_x, mouse_y};
      poly.push_back(p);
      input_poly.push_back(p);
    }
  }
  
  glutPostRedisplay();
}
