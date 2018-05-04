#include "time_distribution.h"

std::default_random_engine TimeDistribution::generator;
std::uniform_real_distribution<Time> TimeDistribution::serviceUniformDistr;
std::exponential_distribution<Time> TimeDistribution::serviceExpDistr;
std::exponential_distribution<Time> TimeDistribution::timeOutDistr;
std::normal_distribution<Time> TimeDistribution::thinkDistr;
std::uniform_real_distribution<float> TimeDistribution::serviceProbDistr(0.0, 1.0);

Time TimeDistribution::thinkTimeMean;
Time TimeDistribution::thinkTimeVariance;
Time TimeDistribution::timeOutMinm;
Time TimeDistribution::timeOutExpMean;
Time TimeDistribution::serviceTimeConst;
Time TimeDistribution::TimeDistribution::serviceTimeUniformMin;
Time TimeDistribution::TimeDistribution::serviceTimeUniformMax;
Time TimeDistribution::TimeDistribution::serviceTimeExpMean;
float TimeDistribution::TimeDistribution::serviceProb1;
float TimeDistribution::TimeDistribution::serviceProb2;


void TimeDistribution::setSeed(int seed)
{
	generator.seed(seed);
}

void TimeDistribution::setThinkTimeDistribution(Time mean, Time variance)
{
	thinkTimeMean = mean;
	thinkTimeVariance = variance;
	thinkDistr.param(std::normal_distribution<Time>::param_type(thinkTimeMean, thinkTimeVariance));
}

void TimeDistribution::setTimeOutDistribution(Time minm, Time expMean)
{
	timeOutMinm = minm;
	timeOutExpMean = expMean;
	timeOutDistr.param(std::exponential_distribution<Time>::param_type(1.0 / expMean));
}

void TimeDistribution::setServiceTimeDistribution(Time constTime, Time uniformMin, Time uniformMax, Time expMean, float p1, float p2)
{
	serviceTimeConst = constTime;
	serviceTimeUniformMin = uniformMin;
	serviceTimeUniformMax = uniformMax;
	serviceTimeExpMean = expMean;
	serviceProb1 = p1;
	serviceProb2 = p2;

	serviceUniformDistr.param(std::uniform_real_distribution<Time>::param_type(serviceTimeUniformMin, serviceTimeUniformMax));
	serviceExpDistr.param(std::exponential_distribution<Time>::param_type(1.0 / serviceTimeExpMean));
}

Time TimeDistribution::getThinkTime()
{
	Time t;
	while (true)
	{
		t = thinkDistr(generator);
		if (t > 0)
		{
			return t;
		}
	}
}

Time TimeDistribution::getTimeOutTime()
{
	return timeOutMinm + timeOutDistr(generator);
}

Time TimeDistribution::getServiceTime()
{
	float p = serviceProbDistr(generator);
	if (p < serviceProb1)
		return serviceTimeConst;
	else if (p < serviceProb1 + serviceProb2)
		return serviceUniformDistr(generator);
	else
		return serviceExpDistr(generator);
}
