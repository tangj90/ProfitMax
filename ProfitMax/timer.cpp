#include "stdafx.h"


Timer::Timer()
{
	QueryPerformanceFrequency(&__TimeFreq);
	QueryPerformanceCounter(&__StartTime);
	__LastTime = __StartTime , __EndTime = __StartTime;
	__processName = "Unnamed";
}


Timer::Timer(const char* processName)
{
	QueryPerformanceFrequency(&__TimeFreq);
	QueryPerformanceCounter(&__StartTime);
	__LastTime = __StartTime , __EndTime = __StartTime;
	__processName = processName;
}


void Timer::refresh_time()
{
	QueryPerformanceCounter(&__StartTime);
	__LastTime = __StartTime , __EndTime = __StartTime;
}


void Timer::record_current_time()
{
	__LastTime = __EndTime;
	QueryPerformanceCounter(&__EndTime);
}


double Timer::get_operation_time()
{
	this->record_current_time();
	return (double)(__EndTime.QuadPart - __LastTime.QuadPart) / __TimeFreq.QuadPart;
}


void Timer::log_operation_time()
{
	double duration = this->get_operation_time();
	std::cout << "->Time used (sec): " << duration << std::endl;
}


void Timer::log_operation_time(const char* operationName)
{
	double duration = this->get_operation_time();
	std::cout << "->Time used (sec) for operation [" << operationName << "]: " << duration << std::endl;
}


double Timer::get_total_time()
{
	this->record_current_time();
	return (double)(__EndTime.QuadPart - __StartTime.QuadPart) / __TimeFreq.QuadPart;
}


void Timer::log_total_time()
{
	double duration = this->get_total_time();
	std::cout << "--->Time used (sec) for process [" << __processName << "]: " << duration << std::endl;
}


void Timer::log_sub_total_time() const
{
	double duration = (double)(__EndTime.QuadPart - __StartTime.QuadPart) / __TimeFreq.QuadPart;
	std::cout << "--->Time used (sec) for process [" << __processName << "]: " << duration << std::endl;
}
