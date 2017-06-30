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
#define GRANULARITY 80

using namespace std;
using namespace chrono;

int points_num = 0;
string path = "";
int tasks = 1;

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

CirclesVector make_all_possible_circles(PointsVector& data){
    CirclesVector result;
    int n = 0;
    vector<thread> th(tasks);
    mutex mn, mresult;

    cout << "Start2" << endl;
    for(int i = 0; i < tasks; i++){
        th[i] = thread([&](){
            while(n < data.size() - 1){
                mn.lock();
                int index = n;
                auto point = data[n];
                n++;
                mn.unlock();
                for(int t = index + 1; t < data.size(); t++){
                    Circle temp = make_circle_by_two_points(point, data[t]);
                    if(temp.isValid()){
                        mresult.lock();
                        result.add_circle(temp);
                        mresult.unlock();
                    }
                }
            }
        });
    }

    for(int i = 0; i < tasks; i++){
        th[i].join();
    }
    cout << "End2" << endl;
    cout << "Start3" << endl;

    n = 0;
    for(int i = 0; i < tasks; i++){
        th[i] = thread([&](){
            while(n < data.size() - 2){
                mn.lock();
                int index = n;
                auto point = data[n];
                n++;
                mn.unlock();
                for(int t = index + 1; t < data.size() - 1; t++){
                    for(int y = t + 1; y < data.size(); y++){
                        Circle temp = make_circle_by_three_points(point, data[t], data[y]);
                        if(temp.isValid()){
                            mresult.lock();
                            result.add_circle(temp);
                            mresult.unlock();
                        }
                    }
                }
            }
        });
    }

    for(int i = 0; i < tasks; i++){
        th[i].join();
    }
    cout << "End3" << endl;

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

void generate_tests(CirclesVector circles, PointsVector& points, int task){
    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    typedef std::chrono::duration<float> fsec;

    auto t0 = Time::now();


    vector<thread> th(task);
    mutex mvect, mn;
    //mutex mn, mresult;

    while(circles.size() >= 2){
        CirclesVector temp;
        size_t n = 0;
        for(int i = 0; i < task; i++){
            th[i] = thread([&](){
                while(n < circles.size()){
                    mn.lock();
                    size_t index = n;
                    n += GRANULARITY;
                    mn.unlock();
                    auto c = circles.get_elements(index, min(index + GRANULARITY, circles.size()));

                    auto small = c.smallest_circle(points);
                    if(small.get_radius() != -1)
                        temp.add_circle(c.smallest_circle(points));
                }
            });
        }


        for(int i = 0; i < task; i++){
            th[i].join();
        }


        mvect.lock();
        circles = temp;
        mvect.unlock();
    }





    /*while(circles.size() >= 2){
        ThreadPool pool(tasks);
        vector< future<Circle> > results;


        for(size_t i = 0; i < circles.size(); i = i + GRANULARITY) {
            results.emplace_back(
                pool.enqueue([&](){
                    auto c = circles.get_elements(i, min(i + GRANULARITY, circles.size()));
                    c.print();
                    cout << "\n\n\n\n";
                    return c.smallest_circle(points);
                })
            );
        }

        CirclesVector temp;
        for(auto && result: results){
            Circle see = result.get();
            temp.add_circle(see);
        }
        circles = temp;
    }*/

    auto t1 = Time::now();
    fsec fs = t1 - t0;
    ms d = std::chrono::duration_cast<ms>(fs);
    std::cout << fs.count() << "s\n";
    std::cout << d.count() << "ms\n";
    printf ("\n");
}

void tester(CirclesVector circles, PointsVector points){
    cout << 1 << endl;
    generate_tests(circles, points, 1);
    cout << 2 << endl;
    generate_tests(circles, points, 2);
    cout << 3 << endl;
    generate_tests(circles, points, 3);
    cout << 4 << endl;
    generate_tests(circles, points, 4);
    cout << 5 << endl;
    generate_tests(circles, points, 5);
    cout << 6 << endl;
    generate_tests(circles, points, 6);
    cout << 7 << endl;
    generate_tests(circles, points, 7);
    cout << 8 << endl;
    generate_tests(circles, points, 8);
    cout << 9 << endl;
    generate_tests(circles, points, 9);
    cout << 10 << endl;
    generate_tests(circles, points, 10);
    cout << 11 << endl;
    generate_tests(circles, points, 11);
    cout << 12 << endl;
    generate_tests(circles, points, 12);
    cout << 13 << endl;
    generate_tests(circles, points, 13);
    cout << 14 << endl;
    generate_tests(circles, points, 14);
    cout << 15 << endl;
    generate_tests(circles, points, 15);
    cout << 16 << endl;
    generate_tests(circles, points, 16);
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
        //to-do ADD QUIET MODE
    }

    PointsVector points;
    cout << "generate points" << endl;
    if(strcmp(path.c_str(), "") == 0){
        points = generate_points(points_num);
    }
    else{
        points = read_file(path);
    }
    cout << "points are generated" << endl;


    cout << "find all circles, containing the points" << endl;
    CirclesVector circles = make_all_possible_circles(points);
    cout << circles.size() << endl;
    cout << "circles are ready" << endl;


    tester(circles, points);

    //https://www.desmos.com/calculator
    return 0;
}
