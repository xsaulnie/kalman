#ifndef INFO_HPP
#define INFO_HPP
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

struct Point
{
    double x;
    double y;
    double z;
};

class Info
{
   private:
   double coord[3];
   int idx;
   char choice;
   int here;
   double dmesure;

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
   struct Point incpred;
   struct Point dpred;
   struct Point backup;
   struct Point backa;


   Info(double siga, double sigp);
   //void load_Info0(char buf[]);
   void ana(std::string buf);
   void add(std::string buf);
   char save_choice(std::string buf);
   void saving();
   void save(Point * var);
   void writing(std::string buf);
   void compute(int step);
   std::string response();
   //struct Point speed(double V, struct Point euler);
   void display();
   void save_to_prev();
};
#endif