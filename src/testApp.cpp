#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include "moteur.h"

const double mouseFactor=0.5;

double  x[4]={240,440,640,840};
double  y[4]={0,800,0,800};
GLfloat xMouse,yMouse;

double xScreen,yScreen;


GLdouble xBall[ballNumber],yBall[ballNumber],alphaBall[ballNumber];
double alpha=M_PI/2;
double alphaW=-2.17,alphaX=-2.67,alphaC=-3.17;


void draw() {
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  
  glLineWidth(5);
  glBegin(GL_LINES);
  
   //table
  glColor3f(1.0,1.0,1.0);
  glVertex2f(tableEnd1->x,tableEnd1->y);
  glVertex2f(tableEnd2->x,tableEnd2->y);
  
  glVertex2f(tableCenter->x,tableCenter->y);
  glVertex2f(netSummit->x,netSummit->y);
  
  //raquettes
  glColor3f(1.0,0.2,0);
  
  glVertex2f(x[0],y[0]);
  glVertex2f(x[1],y[1]);
  
  glVertex2f(x[2],y[2]);
  glVertex2f(x[3],y[3]);
  
 
  glEnd();
  
  
  for(int i=0;i<ballNumber;i++){
    
    
    glColor3f(0,1.0,0);
    glLineWidth(2);
    
    if(yBall[i]>yScreen){
      glBegin(GL_LINES);
      glVertex2f(xBall[i],yScreen);
      glVertex2f(xBall[i],yScreen-10);
      glEnd();
    }
    else{
      glPushMatrix();
      glTranslatef(xBall[i],yBall[i],0);
      glRotated(-180/M_PI*alphaBall[i],0,0,-1);
      
      glutWireSphere((GLdouble) ballRadius,10,2);

      glPointSize(5);
      glColor3f(0,0,1.0);
      glBegin(GL_POINTS);   
      glVertex2f(ballRadius-4,0);
      glEnd();

      glPopMatrix();
    }
    
  }
  glFlush();
  
  glutSwapBuffers();
}


void keyPressed(unsigned char key,int x,int y ) {
  switch(key){
    case(' '):
      setBall(300,400);
      break;
    case('-'):
      alpha-=0.1;
      break;
    case ('+'):
      alpha+=0.1;
      break;
    case('W'):
      alphaW=alpha;
      break;
    case('w'):
      alpha=alphaW;
      break;
    case('X'):
      alphaX=alpha;
      break;
    case('x'):
      alpha=alphaX;
      break;
    case('C'):
      alphaC=alpha;
      break;
    case('c'):
      alpha=alphaC;
      break;
  }
}

void mouseMoved( int x, int y) {
    xMouse=xScreen*0.5+mouseFactor*(x-xScreen/2);
    yMouse=yScreen*0.25-mouseFactor*(y-yScreen/2);
}



void reshape (int w, int h)
{
   xScreen=w;
   yScreen=h;
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluOrtho2D (0.0, (GLdouble) w, 0.0, (GLdouble) h);
   
   moteurResize(w,h);
   setBall(w/4,h/2);
}

void update(int useless)
{

  x[0]=xMouse-tableLength/2+50*sin(alpha);
  y[0]=yMouse-50*cos(alpha);
  x[1]=xMouse-tableLength/2-50*sin(alpha);
  y[1]=yMouse+50*cos(alpha);
  
  x[2]=xMouse+tableLength/2-50*sin(alpha);
  y[2]=yMouse-50*cos(alpha);
  x[3]=xMouse+tableLength/2+50*sin(alpha);
  y[3]=yMouse+50*cos(alpha);


  updateMoteur(true,true,x,y,xBall,yBall,alphaBall);

  glutTimerFunc(20,update,0); //to be called again in 20 ms
  
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (1200, 800);
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   glutSetCursor(GLUT_CURSOR_NONE);
   update(0);
   
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
   
   glutDisplayFunc(draw);
   glutReshapeFunc(reshape);
   glutPassiveMotionFunc(mouseMoved);
   glutKeyboardFunc(keyPressed);
   glutMainLoop();
   
   return 0;
}
