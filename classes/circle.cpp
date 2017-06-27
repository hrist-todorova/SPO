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
#include "point.cpp"

#define FLOAT_EPS 0.00001
#define GRANULARITY 10

using namespace std;


class Circle{
private:
    Point c;
    float r;
public:
    Circle(){
        c = Point(0,0);
        r = 0;
    }
    Circle(Point _c, float _r): c(_c), r(_r) {}

    Circle& operator=(Circle other){
        if(this == &other)
            return* this;
        this->r = other.get_radius();
        this->c = other.get_center();
        return* this;
    }

    bool isValid(){
        return r >= 0;
    }

    float get_radius(){
        return r;
    }
    Point get_center(){
        return c;
    }

    void print_info(){
        cout << "Circle with radius = " << r << " and center (" << c.get_x() << ", ";
        cout << c.get_y() << ")." << endl;
    }

    bool contains_point(Point a){
        return c.distance(a) <= r + FLOAT_EPS;
    }

    bool circle_contains_all_points(PointsVector vec){
        for(size_t i = 0; i < vec.size(); i++){
            if(!contains_point(vec[i]))
                return false;
        }
        return true;
    }
};

class CirclesVector{
private:
    vector<Circle> circles;
public:
    CirclesVector() {}

    Circle operator[](unsigned int i) { return circles[i];}

    CirclesVector& operator=(CirclesVector other){
        if(this == &other)
            return* this;
        this->circles = other.get_vector();
        return* this;
    }

    void add_circle(Circle temp){
        circles.push_back(temp);
    }

    vector<Circle> get_vector(){
        return circles;
    }

    size_t size(){
        return circles.size();
    }
    CirclesVector get_elements(int from, int to){
        CirclesVector result;
        for(int i = from; i < to; i++){
            result.add_circle(circles[i]);
        }
        return result;
    }

    void print(){
        for(size_t i = 0; i < circles.size(); i++){
            circles[i].print_info();
        }
    }

    Circle smallest_circle(PointsVector vec){
        Circle result = Circle(Point(0,0), -1);
        bool found = false;

        for(size_t i = 0; i < circles.size(); i++){
            if(!found && circles[i].circle_contains_all_points(vec)){
                found = true;
                result = circles[i];
            }
            if(found && circles[i].circle_contains_all_points(vec) && result.get_radius() > circles[i].get_radius()){
                result = circles[i];
            }
        }

        return result;
    }
};
