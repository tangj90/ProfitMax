#pragma once


class Timer
{
private:
	LARGE_INTEGER __StartTime, __LastTime, __EndTime, __TimeFreq;
	const char* __processName;
public:
	Timer();
	explicit Timer(const char* processName);
	void refresh_time();
	void record_current_time();
	double get_operation_time();
	void log_operation_time();
	void log_operation_time(const char* operationName);
	double get_total_time();
	void log_total_time();
	void log_sub_total_time() const;
};

using TTimer = Timer;
using PTimer = std::shared_ptr<Timer>;
