#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <cfloat>
#include <vector>
#include <thread>
#include <chrono>
#include <future>

#define FLOAT_EPS 0.00001

using namespace std;

class Point{
private:
    float x;
    float y;
public:
    Point(): x(0), y(0){}
    Point(float _x, float _y): x(_x), y(_y){}
    Point& operator=(Point other){
        if(this == &other)
            return* this;
        this->x = other.get_x();
        this->y = other.get_y();
        return* this;
    }

    float get_x(){ return x; }
    float get_y(){ return y; }

    void print(){
        cout << "Point x: " << x << ", y: " << y << endl;
    }
    float distance(Point other){
        return sqrt((x - other.x)*(x - other.x) + (y - other.y)*(y - other.y));
    }

    Point middle_point(Point other){
        return Point((x + other.x)/2, (y + other.y)/2);
    }
};

class PointsVector{
private:
    vector<Point> points;
public:
    PointsVector() {}
    PointsVector(size_t num_points){
        for(size_t i = 0; i < num_points; i++){
            points.push_back(Point(rand(), rand()));
        }
    }
    Point operator[](unsigned int i) { return points[i];}

    void add_point(Point temp){
        points.push_back(temp);
    }

    size_t size(){
        return points.size();
    }

    PointsVector get_elements(int from, int to){
        PointsVector result;
        for(int i = from; i < to && i < points.size(); i++){
            result.add_point(points[i]);
        }
        return result;
    }

    void print(){
        for(size_t i = 0; i < points.size(); i++){
            points[i].print();
        }
    }
};
