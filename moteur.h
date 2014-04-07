#ifndef MOTEUR_H
#define MOTEUR_H

//for debug messages about bounces
//#define DEBUG

const double tableLength=700;
const double netHeight=50;
//double xScreen=100;
//double yScreen=100;

const double tableBounceEfficiency=0.9;
const double tableGrip=0.121;
const double racketBounceEfficiency=0.9;
const double racketGrip=10;
const double drag=0.4;//s^-1
const double rotDrag=0.1;//s^-1
const double g=1500;//pixel.s^-2
const double magnus=25e-10;

const double ballRadius=12;//pixel


const int updateCycles=100;

const int memSize=100;
const int pointNumber=4;
const int ballNumber=1;


struct Point{
public:
  Point* projected(Point*a,Point*b);
  double x;
  double y;
  Point(double x0=0,double y0=0){
    x=x0;
    y=y0;
  }
};

extern Point *tableCenter;
extern Point *tableEnd1;
extern Point *tableEnd2;
extern Point *netSummit;


/*beginLeft is a boolean giving the ball to the left player*/
void init();
void moteurResize(int x,int y);

void setBall(float x,float y);

/*called at time t(seconds), finger points given by x[] and y[]
  (with fingers by pair: two fingers from hand1, two from hand2 ...)
  result is copied in the buffers *xb and *yb
update MUST be provided zeros when no fingers are present*/
void updateMoteur(bool racket1_ok,bool racket2_ok,double x[pointNumber],double y[pointNumber],double xbuf[ballNumber],double ybuf[ballNumber],double anglebuf[pointNumber]);




#endif /* MOTEUR_H */
