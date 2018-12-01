#include "ofApp.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <tgmath.h>

using std::cout;
using std::endl;


float lerp(float x1, float x2, float w)
{
  return x1 + w * (x2 - x1);
}
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
    sed[i] = 0;
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

  //We use the model described in 
  //www-ljk.imag.fr/Publications/Basilic/com.lmc.publi.PUBLI_Inproceedings@117681e94b6_fff75c/FastErosion_PG07.pdf
  float gravity = 1.0;
  float distance = 1.0;
  float time_step = 0.1;
  float sediment_capacity_constant = 0.1;
  float dissolving_constant = 1;
  float sedimentation_constant = 1;
  float evaporation_constant = 0.0001;

  // rain
  for(int i = 0; i < SIZE; i++)
  {
    //water[i] += 0.05;
  }
  //water[(int)ofRandom(SIZE)] = 0.1;

  // water movement
  for(int i = 0; i < SIZE; i++)
  {
    float left_height_diff = (heights[i] + water[i]) - (heights[i-1] + water[i-1]);
    pipe_flux_left[i] = max(0.0f, pipe_flux_left[i] + time_step * gravity * left_height_diff);

    float right_height_diff = (heights[i] + water[i]) - (heights[i+1] + water[i+1]);
    pipe_flux_right[i] = max(0.0f, pipe_flux_right[i] + time_step * gravity * right_height_diff);

    //don't output water to the left if on the left edge or to the right if
    //on the right edge
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
    float flux_in;

    //only take water input from the left if on the right edge and right if
    //on the left edge
    if(i == 0)
    {
      flux_in = pipe_flux_left[i+1];
    }
    else if(i == SIZE - 1)
    {
      flux_in = pipe_flux_right[i-1];
    }
    else
    {
      flux_in = pipe_flux_right[i-1] + pipe_flux_left[i+1];
    }

    float flux_out = pipe_flux_right[i] + pipe_flux_left[i];

    float water_area_diff = time_step * (flux_in - flux_out);

    float prev_water = water[i];
    water[i] += water_area_diff / distance;

    float passed_water = (pipe_flux_right[i-1] - pipe_flux_left[i] + pipe_flux_right[i] - pipe_flux_left[i+1]) / 2;
    float  average_water = (prev_water + water[i]) / 2;
    if(passed_water == 0)
    {
      vel[i] = 0;
    }
    else
    {
      vel[i] = passed_water / (distance * average_water);
    }
  }

  // sediment dissolution
  for(int i = 0; i < SIZE; i++)
  {
    float local_angle;
    if(vel[i] > 0)
    {
      local_angle = atan((heights[i] - heights[i+1]) / distance);
      if(i == SIZE - 1)
      {
        local_angle = atan((heights[i-1] - heights[i]) / distance);
      }
    }
    else
    {
      local_angle = atan((heights[i] - heights[i-1]) / distance);
      if(i == 0)
      {
        local_angle = atan((heights[i+1] - heights[i]) / distance);
      }
    }

    float sediment_capacity = sediment_capacity_constant * sin(local_angle) * abs(vel[i]);
    cout << sediment_capacity << endl;
    cout << sed[i] << endl;
    cout << sin(local_angle) << " " << abs(vel[i]) << endl;

    if(sediment_capacity > sed[i])
    {
      float sediment_exchange = dissolving_constant * (sediment_capacity - sed[i]);
      cout << "eroding: " << sediment_exchange << endl;
      heights[i] -= sediment_exchange;
      sed[i] += sediment_exchange;
    }
    else if(sediment_capacity < sed[i])
    {
      
      float sediment_exchange = sedimentation_constant * (sed[i] - sediment_capacity);
      cout << "depositing: " << sediment_exchange << endl;
      heights[i] += sediment_exchange;
      sed[i] -= sediment_exchange;
      
    }
    else 
    {
      cout << "What?" << endl;
    }
  }

  // sediment movement
  /*
  float tmp_sed[SIZE];
  for(int i = 0; i < SIZE; i++)
  {
    float previous_position = (float)i - (vel[i] * time_step);
    int previous_index = (int)previous_position;
    int left_index = previous_index;
    int right_index = previous_index + 1;
    float interp = previous_position - (float)left_index;

    //cout << "Moving from " << (int)(i - vel[i] * time_step) << " to " << i << " at " << vel[i] << endl;
    if(left_index < 0)
    {
      tmp_sed[i] = sed[0];
    }
    else if(right_index >= SIZE)
    {
      tmp_sed[i] = sed[SIZE-1];
    }
    else
    {
      tmp_sed[i] = lerp(sed[left_index], sed[right_index], interp);
    }
  }
  for(int i = 0; i < SIZE; i++)
  {
    sed[i] = tmp_sed[i];
  }
  */
  

  // evaporation
  for(int i = 0; i < SIZE; i++)
  {
    water[i] = water[i] * (1 - evaporation_constant * time_step);
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
  for(int i = 0; i < SIZE; i++)
  {
    ofSetColor(255, 0, 0);
    ofDrawLine(water_rects[i].getCenter().x, water_rects[i].getCenter().y, water_rects[i].getCenter().x + vel[i] * WIDTH, water_rects[i].getCenter().y);
  }


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){


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
  water[(int)(x / WIDTH)] += 10;
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
