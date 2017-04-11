#pragma once


class Timer
{
private:
	LARGE_INTEGER __StartTime, __LastTime, __EndTime, __TimeFreq;
	const char* __processName;
public:
	Timer();
	explicit Timer(const char* processName);
	/// Refresh time
	void refresh_time();
	/// Record current time
	void record_current_time();
	/// Get the operation time from the last recorded moment
	double get_operation_time();
	/// Print the operation time from the last recorded moment
	void log_operation_time();
	/// Print the operation time from the last recorded moment with a given name
	void log_operation_time(const char* operationName);
	/// Get the total time from the beginning
	double get_total_time();
	/// Print the total time from the beginning
	void log_total_time();
	/// Print the total time from the beginning to the last recorded moment
	void log_sub_total_time() const;
};

using TTimer = Timer;
using PTimer = std::shared_ptr<Timer>;
