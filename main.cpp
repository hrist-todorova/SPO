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
#include <fstream>
#include "pool/ThreadPool.h"
#include "classes/circle.cpp"

#define FLOAT_EPS 0.00001
#define GRANULARITY 10

using namespace std;

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

PointsVector read_file(string path){
    PointsVector result;
    int rows = 0;
    ifstream ifs(path);
    ifs >> rows;
    for(int i = 0; i < rows; i++){
        float a, b;
        ifs >> a >> b;
        result.add_point(Point(a, b));
    }
    ifs.close();
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
    cout << "Smallest circle is" << endl;
    circles.smallest_circle(points).print_info();
    cout << endl;
    cout << endl;
    cout << endl;
    cout << "algorithm starts here." << endl;


    while(circles.size() >= 2){
        ThreadPool pool(tasks);
        vector< future<Circle> > results;


        for(size_t i = 0; i < circles.size(); i = i + GRANULARITY) {
            results.emplace_back(
                pool.enqueue([](CirclesVector c, PointsVector p){
                    return c.smallest_circle(p);
                }, circles.get_elements(i, min(i + GRANULARITY, circles.size())), points)
            );
        }

        CirclesVector temp;
        for(auto && result: results){
            Circle see = result.get();
            temp.add_circle(see);
        }
        circles = temp;
    }

    circles.print();


    printf ("\n");


    return 0;
}
