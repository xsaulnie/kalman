#include <unistd.h>
#include <string.h>
#include <cstdio>
#include <vector>
#include "Info.hpp"
#include <math.h>

// x : 0.15770046217221534, y : 1.9150425098450796, z : 0.5880042523387061
// 0.15770046217221534, y : 1.9150425098450796, z : 0.5880042523387061

//-14978.9271201872343226569 25508.2931977407206431963 -208.6079913849022773320
// x : 1.722334017720641, y : 1.5834824370067508, z : 1.6939691057604023
//x : 1.722334017720641, y : 1.5834824370067508, z : 1.6939691057604023


Info::Info(double accsig, double accpos)
{
    std::cerr << accsig << " " <<accpos << std::endl;
    this->coord[0] = 0;
    this->coord[1] = 0;
    this->coord[2] = 0; 
    this->idx = 0;
    this->choice = '0';
    this->here = 0;
    this->incpred = {.x= accsig, .y=accsig, .z= accsig};
    //this->incpred = {.x = 0.0005+accsig*0.0005, .y = 0.0005+accsig*0.0005, .z= 0.0005+accsig*0.0005};
    //this->incpred= {.x = 0.0001, .y = 0.0001, .z= 0.0001};
    //this->incpred= {.x = 0.0005, .y = 0.0005, .z= 0.0005};
    //this->incpred = {.x = 0.0004, .y = 0.001, .z= 0.001};
    //dpred = {.x = 0.0004*300, .y = 0.001*300, .z = 0.001*300};
    dpred = {.x = 0, .y = 0, .z = 0};
    //this->dmesure = 0.1;
    //this->dmesure = 1;
    this->dmesure = accpos;

    std::cout << accsig << " " << accpos << std::endl;
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
    //std::vector<std::string> tokens;

    pos = buf.find("\n");
    if (save_choice(buf.substr(0, pos)) == '0')
        return;

    buf.erase(0, pos + 1);

    while ((pos = buf.find("\n")) != std::string::npos) {
        //std::cout << "before stof" << buf.substr(0,pos) << std::endl;
        this->coord[this->idx] = stod(buf.substr(0, pos));
        this->idx = (this->idx + 1) % 3;
        //tokens.push_back(token);
        buf.erase(0, pos + 1);
    }

    saving();
    this->choice = '0';
    this->idx = 0;

    // for (unsigned long i = 0 ; i < tokens.size() ; i++)
    // {
    //     std::cout << "Initiate" << tokens[i] << std::endl;
    // }
}

// void Info::add(std::string buf)
// {
//     printf("choice %c\n", this->choice);
//     if (this->choice == '0')
//     {
//         if (buf.find("ACCELERATION") != std::string::npos)
//             this->choice = 'a';
//         else if (buf.find("DIRECTION") != std::string::npos)
//             this->choice = 'd';
//         else if (buf.find("TRUE POSITION") != std::string::npos)
//             this->choice = 'p';
//         else if (buf.find("SPEED")!= std::string::npos)
//             this->choice = 's';
//     }
//     else
//     {
//         writing(buf);
//     }
//     std::cout << buf << std::endl;
// }

void Info::saving()
{
    if (this->choice == 'a')
        save(&this->cur_acc);
    else if (this->choice == 'd')
        save(&this->cur_dir);
    else if (this->choice == 'p')
    {
        this->here = 1;
        save(&this->cur_pos);
    }
    else if (this->choice == 't')
        save(&this->true_pos);
    else if (this->choice == 's')
    {
        this->speed0 = coord[0];
        // this->cur_V.x = this->speed0 /3.6;
        // this->cur_V.y = 0;
        // this->cur_V.z = 0;
    }
}

void Info::save(Point * var)
{
    var->x = coord[0];
    var->y = coord[1];
    var->z = coord[2];
}

// void Info::writing(std::string buf)
// {
//     std::cout << "before stof " << buf << std::endl;
//     coord[idx] = std::stof(buf);
//     this->idx = (this->idx + 1) % 3;
//     if (this->idx == 0 || this->choice == 's')
//     {
//         saving();
//         this->choice = '0';
//     }

// }

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

    //x :-1.19487
    //y :0.134686
    //z :0.5


}

void Info::compute(int step)
{

    struct Point  mesure;
    struct Point gain;
    //double fgain = 0.3;
    //double dpred = 0.0001;
    //double dmesure = 0.1;


    // if (this->here == 1)
    // {
    //     std::cerr << "On step : " << step << std::endl;
    //     this->here = 0;
    // }

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

        // this->result.x = this->true_pos.x;
        // this->result.y = this->true_pos.y;
        // this->result.z = this->true_pos.z;
        // return;

    // else if (step % 300 == 0)
    // {
    //     std::cerr << "Here " << step << std::endl;
    //     this->result.x = this->cur_pos.x;
    //     this->result.y = this->cur_pos.y;
    //     this->result.z = this->cur_pos.z;

    //     this->cur_V.x = this->cur_V.x + this->prev_acc.x * 0.01;
    //     this->cur_V.y = this->cur_V.y + this->prev_acc.y * 0.01;
    //     this->cur_V.z = this->cur_V.z + this->prev_acc.z * 0.01;

    //     this->prev_V.x = this->cur_V.x;
    //     this->prev_V.y = this->cur_V.y;
    //     this->prev_V.z = this->cur_V.z;

    //     this->prev_acc.x = this->cur_acc.x;
    //     this->prev_acc.y = this->cur_acc.y;
    //     this->prev_acc.z = this->cur_acc.z;

    //     return;
    // }

    this->cur_V.x = this->cur_V.x + this->prev_acc.x * 0.01;
    this->cur_V.y = this->cur_V.y + this->prev_acc.y * 0.01;
    this->cur_V.z = this->cur_V.z + this->prev_acc.z * 0.01;


    this->result.x = this->result.x + this->prev_V.x * 0.01 + this->prev_acc.x * 0.0001 /2;
    this->result.y = this->result.y + this->prev_V.y * 0.01 + this->prev_acc.y * 0.0001 /2;
    this->result.z = this->result.z + this->prev_V.z * 0.01 + this->prev_acc.z * 0.0001 /2;

    // double esti = 0;
    // if (step % 301 == 0)
    // {
    //     esti = (backup.x - this->result.x) / 0.01; 
    // }
    save_to_prev();

    //gain = dpred /(dpred +dmesure)
    //new dpred = (1-gain)*dpred + gain*dmesure

    //gain vaud 1 : dmesure 0 (certaine)
    // if (step % 300 == 0)
    // {
    //     this->result.x = this->true_pos.x;
    //     this->result.y = this->true_pos.y;
    //     this->result.z = this->true_pos.z;


    //     save_to_prev();
    //     //exit(0);
    //     return;
    // }

    if (step % 300 == 0)
    {
        
        //if (fgain == 0)
        //{

        std::cerr << "dpred " << this->dpred.x << " " << this->dpred.y << " " <<this->dpred.z << std::endl;
        gain.x = this->dpred.x/(this->dpred.x + this->dmesure);
        gain.y = this->dpred.y/(this->dpred.y + this->dmesure);
        gain.z = this->dpred.z/(this->dpred.z + this->dmesure);

        // dpred.x = (1-gain.x) * this->dpred.x + gain.x*this->dmesure;
        // dpred.y = (1-gain.y) * this->dpred.y + gain.y*this->dmesure;
        // dpred.z = (1-gain.z) * this->dpred.z + gain.z*this->dmesure;
        dpred.x = (this->dpred.x * this->dmesure) / (this->dpred.x + this->dmesure);
        dpred.y = (this->dpred.y * this->dmesure) / (this->dpred.y + this->dmesure);
        dpred.z = (this->dpred.z * this->dmesure) / (this->dpred.z + this->dmesure);

        // dpred.x = 0;
        // dpred.y = 0;
        // dpred.z = 0;
        //}

        // dpred.x = (1-gain.x) * this->dmesure + gain.x * this->dpred.x;
        // dpred.y = (1-gain.y) * this->dmesure + gain.y * this->dpred.y;
        // dpred.z = (1-gain.z) * this->dmesure + gain.z * this->dpred.z;

        std::cerr << "gain " << gain.x << " " << gain.y << " " << gain.z << std::endl;
        std::cerr << "dpred " << this->dpred.x << " " <<this->dpred.y << " " <<this->dpred.z << std::endl;



        mesure.x = this->cur_pos.x - this->result.x;
        mesure.y = this->cur_pos.y - this->result.y;
        mesure.z = this->cur_pos.z - this->result.z;
        

        //0.35 best gain !
        this->result.x = this->result.x + gain.x * (mesure.x);
        this->result.y = this->result.y + gain.y * (mesure.y);
        this->result.z = this->result.z + gain.z * (mesure.z);

        //double newv = (this->result.x - this->backup.x) / (0.01 * 299);
        //std::cerr << "||| backup |||" << newv << "/" << this->prev_V.x << std::endl;

        // this->prev_V.x = (this->result.x - this->backup.x) / (0.01 * 299);
        // this->prev_V.y = (this->result.y - this->backup.y) / (0.01 * 299);
        // this->prev_V.z = (this->result.z - this->backup.z) / (0.01 * 299);

        // this->result.x = this->true_pos.x;
        // this->result.y = this->true_pos.y;
        // this->result.z = this->true_pos.z;
    }

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

    //std::cout << "response is : " << "<<"<< ss.str() << ">>"<< std::endl;
    return ss.str(); 
}