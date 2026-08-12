#ifndef INFO_HPP
#define INFO_HPP
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

class Point
{
    public:
    double x;
    double y;
    double z;

    double get(int idx);
};

class Info
{
   private:
   double coord[3];
   int idx;
   char choice;
   double dmesure;
   std::vector<struct Point> trajectory;

   public:
   struct Point position0;
   double speed0;

   struct Point cur_pos;
   struct Point cur_acc;
   struct Point cur_dir;
   struct Point cur_V;
   struct Point result;
   struct Point true_pos;
   struct Point prev_acc;
   struct Point prev_V;
   struct Point prev_dir;
   struct Point incpred;
   struct Point dpred;
   struct Point backup;
   struct Point backa;


   Info(double siga, double sigp);
   void ana(std::string buf);
   void add(std::string buf);
   char save_choice(std::string buf);
   void saving();
   void save(Point * var);
   void writing(std::string buf);
   void compute(int step);
   void compute_by_speed(int step);
   void save_point_trajectory(struct Point p);
   std::string response();
   void display();
   void save_to_prev(int step);
   void show_trajectory();
   struct Point hist(int idx);
   size_t histsize();
};
#endif