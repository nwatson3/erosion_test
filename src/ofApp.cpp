#include "ofApp.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <tgmath.h>

using std::cout;
using std::endl;

//--------------------------------------------------------------
void ofApp::setup(){
  counter = 0;
  for(int i = 0; i < SIZE; i++)
  {
    heights_rects[i] = ofRectangle(0, 0, 0, 0);
    water_rects[i] = ofRectangle(0, 0, 0, 0);
    sed_rects[i] = ofRectangle(0, 0, 0, 0);
    heights[i] = 8 + 4 * sin((float)i/6.0);
    //heights[i] = 12;
  }

}

//--------------------------------------------------------------
void ofApp::update(){

  for(int i = 0; i < SIZE; i++)
  {
    heights_rects[i].setY(window_height);
    heights_rects[i].setX(i * WIDTH);
    heights_rects[i].setHeight(-WIDTH * heights[i]);
    heights_rects[i].setWidth(WIDTH);

    sed_rects[i].setY(heights_rects[i].getMinY());
    sed_rects[i].setX(heights_rects[i].getMinX());
    sed_rects[i].setHeight(-WIDTH * sed[i]);
    sed_rects[i].setWidth(WIDTH);

    water_rects[i].setY(sed_rects[i].getMinY());
    water_rects[i].setX(sed_rects[i].getMinX());
    water_rects[i].setHeight(-WIDTH * water[i]);
    water_rects[i].setWidth(WIDTH);
  }

}

//--------------------------------------------------------------
void ofApp::draw(){

  for(int i = 0; i < SIZE; i++)
  {
    ofSetColor(0,0,0);
    ofDrawRectangle(heights_rects[i]);
    ofSetColor(0,0,255);
    ofDrawRectangle(water_rects[i]);
    ofSetColor(127,127,127);
    ofDrawRectangle(sed_rects[i]);
  }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

  int upperkey = toupper(key);

  if(upperkey != 'J')
    return;

  int step = counter % 2;

  //We use the model described in 
  //www-ljk.imag.fr/Publications/Basilic/com.lmc.publi.PUBLI_Inproceedings@117681e94b6_fff75c/FastErosion_PG07.pdf
  //
  if(step == 0)
  {
    water[40] += 0.5;
  }
  if(step == 1)
  {
    float gravity = 1.0;
    float distance = 1.0;
    float time_step = 0.5;
    for(int i = 0; i < SIZE; i++)
    {
      float left_height_diff = (heights[i] + water[i]) - (heights[i-1] + water[i-1]);
      pipe_flux_left[i] = max(0.0f, pipe_flux_left[i] + time_step * gravity * left_height_diff);

      float right_height_diff = (heights[i] + water[i]) - (heights[i+1] + water[i+1]);
      pipe_flux_right[i] = max(0.0f, pipe_flux_right[i] + time_step * gravity * right_height_diff);

      if(i == 0)
      {
        pipe_flux_left[i] = 0;
      }
      if(i == SIZE - 1)
      {
        pipe_flux_right[i] = 0;
      }
    
      float scaling_factor = min(1.0f, (water[i] * distance) / (pipe_flux_left[i] + pipe_flux_right[i]));

      pipe_flux_left[i] *= scaling_factor;
      pipe_flux_right[i] *= scaling_factor;
    }
    for(int i = 0; i < SIZE; i++)
    {
      float flux_in = pipe_flux_right[i-1] + pipe_flux_left[i+1];

      if(i == 0)
      {
        flux_in = pipe_flux_left[i+1];
      }
      if(i == SIZE - 1)
      {
        flux_in = pipe_flux_right[i-1];
      }

      float flux_out = pipe_flux_right[i] + pipe_flux_left[i];

      float water_area_diff = time_step * (flux_in - flux_out);

      float prev_water = water[i];
      water[i] += water_area_diff / distance;

      float passed_water = (pipe_flux_right[i-1] - pipe_flux_left[i] + pipe_flux_right[i] - pipe_flux_left[i+1]) / 2;
      float  average_water = (prev_water + water[i]) / 2;
      vel[i] = passed_water / (distance * average_water);
    }
  }



  
  counter++;

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
  window_height = h;
  window_width = w;
  WIDTH = window_width / SIZE;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
