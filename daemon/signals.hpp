#ifndef FEK_SIGNALS_HPP
#define FEK_SIGNALS_HPP

// posix
#if !defined(NDEBUG)
#include <unistd.h> // write, STDOUT_FILENO, only for debugging
#endif

// libc
#include <csignal>
#include <cassert>

// std

/// signal handlers
// https://www.securecoding.cert.org/confluence/pages/viewpage.action?pageId=3903
// https://stackoverflow.com/questions/16891019/how-to-avoid-using-printf-in-a-signal-handler

/// reloads configuration:
// https://en.wikipedia.org/wiki/Unix_signal#SIGHUP

enum reload_conf_stat : sig_atomic_t {
	done,      // reset to this status when finished loading configuration
	reload,    // the event loop will reload the cofiguration if this value has been set
	reloading, // intermediary status while loading configuration, used for avoiding to update config when already updating it
};

static volatile sig_atomic_t g_reload_conf_flag = reload_conf_stat::done;
inline void handle_SIGHUP(const int sig){
	assert(sig == SIGHUP && "signal handler not registered corretcly");
	if(sig != SIGHUP){
		return;
	}
#if !defined(NDEBUG)
	const char msg[] = "received SIGHUP signal";
	write(STDOUT_FILENO, msg, sizeof(msg)-1); // strlen is not "signal safe" on posix, 1 is stdout
#endif
	 // do not update if reloading or already set on reload (and therefore not reloaded yet)
	auto new_flag = g_reload_conf_flag;
	new_flag = (new_flag == reload_conf_stat::done) ? reload_conf_stat::reload : new_flag;
	g_reload_conf_flag = new_flag;
}


__sighandler_t orig_sigterm = nullptr;
enum sig_term_stat : sig_atomic_t{
	not_received,
	received,
};
static volatile sig_atomic_t g_sig_int_flag = sig_term_stat::not_received;
inline void handle_SIGTERM(const int sig){
	assert(sig == SIGTERM && "signal handler not registered corretcly");
	if (sig != SIGTERM) {
		return;
	}
#if !defined(NDEBUG)
	const char msg[] = "received SIGINT signal";
	write(STDOUT_FILENO, msg, sizeof(msg)-1); // strlen is not "signal safe" on posix
#endif
	g_sig_int_flag = sig_term_stat::received;
	// reset original signal handler, if for some reason our main loop takes to much time to exit,
	// a second SIGINT will terminate the program
	auto this_sigint = signal(SIGTERM, orig_sigterm);
	(void)this_sigint;
}

#endif
