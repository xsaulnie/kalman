#include <unistd.h>
#include <string.h>
#include <cstdio>
#include <vector>
#include "Info.hpp"
#include <math.h>

double Point::get(int idx)
{
    if (idx == 1)
        return this->x;
    else if (idx == 2)
        return this->y;
    else if (idx == 3)
        return this->z;
    return 0;
}

Info::Info(double accsig, double accpos)
{
    std::cerr << accsig << " " <<accpos << std::endl;
    this->coord[0] = 0;
    this->coord[1] = 0;
    this->coord[2] = 0; 
    this->idx = 0;
    this->choice = '0';
    this->incpred = {.x= accsig, .y=accsig, .z= accsig};
    //this->incpred = {.x = 0.0004, .y = 0.001, .z= 0.001};
    this->dpred = {.x = 0, .y = 0, .z = 0};
    this->dmesure = accpos;
    //this->dmesure = 0.2;
}

char Info::save_choice(std::string buf)
{
        if (buf.find("ACCELERATION") != std::string::npos)
            this->choice = 'a';
        else if (buf.find("DIRECTION") != std::string::npos)
            this->choice = 'd';
        else if (buf.find("TRUE POSITION") != std::string::npos)
            this->choice = 't';
        else if (buf.find("POSITION") != std::string::npos)
            this->choice = 'p';
        else if (buf.find("SPEED")!= std::string::npos)
            this->choice = 's';

    return this->choice;
}

void Info::ana(std::string buf)
{
    size_t pos = 0;

    std::cout << "From Server\n<<" << buf << ">>" << std::endl;

    pos = buf.find("\n");
    if (save_choice(buf.substr(0, pos)) == '0')
        return;

    buf.erase(0, pos + 1);

    while ((pos = buf.find("\n")) != std::string::npos) {
        this->coord[this->idx] = stod(buf.substr(0, pos));
        this->idx = (this->idx + 1) % 3;
        buf.erase(0, pos + 1);
    }

    saving();
    this->choice = '0';
    this->idx = 0;
}

void Info::saving()
{
    if (this->choice == 'a')
        save(&this->cur_acc);
    else if (this->choice == 'd')
        save(&this->cur_dir);
    else if (this->choice == 'p')
        save(&this->cur_pos);
    else if (this->choice == 't')
        save(&this->true_pos);
    else if (this->choice == 's')
        this->speed0 = coord[0];
}

void Info::save(Point * var)
{
    var->x = coord[0];
    var->y = coord[1];
    var->z = coord[2];
}

void Info::display()
{
    printf("cur_pos\n");
    printf("%.20lf %.20lf %.20lf\n", this->cur_pos.x, this->cur_pos.y, this->cur_pos.z);

    printf("speed\n");
    printf("%.20lf\n", this->speed0);

    printf("cur_acc\n");
    printf("%.20lf %.20lf %.20lf\n", this->cur_acc.x, this->cur_acc.y, this->cur_acc.z);

    printf("cur_dir\n");
    printf("%.20lf %.20lf %.20lf\n", this->cur_dir.x, this->cur_dir.y, this->cur_dir.z);

    printf("true_pos\n");
    printf("%.20lf %.20lf %.20lf\n", this->true_pos.x, this->true_pos.y, this->true_pos.z);
}

void Info::compute_by_speed(int step)
{
    // struct Point mesure;
    // struct Point gain;

    //connu pos-1 : estimated(speed) inconnu : prochaine position.

    struct Point vglobal;

    
    if (step == 0)
    {
        this->result.x = this->true_pos.x;
        this->result.y = this->true_pos.y;
        this->result.z = this->true_pos.z;

        this->cur_V.x = this->speed0 /3.6;
        this->cur_V.y = 0;
        this->cur_V.z = 0;
        save_to_prev();
        return;
    }
    if (step == 1)
    {
        this->cur_V.x = this->cur_V.x + this->prev_acc.x * 0.01;
        this->cur_V.y = this->cur_V.y + this->prev_acc.y * 0.01;
        this->cur_V.z = this->cur_V.z + this->prev_acc.z * 0.01;

        std::cout << "x :" << this->prev_V.x << std::endl;
        std::cout << "y :" << this->prev_V.y << std::endl;
        std::cout << "z :" << this->prev_V.z << std::endl;

        vglobal.x = cos(this->prev_dir.y) * cos(this->prev_dir.z) * this->prev_V.x + 
                    (-cos(this->prev_dir.y) * sin(this->prev_dir.z) * this->prev_V.y) +
                    sin(this->prev_dir.y) * this->prev_V.z;

        vglobal.y = (cos(this->prev_dir.x) * sin(this->prev_dir.z) + sin(this->prev_dir.x) * sin(this->prev_dir.y) * cos(this->prev_dir.z)) * this->prev_V.x +
                    (cos(this->prev_dir.x) * cos(this->prev_dir.z) - sin(this->prev_dir.x) * sin(this->prev_dir.y) * sin(this->prev_dir.z)) * this->prev_V.y + 
                    (-sin(this->prev_dir.x) * cos(this->prev_dir.y)) * this->prev_V.z;

        vglobal.z = (sin(this->prev_dir.x) * sin(this->prev_dir.z) - cos(this->prev_dir.x) * sin(this->prev_dir.y) * cos(this->prev_dir.z)) * this->prev_V.x +
                    (sin(this->prev_dir.x) * cos(this->prev_dir.z) + cos(this->prev_dir.x) * sin(this->prev_dir.y) * sin(this->prev_dir.z)) * this->prev_V.y + 
                    (cos(this->prev_dir.x) * cos(this->prev_dir.y)) * this->prev_V.z;

        this->result.x = this->result.x + vglobal.x * 0.01;
        this->result.y = this->result.y + vglobal.y * 0.01;
        this->result.z = this->result.z + vglobal.z * 0.01;

        save_to_prev();

        std::cout << "x :" << this->result.x << std::endl;
        std::cout << "y :" << this->result.y << std::endl;
        std::cout << "z :" << this->result.z << std::endl;
        exit(0);
    }

}
struct Point Info::hist(int idx)
{
    return this->trajectory[idx];
}

size_t Info::histsize()
{
    return this->trajectory.size();
}

void Info::show_trajectory()
{
    if (this->trajectory.size() > 5)
    {
        for(std::vector<struct Point>::iterator it = this->trajectory.end() - 5; it != this->trajectory.end(); it++)
        {
            std::cout << (*it).x << " " << (*it).y << std::endl;
        }
    }
}

void Info::compute(int step)
{

    struct Point  mesure;
    struct Point gain;

    if (step == 0)
    {
        this->result.x = this->true_pos.x;
        this->result.y = this->true_pos.y;
        this->result.z = this->true_pos.z;

        this->cur_V.x = this->speed0 /3.6;
        this->cur_V.y = 0;
        this->cur_V.z = 0;

        save_to_prev();
        save_point_trajectory(this->true_pos);
        return;
    }

    this->cur_V.x = this->cur_V.x + this->prev_acc.x * 0.01;
    this->cur_V.y = this->cur_V.y + this->prev_acc.y * 0.01;
    this->cur_V.z = this->cur_V.z + this->prev_acc.z * 0.01;


    this->result.x = this->result.x + this->prev_V.x * 0.01 + this->prev_acc.x * 0.0001 /2;
    this->result.y = this->result.y + this->prev_V.y * 0.01 + this->prev_acc.y * 0.0001 /2;
    this->result.z = this->result.z + this->prev_V.z * 0.01 + this->prev_acc.z * 0.0001 /2;

    save_to_prev();


    if (step % 300 == 0)
    {
        
        //std::cerr << "dpred " << this->dpred.x << " " << this->dpred.y << " " <<this->dpred.z << std::endl;
    
        gain.x = this->dpred.x/(this->dpred.x + this->dmesure);
        gain.y = this->dpred.y/(this->dpred.y + this->dmesure);
        gain.z = this->dpred.z/(this->dpred.z + this->dmesure);


        dpred.x = (this->dpred.x * this->dmesure) / (this->dpred.x + this->dmesure);
        dpred.y = (this->dpred.y * this->dmesure) / (this->dpred.y + this->dmesure);
        dpred.z = (this->dpred.z * this->dmesure) / (this->dpred.z + this->dmesure);


        //std::cerr << "gain " << gain.x << " " << gain.y << " " << gain.z << std::endl;
        //std::cerr << "dpred " << this->dpred.x << " " <<this->dpred.y << " " <<this->dpred.z << std::endl;

        mesure.x = this->cur_pos.x - this->result.x;
        mesure.y = this->cur_pos.y - this->result.y;
        mesure.z = this->cur_pos.z - this->result.z;
        
        //8652
        //0.35 gain
        this->result.x = this->result.x + gain.x * (mesure.x);
        this->result.y = this->result.y + gain.y * (mesure.y);
        this->result.z = this->result.z + gain.z * (mesure.z);

        //this->true_pos.x = gain.x;
        // this->result.x = this->result.x + 0.6 * (mesure.x);
        // this->result.y = this->result.y + 0.6 * (mesure.y);
        // this->result.z = this->result.z + 0.6 * (mesure.z);
    }

    save_point_trajectory(this->result);

}

void Info::save_point_trajectory(struct Point p)
{
    this->trajectory.push_back(p);
}

void Info::save_to_prev()
{
    this->prev_V.x = this->cur_V.x;
    this->prev_V.y = this->cur_V.y;
    this->prev_V.z = this->cur_V.z;

    this->prev_acc.x = this->cur_acc.x;
    this->prev_acc.y = this->cur_acc.y;
    this->prev_acc.z = this->cur_acc.z;

    this->dpred.x += incpred.x;
    this->dpred.y += incpred.y;
    this->dpred.z += incpred.z;

    this->prev_dir.x = this->cur_dir.x;
    this->prev_dir.y = this->cur_dir.y;
    this->prev_dir.z = this->cur_dir.z;

}

std::string Info::response()
{
    std::stringstream ss;

    ss << std::fixed;
    ss << std::setprecision(19) << this->result.x << " " << this->result.y << " " << this->result.z;

    return ss.str(); 
}