#ifndef TIME_DISTRIBUTION_H
#define TIME_DISTRIBUTION_H

#include <random>

typedef double Time;

class TimeDistribution
{
	static std::default_random_engine generator;
	static std::uniform_real_distribution<Time> serviceUniformDistr;
	static std::exponential_distribution<Time> serviceExpDistr;
	static std::exponential_distribution<Time> timeOutDistr;
	static std::normal_distribution<Time> thinkDistr;
	static std::uniform_real_distribution<float> serviceProbDistr;

	static Time thinkTimeMean;
	static Time thinkTimeVariance;
	static Time timeOutMinm;
	static Time timeOutExpMean;
	static Time serviceTimeConst;
	static Time serviceTimeUniformMin;
	static Time serviceTimeUniformMax;
	static Time serviceTimeExpMean;
	static float serviceProb1;
	static float serviceProb2;
	TimeDistribution();

public:
	static void setSeed(int seed);
	static void setThinkTimeDistribution(Time mean, Time variance);
	static void setTimeOutDistribution(Time minm, Time expParam);
	static void setServiceTimeDistribution(Time const, Time uniformMin, Time uniformMax, Time expParam, float prob1, float p2);
	static Time getThinkTime();
	static Time getTimeOutTime();
	static Time getServiceTime();
};

#endif
