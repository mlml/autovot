/************************************************************************
 Copyright (c) 2014 Joseph Keshet, Morgan Sonderegger, Thea Knowles

This file is part of Autovot, a package for automatic extraction of
voice onset time (VOT) from audio files.

Autovot is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Autovot is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Autovot.  If not, see
<http://www.gnu.org/licenses/>.
************************************************************************/


#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <iomanip>

class Timer {
public:
  void start() {
    getrusage(RUSAGE_SELF, &before);
  }
  void stop() {
    getrusage(RUSAGE_SELF, &after);
  }
  long print(char *message) {
    long msecs_user = ((after.ru_utime.tv_sec - before.ru_utime.tv_sec) * 1000 +
                       (after.ru_utime.tv_usec - before.ru_utime.tv_usec) / 1000);
    long msecs_system = ((after.ru_stime.tv_sec - before.ru_stime.tv_sec) * 1000 +
                         (after.ru_stime.tv_usec -  before.ru_stime.tv_usec) / 1000);
    std::cout << "--> " << std::setw(36) << message << " CPU " << std::setw(4) << (msecs_system + msecs_user) 
    << " msec (user: " << msecs_user << " msec, system: " << msecs_system << " msec)."
    << std::endl;
    
    return (msecs_system + msecs_user);
  }    
  void print_user() {
    long msecs_user = ((after.ru_utime.tv_sec - before.ru_utime.tv_sec) * 1000 +
                       (after.ru_utime.tv_usec - before.ru_utime.tv_usec) / 1000);
    std::cout << "CPU: utilization " << msecs_user << " msec."
    << std::endl;
  }    
protected:
  struct rusage before;
  struct rusage after;
};