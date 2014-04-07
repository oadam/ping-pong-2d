#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "moteur.h"

const double dt=0.02/updateCycles;
const double sqRadius=(ballRadius+2.5)*(ballRadius+2.5);



typedef Point Vect;

class Ball : public Point{
public:
  Vect v;//pixel.s^-1
  double ang,vang;//rad rad.s^-1

  void set(double x0=300,double y0=0,double vx0=0,double vy0=0,double vang0=0);
  void racketBounce(int);
  void handleBounce(Point*a, Point*b, double bEfficiency, double grip, int racketID=-1);
  void pointBounce(Point*p,double bEfficiency,double grip,int racketID);
  bool collidesLine(Point* a,Point* b);
  bool touches(Point*p);
  void update(bool r1OK,bool r2OK);
};


class MemIndex{
private:
  int i;
public:
  int prev();
  int get();
  int next();
  void inc();
  MemIndex(){
    i=0;
  }
};


Ball balls[ballNumber];

Point pMem[memSize][pointNumber];
Point* next=new Point;


Point *tableCenter=new Point;
Point *tableEnd1=new Point;
Point *tableEnd2=new Point;
Point *netSummit=new Point;

Point screenEndTL0=Point(0,-10000);
Point *screenEndTL=&screenEndTL0;
Point screenEndTR0=Point(100,-10000);
Point *screenEndTR=&screenEndTR0;
Point screenEndBL0=Point(0,100);
Point *screenEndBL=&screenEndBL0;
Point screenEndBR0=Point(100,100);
Point *screenEndBR=&screenEndBR0;




void moteurResize(int x,int y){
  double xTableCenter=x/2;
  double yTableCenter=3*y/4;
  tableCenter->x=xTableCenter;
  tableCenter->y=yTableCenter;
  tableEnd1->x=xTableCenter-tableLength/2;
  tableEnd1->y=yTableCenter;
  tableEnd2->x=xTableCenter+tableLength/2;
  tableEnd2->y=yTableCenter;
  netSummit->x=xTableCenter;
  netSummit->y=yTableCenter-netHeight;
  
  screenEndTR->x=x;
  screenEndBR->x=x;
  screenEndBR->y=y;
  screenEndBL->y=y;
}


MemIndex m;
double t=0;


double oldx[4];
double oldy[4];

int racket1Enable,racket2Enable;


/* det Vect(a1a2) Vect(b1b2)*/
inline double det(Point* a1,Point* a2,Point* b1,Point* b2){
  return (a2->x-a1->x)*(b2->y-b1->y)-(a2->y-a1->y)*(b2->x-b1->x);
}

inline double scal(Point* a1,Point* a2,Point* b1,Point* b2){
  return (a2->x-a1->x)*(b2->x-b1->x)+(a2->y-a1->y)*(b2->y-b1->y);
}


void init(){
  setBall(300,0);
  racket1Enable=0;
  racket2Enable=0;

}

void setBall(float x,float y){
 for (int i=0;i<ballNumber;i++){
   balls[i].set(x,y);
  }
}







void Ball::update(bool racket1_ok,bool racket2_ok){

  v.y+=g*dt;//gravity applying
  v.x*=(1-drag*dt);//drag
  v.y*=(1-drag*dt);
  vang*=(1-rotDrag*dt);

  //magnus
  double coef=magnus*pow(v.x*v.x+v.y*v.y,0.5)*vang;
  v.x+=v.y*coef;
  v.y+=-v.x*coef;


  //printf("\n current y %.4f next %.4f  rot %.3f\n",y,next->y,vang*ballRadius);//***************hardcore debug (1 printf per update)



  handleBounce(tableEnd2,tableEnd1,tableBounceEfficiency,tableGrip);
  

  //screen bouncing  
  handleBounce(screenEndBR,screenEndTR,0.8,0);  
  handleBounce(screenEndBL,screenEndTL,0.8,0); 
  handleBounce(screenEndBR,screenEndBL,1.3,0);
  

  //net bouncing
    handleBounce(tableCenter,netSummit,tableBounceEfficiency,tableGrip);//FIXME handle net summit bouncing
  

  //racket bouncing
  for(int i=0;i<pointNumber/2;i++){
      if ((i==0 && racket1Enable<=0 && racket1_ok)||(i ==1 && racket2Enable<=0 && racket2_ok)){
      Point* a1=&pMem[m.prev()][2*i];
      Point* b1=&pMem[m.prev()][2*i+1];
      Point* a2=&pMem[m.get()][2*i];
      Point* b2=&pMem[m.get()][2*i+1];

      handleBounce(a2,b2,racketBounceEfficiency,racketGrip,i);

    }

  }


  /*position update******************************************************************/



  x+=v.x*dt;
  y+=v.y*dt;
  ang+=vang*dt;

}


void updateMoteur(bool racket1_ok,bool racket2_ok,double x[pointNumber],double y[pointNumber],double xbuf[ballNumber],double ybuf[ballNumber],double anglebuf[ballNumber]){

  racket1Enable--;
  racket2Enable--;

  if(!racket1_ok){
    racket1Enable=memSize/updateCycles+1;
  }

  if(!racket2_ok){
    racket2Enable=memSize/updateCycles+1;
  }


  for(int i=0;i<pointNumber;i++){
      Point *p=&pMem[m.get()][i];
      oldx[i]=p->x;
      oldy[i]=p->y;
  }

  for (int c=0;c<updateCycles;c++){

    /*mem update ***************************************************************/
    m.inc();

    for(int i=0;i<pointNumber;i++){
      Point *p=&pMem[m.get()][i];
      double transCoef=((double)c)/(updateCycles-1);
      p->x=transCoef*x[i]+(1-transCoef)*oldx[i];
      p->y=transCoef*y[i]+(1-transCoef)*oldy[i];
    }

    t+=dt;

    for(int b=0;b<ballNumber;b++){
      balls[b].update(racket1_ok,racket2_ok);
    }
  }

  for(int b=0;b<ballNumber;b++){
    if(xbuf!=NULL||ybuf!=NULL){
      xbuf[b]=balls[b].x;
      ybuf[b]=balls[b].y;
    }
    if(anglebuf!=NULL)
      anglebuf[b]=balls[b].ang;
  }

}




void Ball::set(double x0,double y0,double vx0,double vy0,double vang0){
    x=x0;
    y=y0;
    v.x=vx0;
    v.y=vy0;
    ang=0;
    vang=vang0;
  }



int MemIndex::next(){
  return (i+1)%memSize;
}

int MemIndex::prev(){
  return (i+memSize-1)%memSize;
}

int MemIndex::get(){
  return i;
}

void MemIndex::inc(){
  i=next();
}

bool Ball::touches(Point*p){
  return(pow(p->x-this->x,2)+pow(p->y-this->y,2)<sqRadius);
}

void Ball::handleBounce(Point*a, Point*b,double bEfficiency,double grip,int racketID){
  double sqLength=pow(b->x-a->x,2)+pow(b->y-a->y,2);
  double de=det(a,this,a,b);
  if(de*de/sqLength < sqRadius){
    //touching the line
    if(scal(a,b,a,this)>0 && scal(b,a,b,this)>0){
      //within line segment limits
      //printf("collision entre balle %.2f/%.2f et ligne  %.2f/%.2f  %.2f/%.2f\n",x,y,a->x,a->y,b->x,b->y);
      Point* projected=this->projected(a,b);
      pointBounce(projected,bEfficiency,grip,racketID);
      delete(projected);
      return;
    }else{
      //search for extremity bounces
      if(touches(a)){
	pointBounce(a,bEfficiency,grip,racketID);
      }
      if(touches(b)){
	pointBounce(b,bEfficiency,grip,racketID);
      }
    }
  }

}

Point* Point::projected(Point* a,Point* b){
  double sqLength=pow(b->x-a->x,2)+pow(b->y-a->y,2);
  double coef=scal(a,this,a,b)/sqLength;
  printf("projected %f %f\n",a->x+coef*(b->x-a->x),a->y+coef*(b->y-a->y));
  return ( new Point(a->x+coef*(b->x-a->x),a->y+coef*(b->y-a->y)));
}

/*bouncing ball b on a point p of a given  speed and bounce efficiency and grip efficiency*/
void Ball::pointBounce(Point* p,double bEfficiency,double grip,int racketID){


  
  double vxR=0,vyR=0;
  
  if(racketID!=-1){
    /*apply bouncing of ball b on the racket between point 2*i and 2*i+1*/
    Point* a=&pMem[m.get()][2*racketID];
    Point* b=&pMem[m.get()][2*racketID+1];
    
    
    Point* olda=&pMem[m.next()][2*racketID];
    Point* oldb=&pMem[m.next()][2*racketID+1];
    
    //calculating racket speed (next is the oldest entry of memory)
    vxR=-(olda->x+oldb->x-a->x-b->x)/(memSize*dt)/2;
    vyR=-(olda->y+oldb->y-a->y-b->y)/(memSize*dt)/2;
  }
  
#ifdef DEBUG
  printf("time:%.4f coord:%.2f/%.2f surface speed %.2f/%.2f\n     speed x/y/rot :  %.2f/%.2f/%.2f -> ",t,x,y,vxR,vyR,v.x,v.y,ballRadius*vang);
#endif

  double alpha;

  if(p->x==this->x){
    alpha=M_PI_2;
  }
  else{
    alpha=atan((p->y-this->y)/(p->x-this->x));
  }


  double c=cos(alpha);
  double s=sin(alpha);



  if(-s*this->x + c*this->y > -s* p->x + c* p->y ){
    c=-c;
    s=-s;
  }



  double vxL0=v.x-vxR;
  double vyL0=v.y-vyR;

  double vxL=c*vxL0+s*vyL0;
  double vyL=-s*vxL0+c*vyL0;
  

  if(vyL<0){
 #ifdef DEBUG
   printf("no bounce: ball leaving\n");
 #endif
  }


  //vertical rebound +  applying efficiency
  double deltaVy=(1+bEfficiency)*vyL;

  //horizontal/spinning
  double deltaVx=-1*0.4*(vxL+ballRadius*vang);
  double deltaVxMax=grip*(deltaVy);
  deltaVx=(deltaVx>deltaVxMax)?deltaVxMax:deltaVx;
  deltaVx=(deltaVx<-deltaVxMax)?-deltaVxMax:deltaVx;


  vxL+=deltaVx;
  vyL-=deltaVy;
  vang+=1.5*deltaVx/ballRadius;

  vxL0=c*vxL-s*vyL;
  vyL0=s*vxL+c*vyL;

  v.x=vxL0+vxR;
  v.y=vyL0+vyR;

#ifdef DEBUG
  printf("%.2f/%.2f/%.2f  (angle %.2f) coord moved to %.2f/%.2f\n",v.x,v.y,ballRadius*vang,alpha*90/M_PI_2,x,y);
#endif

}



//
///**useful for compilation testing*/
//int main(int argc, const char* argv[]){
//  init();
//
//   balls[0].set(xTableCenter+5,0,0.0,0,-0*100/ballRadius);//   for(int i=0;i<40;i++){ printf("c%i n%i p%i\n",m.get(),m.next(),m.prev());m.inc();}
//
//   /* for(double d=799.5;d<800.5;d+=0.001){
//     Point* p1=new Point(645,d);
//     printf("%f: %i\n",d,isLeft(&tableEnd1,&tableEnd2,p1));
//     delete(p1);
//   }
//   exit(0);*/
//
//  double x[4]={840,1,84000,1000};
//  double y[4]={790,790,750,750};
//
//  double xbu[ballNumber];
//  double ybu[ballNumber];
//
//  if(argc!=2){
//    printf("1 argument attendu\n");
//  }
//  else{
//    float iter=atoi(argv[1]);
//    int i;
//    for(int i=0;i<iter;i++){
//      //usleep(2000);
//      //    y[0]-=0*0.02;
//      //    y[1]-=0*0.02;
//      x[0]-=0.0;
//      x[1]-=0.0;
//
//      updateMoteur(1,x,y,xbu,ybu,xbu);
//      // printf("%.2f-",ybu[0]);
//    }
//  }
//}
//
