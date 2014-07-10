// **********************************************************************************
//
// BSD License.
// This file is part of a Hough Transformation tutorial,
// see: http://www.keymolen.com/2013/05/hough-transformation-c-implementation.html
//
// Copyright (c) 2013, Bruno Keymolen, email: bruno.keymolen@gmail.com
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, this
// list of conditions and the following disclaimer in the documentation and/or other
// materials provided with the distribution.
// Neither the name of "Bruno Keymolen" nor the names of its contributors may be
// used to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// **********************************************************************************

#include "hough.h"
#include <cmath>
#include <iostream>
#include <queue>
#include <string.h>
#include <stdlib.h>


#define DEG2RAD 0.017453293f


using namespace std;

namespace keymolen {
	class Point {
	public:
		int theta;
		int rho;
		int val;
		Point(int theta, int rho, int val)
			: theta(theta), rho(rho), val(val)
		{}
	};

	struct PointCompare {
		bool operator()(const Point &p1, const Point &p2) const {
		return p1.val < p2.val;
		}
	};

	Hough::Hough():_accu(0), _accu_w(0), _accu_h(0), _img_w(0), _img_h(0)
	{

	}

	Hough::~Hough() {
		if(_accu)
			free(_accu);
	}


	int Hough::Transform(unsigned char* img_data, int w, int h)
	{
		_img_w = w;
		_img_h = h;
		//Create the accu
		double hough_h = ((sqrt(2.0) * (double)(h>w?h:w)) / 2.0);
		_accu_h = hough_h * 2.0; // -r -> +r
		_accu_w = 180;
		//std::cout << "width: " << _accu_w <<" height: " << _accu_h << std::endl;

		_accu = (unsigned int*)calloc(_accu_h * _accu_w, sizeof(unsigned int));

		double center_x = w/2;
		double center_y = h/2;


		for(int y=0;y<h;y++)
		{
			for(int x=0;x<w;x++)
			{
				if( img_data[ (y*w) + x] > 250 )
				{
					for(int t=0;t<180;t++)
					{
						double r = ( ((double)x - center_x) * cos((double)t * DEG2RAD)) + (((double)y - center_y) * sin((double)t * DEG2RAD));
						_accu[ (int)((round(r + hough_h) * 180.0)) + t]++;
					}
				}
			}
		}

		return 0;
	}

	std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > Hough::GetLines(int threshold)
	{

		std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > lines;
		if(_accu == 0)
			return lines;

		int max_v = 0, max_r = 0, max_t = 0;
		for(int r=0;r<_accu_h;r++)
		{
			for(int t=0;t<_accu_w;t++)
			{
				if((int)_accu[(r*_accu_w) + t] > max_v) {
					max_v = (int)_accu[(r*_accu_w) + t];
					max_t = t;
					max_r = r;
				}
				if((int)_accu[(r*_accu_w) + t] >= threshold)
				{
					//Is this point a local maxima (9x9)
					int max = _accu[(r*_accu_w) + t];
					for(int ly=-4;ly<=4;ly++)
					{
						for(int lx=-4;lx<=4;lx++)
						{
							if( (ly+r>=0 && ly+r<_accu_h) && (lx+t>=0 && lx+t<_accu_w)  )
							{
								if( (int)_accu[( (r+ly)*_accu_w) + (t+lx)] > max )
								{
									max = _accu[( (r+ly)*_accu_w) + (t+lx)];
									ly = lx = 5;
								}
							}
						}
					}
					if(max > (int)_accu[(r*_accu_w) + t])
						continue;


					int x1, y1, x2, y2;
					x1 = y1 = x2 = y2 = 0;

					if(t >= 45 && t <= 135)
					{
						//y = (r - x cos(t)) / sin(t)
						x1 = 0;
						y1 = ((double)(r-(_accu_h/2)) - ((x1 - (_img_w/2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (_img_h / 2);
						x2 = _img_w - 0;
						y2 = ((double)(r-(_accu_h/2)) - ((x2 - (_img_w/2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (_img_h / 2);
					}
					else
					{
						//x = (r - y sin(t)) / cos(t);
						y1 = 0;
						x1 = ((double)(r-(_accu_h/2)) - ((y1 - (_img_h/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (_img_w / 2);
						y2 = _img_h - 0;
						x2 = ((double)(r-(_accu_h/2)) - ((y2 - (_img_h/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (_img_w / 2);
					}

					lines.push_back(std::pair< std::pair<int, int>, std::pair<int, int> >(std::pair<int, int>(x1,y1), std::pair<int, int>(x2,y2)));

				}
			}
		}

		priority_queue<Point, vector<Point>, PointCompare> queue;

		for(int r=0;r<_accu_h;r++)
		{
			for(int t=0;t<_accu_w;t++)
			{
				if((int)_accu[(r*_accu_w) + t] > max_v * 0.6 )
					queue.push(Point(t,r,(int)_accu[(r*_accu_w) + t]));
			}
		}
		cerr << "size: " << queue.size() << " " << max_v << endl;
		while (!queue.empty())
		{
			Point t = queue.top();
			std::cout << "rho: " << t.rho << " theta " << t.theta << std::endl;
			queue.pop();
		}

		//std::cout << "v: "<< max_v <<" r: " << max_r << " t: " << max_t<< std::endl;
		//std::cout << "lines: " << lines.size() << " " << threshold << std::endl;
		return lines;
	}

	const unsigned int* Hough::GetAccu(int *w, int *h)
	{
		*w = _accu_w;
		*h = _accu_h;

		return _accu;
	}
}
