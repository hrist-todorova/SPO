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
#include "pool/ThreadPool.h"

#define FLOAT_EPS 0.00001
#define GRANULARITY 10

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
        for(int i = from; i < to; i++){
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

        if(!found){
            cout << "There's no such circle" << endl;
        }

        return result;
    }
};

Circle make_circle_by_two_points(Point one, Point two){
    Point c = one.middle_point(two);
    float r = c.distance(one);
    return Circle(c, r);
}

Circle make_circle_by_three_points(Point one, Point two, Point three){
    float ma = (two.get_y() - one.get_y())/(two.get_x() - one.get_x()); // first slope
    float mb = (three.get_y() - two.get_y())/(three.get_x() - two.get_x()); // second slope

    float x = (ma*mb*(one.get_y() - three.get_y()) + mb*(one.get_x() + two.get_x()) - ma*(two.get_x() + three.get_x()))/(2*(mb - ma));

    float y = -(1 / ma)*(x - (one.get_x() + two.get_x())/2) + (one.get_y() + two.get_y())/2;
    Point center(x, y);
    float r = center.distance(one);

    if(isnan(x) || isnan(y)){
        return Circle(Point(0,0), -1);
    }

    return Circle(center, r);
}

CirclesVector make_all_possible_circles(PointsVector vec){
    CirclesVector result;
    for(size_t i = 0; i < vec.size() - 1; i++){
        for(size_t j = i+1; j < vec.size(); j++){
            Circle temp = make_circle_by_two_points(vec[i], vec[j]);
            if(temp.isValid()){
                result.add_circle(temp);
            }
        }
    }
    for(size_t i = 0; i < vec.size() - 2; i++){
        for(size_t j = i + 1; j < vec.size() - 1; j++){
            for(size_t k = j + 1; k < vec.size(); k++){
                Circle temp = make_circle_by_three_points(vec[i], vec[j], vec[k]);
                if(temp.isValid()){
                    result.add_circle(temp);
                }
            }
        }
    }
    return result;
}

PointsVector generate_points(int points){
    PointsVector result;
    for(int i = 0; i < points; i++){
        result.add_point(Point(rand(), rand()));
    }

    return result;
}

PointsVector read_file(string path){ //DOESN'T COMPILE
    PointsVector result;
    int rows = 0;
    //ifstream infile(path);
   // while(infile >> rows){
    //    cout << "Value from file is " << rows << endl;
    //}

    return result;
}

void threadFn(int & x){
    cout << "Biiitch" << x++;
    x = x+1;
}

int main(int argc, char *argv[])
{
    int points_num = 0;
    string path = "";
    int tasks = 1;

    for(size_t i = 0; i < argc; i++){
        if(strcmp(argv[i], "-n") == 0){
            points_num = atoi(argv[i + 1]);
        }
        if(strcmp(argv[i], "-i") == 0){
            path = argv[i + 1];
        }
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "-tasks") == 0){
            tasks = atoi(argv[i + 1]);
        }
        //to-do ADD QUIET MODE
    }

    printf("The number of tasks is %d\n", tasks);

    PointsVector points;
    if(strcmp(path.c_str(), "") == 0){
        printf("The number of points is %d\n", points_num);
        points = generate_points(points_num);
    }
    else{
        printf("The points file is %s\n", path.c_str());
        points = read_file(path);
    }

    CirclesVector circles = make_all_possible_circles(points);

    ThreadPool pool(tasks);
    vector< future<Circle> > results;

    circles.smallest_circle(points).print_info();


    size_t size_ = circles.size();
    for(size_t i = 0; i < circles.size(); i++) {
        results.emplace_back(
            pool.enqueue([](CirclesVector temp, PointsVector wft){
                return temp.smallest_circle(wft);
            }, circles, points)
        );
    }


    for(auto && result: results){
        Circle see = result.get();
        see.print_info();
    }


    printf ("\n");






    return 0;
}
