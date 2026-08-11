#ifndef udpserver_HPP
#define udpserver_HPP
#include <string.h>
#include <iostream>
#include <string>

bool not_finished(char buf[]);
bool success(char buf[]);
void parse_arguments(int argc, char *argv[], struct Point *sig);
int r10(double nb);

#endif
