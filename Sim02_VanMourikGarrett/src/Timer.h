#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>

class Timer
{
private:
	double long initialTime;

public:
	void start()
	{
		struct timeval currentTime; 
		gettimeofday( &currentTime, 0 );
    	initialTime = currentTime.tv_sec * 1000000 + currentTime.tv_usec; //
	}

	double long sec()
	{
		struct timeval currentTime; 
		gettimeofday( &currentTime, 0 );
		double long finalTime = currentTime.tv_sec * 1000000 + currentTime.tv_usec;

		return finalTime - initialTime;
	}

	double long mSec()
	{
		struct timeval currentTime; 
		gettimeofday( &currentTime, 0 );
		double long finalTime = currentTime.tv_sec * 1000000 + currentTime.tv_usec;

		return (finalTime - initialTime) / 1e3;
	}

	double long uSec()
	{
		struct timeval currentTime; 
		gettimeofday( &currentTime, 0 );
		double long finalTime = currentTime.tv_sec * 1000000 + currentTime.tv_usec;

		return (finalTime - initialTime) / 1e6;
	}

	// void wait(double long delayTime)
	// {
	// 	while ( mSec() < delayTime ){}
	// }

};



#endif /* TIMER_H */ 