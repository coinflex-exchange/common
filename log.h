#include <iostream>
#include <memory>
#include <sstream>

class Log;

class LogStream : public std::stringstream {
	friend class Log;

private:
	std::ostream * const stream_ptr;

public:
	// [C++11] LogStream(LogStream &&move) : std::stringstream(std::move(move)), stream_ptr(move.stream_ptr) { }
	LogStream(LogStream &&move) : stream_ptr(move.stream_ptr) {
		this->str(move.str());
	}
	~LogStream();

private:
	LogStream(std::ostream *stream_ptr, const char label[]);
	LogStream(const LogStream &) = delete;
	LogStream & operator = (const LogStream &) = delete;

};


class Log {
	friend class LogStream;

public:
	enum Level {
		SILENT, FATAL, ERROR, WARN, INFO, DEBUG, TRACE
	};

public:
	std::ostream * const trace_stream_ptr;
	std::ostream * const debug_stream_ptr;
	std::ostream * const info_stream_ptr;
	std::ostream * const warn_stream_ptr;
	std::ostream * const error_stream_ptr;
	std::ostream * const fatal_stream_ptr;

private:
	static const char trace_label[];
	static const char debug_label[];
	static const char info_label[];
	static const char warn_label[];
	static const char error_label[];
	static const char fatal_label[];

public:
	Log(Level level) : trace_stream_ptr(level < TRACE ? nullptr : &std::clog), debug_stream_ptr(level < DEBUG ? nullptr : &std::clog), info_stream_ptr(level < INFO ? nullptr : &std::clog), warn_stream_ptr(level < WARN ? nullptr : &std::clog), error_stream_ptr(level < ERROR ? nullptr : &std::clog), fatal_stream_ptr(level < FATAL ? nullptr : &std::clog) { }
	Log(std::ostream *trace_stream_ptr, std::ostream *debug_stream_ptr, std::ostream *info_stream_ptr, std::ostream *warn_stream_ptr, std::ostream *error_stream_ptr, std::ostream *fatal_stream_ptr) : trace_stream_ptr(trace_stream_ptr), debug_stream_ptr(debug_stream_ptr), info_stream_ptr(info_stream_ptr), warn_stream_ptr(warn_stream_ptr), error_stream_ptr(error_stream_ptr), fatal_stream_ptr(fatal_stream_ptr) { }
	bool trace_enabled() const { return trace_stream_ptr; }
	bool debug_enabled() const { return debug_stream_ptr; }
	bool info_enabled() const { return info_stream_ptr; }
	bool warn_enabled() const { return warn_stream_ptr; }
	bool error_enabled() const { return error_stream_ptr; }
	bool fatal_enabled() const { return fatal_stream_ptr; }
	LogStream trace() { return LogStream(trace_stream_ptr, trace_label); }
	LogStream debug() { return LogStream(debug_stream_ptr, debug_label); }
	LogStream info() { return LogStream(info_stream_ptr, info_label); }
	LogStream warn() { return LogStream(warn_stream_ptr, warn_label); }
	LogStream error() { return LogStream(error_stream_ptr, error_label); }
	LogStream fatal() { return LogStream(fatal_stream_ptr, fatal_label); }

};