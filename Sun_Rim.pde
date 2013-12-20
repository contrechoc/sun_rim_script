/*

This script is used in the solar rim dress

It gets a picture of the sun and uses a filter to scan a circle for bright spots.

It sends 64 points to an ATMega328 with a RF transceiver, using the serial port.

contrechoc

2013

*/

import processing.serial.*;

// The serial port:
Serial myPort;

PImage img;
int smallPoint, largePoint;

long timer;
long interval = 1500;

float v = 1.0 / 144.0;
float[][] kernel = {
  { 
    v, v, v, v, v, v, v
  }
  , 
  { 
    v, v, v, v, v, v, v
  }
  , 
  { 
    v, v, v, v, v, v, v
  }
  , 
  { 
    v, v, v, v, v, v, v
  }
  , 
  { 
    v, v, v, v, v, v, v
  }
  , 
  { 
    v, v, v, v, v, v, v
  }
  , 
  { 
    v, v, v, v, v, v, v
  }
};


char[] sendingChars = new char[8];


void setup() {
  size(500, 480);
  img = loadImage("http://soho.nascom.nasa.gov/data/realtime/eit_195/512/latest.jpg");
  smallPoint = 4;
  largePoint = 40;
  imageMode(CENTER);
  noStroke();
  background(255);
  image(img, width/2, height/2);

  // List all the available serial ports:
  println(Serial.list());

  // Open the port you are using at the rate you want:
  myPort = new Serial(this, Serial.list()[0], 9600);

  timer = millis() + interval;
}

void draw() { 
  if (timer< millis() ) {
    background(255);
    image(img, width/2, height/2);
    int mXX =  int( (mouseY+0.0)/50.0);

    //image(img, width/2, height/2);
    float fRadius =  .904;//(mouseX + 0.0 - width/2.0) / 500.0 + 1.0;

    float numberOfP = 64;//mouseY/5;
    text(numberOfP, 50, 50);
    // println( numberOfP );
    int charCounter = 0;
    int bitCounter = 0;

    for ( int i = 0; i < 8 ; i++) {
      sendingChars[i] = 0;
    }

    for ( int i = 0; i < int(numberOfP) ; i++) {
      float radius = 200 + int( (+width/2 - mouseX + random(150))/5.0) ;
      float px = width/2 + radius * fRadius*sin( float(i) / numberOfP *PI*2 );
      float py = height/2 + radius * fRadius* cos( float(i) / numberOfP *PI*2 );

      float sum = 0; // Kernel sum for this pixel
      int widthMatrix = mXX + 2;
      for (int ky = -widthMatrix; ky <= widthMatrix; ky++) {
        for (int kx = -widthMatrix; kx <= widthMatrix; kx++) {

          float val = red(img.get(int(px+ kx), int(py+ ky)))+
            green(img.get(int(px+ kx), int(py+ ky)))+
            blue(img.get(int(px+ kx), int(py+ ky)))
            ;
          sum += val/3.0/(widthMatrix*2.0+0.0)/(widthMatrix*2.0+0.0);
        }
      }
      //cutoff
      if (sum > 100 ) sum = 255; 
      else sum = 0;
      color pix =color(sum);

      fill(pix);
      ellipse(px, py, 10, 10);
      if ( i == 0 )  ellipse(px, py, 20, 20);
      if ( i == 8 )  ellipse(px, py, 10, 20);

      if ( sum == 255 ) {
        sendingChars[charCounter] |= (1<<bitCounter);
      }
      bitCounter++;
      if ( bitCounter > 7) {
        bitCounter = 0;
        charCounter++;
      }
    }


    {
      for ( int i = 0; i < 8 ; i++) {
        print(sendingChars[i]);
        print("  ");
      }
      println("");
      timer = millis() + interval;
      myPort.write("#" + sendingChars[7] +sendingChars[6] +sendingChars[4] +sendingChars[3] +sendingChars[3] 
        +sendingChars[2] +sendingChars[1] +sendingChars[0]  );
    }
  }
}

