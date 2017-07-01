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
#include <mutex>
#include <fstream>
#include "classes/circle.cpp"

#define FLOAT_EPS 0.00001
#define GRANULARITY 35

using namespace std;
using namespace chrono;

int points_num = 2;
string path = "";
int tasks = 1;
bool quiet_mode = false;

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
    if(isnan(x) || isnan(y)){
        return Circle(Point(0,0), -1);
    }

    Point center(x, y);
    float r = center.distance(one);
    return Circle(center, r);
}

CirclesVector make_all_possible_circles(PointsVector& data, int tasks_num){
    CirclesVector result;
    int n = 0;
    vector<thread> th(tasks_num);
    mutex mn, mresult;

    if(!quiet_mode)
        printf("Generating circles by two points ...\n");
    for(int i = 0; i < tasks_num; i++){
        th[i] = thread([&, i](){
            if(!quiet_mode)
                printf("Thread %d started.\n", i + 1);
            while(n < data.size() - 1){
                mn.lock();
                int index = n;
                Point first_point = data[n];
                n++;
                mn.unlock();
                for(int t = index + 1; t < data.size(); t++){
                    Circle temp = make_circle_by_two_points(first_point, data[t]);
                    if(temp.isValid() && temp.circle_contains_all_points(data)){
                        mresult.lock();
                        result.add_circle(temp);
                        mresult.unlock();
                    }
                }
            }
            if(!quiet_mode)
                printf("Thread %d stopped.\n", i + 1);
        });
    }

    for(int i = 0; i < tasks_num; i++){
        th[i].join();
    }

    if(!quiet_mode)
        printf("Generating circles by three points ...\n");

    n = 0;
    for(int i = 0; i < tasks_num; i++){
        th[i] = thread([&, i](){
            if(!quiet_mode)
                printf("Thread %d started.\n", i + 1);
            while(n < data.size() - 2){
                mn.lock();
                int index = n;
                auto first_point = data[n];
                n++;
                mn.unlock();
                for(int t = index + 1; t < data.size() - 1; t++){
                    for(int y = t + 1; y < data.size(); y++){
                        Circle temp = make_circle_by_three_points(first_point, data[t], data[y]);
                        if(temp.isValid() && temp.circle_contains_all_points(data)){
                            mresult.lock();
                            result.add_circle(temp);
                            mresult.unlock();
                        }
                    }
                }
            }
            if(!quiet_mode)
                printf("Thread %d stopped.\n", i + 1);
        });
    }


    for(int i = 0; i < tasks_num; i++){
        th[i].join();
    }

    return result;
}

PointsVector read_file(string path){
    PointsVector result;
    int rows = 0;
    ifstream file;
    file.open(path);
    if(file.is_open()){
        file >> rows;
        for(int i = 0; i < rows; i++){
            float a, b;
            file >> a >> b;
            result.add_point(Point(a, b));
        }
        file.close();
    }

    return result;
}

Circle find_circle(CirclesVector circles, PointsVector& points, int task){
    vector<thread> th(task);
    mutex mvect, mn;

    while(circles.size() >= 2){
        CirclesVector temp;
        size_t n = 0;
        for(int i = 0; i < task; i++){
            th[i] = thread([&, i](){
                if(!quiet_mode)
                    printf("Thread %d started.\n", i + 1);
                while(n < circles.size()){
                    mn.lock();
                    size_t index = n;
                    n += GRANULARITY;
                    mn.unlock();
                    auto small = circles.get_elements(index, index + GRANULARITY).smallest_circle(points);
                    if(small.get_radius() != -1)
                        temp.add_circle(small);
                }
                if(!quiet_mode)
                    printf("Thread %d stopped.\n", i + 1);
            });
        }

        for(int i = 0; i < task; i++){
            th[i].join();
        }
        mvect.lock();
        circles = temp;
        mvect.unlock();
    }

    return circles[0];
}

void find_smallest_enclosing_circle(PointsVector& data){
    auto start = high_resolution_clock::now();

    CirclesVector circles = make_all_possible_circles(data, tasks);
    if(!quiet_mode)
        printf("%u circles created.\n", circles.size());

    printf("Finding the smallest circle ..... \n");
    Circle c = find_circle(circles, data, tasks);
    auto ending = high_resolution_clock::now();

    printf("\nThe circle we are looking for is: \n");
    c.print();


    duration<float> secs = ending - start;
    milliseconds ms = duration_cast<milliseconds>(secs);
    printf("\nTime used to calculate the problem: \n");
    cout << secs.count() << "s\n";
    cout << ms.count() << "ms\n";
    printf ("\n");
}

int main(int argc, char *argv[])
{
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
        if(strcmp(argv[i], "-q") == 0){
            quiet_mode = true;
        }
    }

    PointsVector points;
    if(!quiet_mode)
        printf("Creating points ... \n");
    if(strcmp(path.c_str(), "") == 0){
        points = PointsVector(points_num);
    }
    else{
        points = read_file(path);
    }
    if(!quiet_mode)
        printf("The points are created.\n");

    if(points.size() >= 2){
        find_smallest_enclosing_circle(points);
    }


    return 0;
}
