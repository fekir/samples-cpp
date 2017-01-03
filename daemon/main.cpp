

#include "daemon.hpp"
#include "signals.hpp"

// posix
#include <syslog.h>
#include <unistd.h> // optarg
#include <getopt.h> // for getopt_long

// libc
#include <csignal>
#include <cassert>

// std
#include <fstream> // read config file
#include <iostream> // read config file
#include <string>

struct config{
	// put here all configurations of the service
	unsigned int delay = 0;
	std::string conf_file_name;
};

const char daemon_name[] = "my_daemon";

std::pair<bool, config> read_conf_file(const std::string& conf_file_name)
{
	config cfg;
	std::fstream myfile(conf_file_name, std::ios_base::in);
	unsigned int a;
	bool ok = false;
	if (myfile >> a)
	{
		ok = true;
		cfg.delay = a;
	}

	return {ok,cfg};
}



int main(const int argc, char* const argv[])
{
	try{
		const option options[] = {
		    {"config_file", required_argument, nullptr, 'c'},
		    {"help", no_argument, nullptr, 'h'},
		    {"daemon", no_argument, nullptr, 'd'},
		    {"pid_file", required_argument, nullptr, 'p'},
		    {nullptr, 0, 0, 0}
		};
		int value = 0;
		int option_index = 0;
		std::string pid_file_name;
		bool daemonize = false; // easier to debug since it avoid forking

		config conf;

		// FIXME: process all command line arguments
		while ((value = getopt_long(argc, argv, "c:p:d:h", options, &option_index)) != -1) {
			switch (value) {
				case 'c':
					conf.conf_file_name = optarg;
					break;
				case 'l':
					break;
				case 'p':
					pid_file_name = optarg;
					break;
				case 'd':
					daemonize = true;
					break;
				case 'h':
				case '?':
					// show help and exit
					return EXIT_SUCCESS;
				default:
					std::cerr << "unrecognized value: " << static_cast<char>(value) << "\n";
					return EXIT_FAILURE;
			}
		}

		// When daemonizing is requested at command line.
		if(daemonize){
			::daemonize();
		}
		locked_file lf;

		if(!pid_file_name.empty()){
			lf = create_pid_file(pid_file_name);
		}

		// Open system log and write message to it
		openlog(daemon_name, LOG_PID|LOG_CONS, LOG_DAEMON);
		const syslog_closer cl;
		syslog(LOG_INFO, "%s has been started", daemon_name);

		// Read configuration from config file (before registering signal for reloading configuration, otherwise update global flag)
		auto res = read_conf_file(conf.conf_file_name);
		if(res.first){
			conf = res.second;
		}else{
			syslog(LOG_ERR, "error while loading configuration");
			return EXIT_FAILURE;
		}

		// register signals
		signal(SIGINT, handle_SIGINT);
		signal(SIGHUP, handle_SIGHUP);
		syslog(LOG_INFO, "registered signals");

		// This global variable can be changed in function handling signal
		g_sig_int_flag = sig_int_stat::not_received;

		// Never ending loop of server
		int counter = 0;
		while (g_sig_int_flag == sig_int_stat::not_received) {
			// check if we need to reload configuration
			if(g_reload_conf_flag == reload_conf_stat::reload){
				const update_flag up(g_reload_conf_flag, reload_conf_stat::reloading, reload_conf_stat::done);
				const auto conf_tmp = read_conf_file(conf.conf_file_name);
				if(conf_tmp.first){
					syslog(LOG_INFO, "configuration reloaded successfully");
					conf = conf_tmp.second;
				}else{
					syslog(LOG_ERR, "error while reloading configuration");
				}
			}

			// do something useful here
			{
				syslog(LOG_INFO, "daemon running (%d)", counter++);
			}

			// Real server should use select() or poll() for waiting at async event.
			// Note: sleep() is interrupted, when signal is received.
			sleep(conf.delay);

		}

		assert(g_sig_int_flag == sig_int_stat::received && "we should have received a sig_int signal!");
		syslog(LOG_INFO, "stopping daemon");


		// Write system log and close it.
		syslog(LOG_INFO, "Stopped %s", daemon_name);

		return EXIT_SUCCESS;
	}catch(const std::exception& ex){
		std::cerr << "Failed with ex: " << ex.what() << "\n";
		return EXIT_FAILURE;
	}catch(...){
		std::cerr << "Failed with unknown exception\n";
		throw; // rethrow, we may get some further information from the runtime
	}
}