#pragma once

#define SIZE 100

#include "ofMain.h"

class ofApp : public ofBaseApp{

  private:

    int counter;
    int WIDTH;
    int window_height, window_width;

    float heights[SIZE];
    float water[SIZE];
    float sed[SIZE];
    ofRectangle heights_rects[SIZE];
    ofRectangle water_rects[SIZE];
    ofRectangle sed_rects[SIZE];

    float vel[SIZE];
    float pipe_flux_right[SIZE]; // Represents the fluid flow between columns i and i+1
    float pipe_flux_left[SIZE]; // Represents the fluid flow between columns i and i-1
    

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
};
