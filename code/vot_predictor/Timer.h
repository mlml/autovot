/*
 *  Timer.h
 *  new_phoneme_reco
 *
 *  Created by Joseph Keshet on 27/7/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

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