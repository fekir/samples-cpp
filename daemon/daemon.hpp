#ifndef FEK_DAEMONIZE_HPP
#define FEK_DAEMONIZE_HPP

// posix
#include <unistd.h> // fork, close, getpid, write, ...
#include <syslog.h> // syslog closer
#include <signal.h> // sig_atomic_t

#include <fcntl.h> // open pid file
#include <sys/stat.h> // umask

// libc
#include <cerrno> // fork on failure writes error on errno
#include <cassert>
#include <cstring> // strerror

// std
#include <limits>
#include <stdexcept>
#include <string>

// from c++17
template <typename T>
const T& myclamp(const T& n, const T& lower, const T& upper) {
	return std::max(lower, std::min(n, upper));
}


struct update_flag{
	volatile sig_atomic_t& flag; // common signature for flag in signal handler
	sig_atomic_t val_out;
	update_flag(volatile sig_atomic_t& flag_, sig_atomic_t val_in_, sig_atomic_t val_out_) : flag(flag_), val_out(val_out_){
		flag = val_in_;
	}
	update_flag(volatile sig_atomic_t& flag_, sig_atomic_t val_out_) : flag(flag_), val_out(val_out_){}
	~update_flag(){
		flag = val_out;
	}
};

inline std::string errno_to_string(const int error){
	// for some reason, I'm unable to pick the right strerror_r -> using the not thread safe variant
	const std::string err = strerror(error);
/*
	std::string err(32, '\0');
	auto res = strerror_r(error, &err.at(0), err.size());
	while(res == ERANGE){
		err.resize(err.size()*2);
		res = strerror_r(error, &err.at(0), err.size());
	}
	assert( err != EINVAL);
	err = err.c_str(); // remove trailing '\0'
*/
	return err;
}

// terminates parent with EXIT_SUCCESS if ok, terminates with EXIT_FAILURE if not
// FIXME: unclear if I should use exit or _exit
//  _exit does not remove tmpfiles, exit does...
inline void fork_and_close_parent(){
	// Fork off the parent process
	const auto pid = fork();

	// Check if an error occurred
	if (pid < 0) { // fork failed, return error
		const auto err = errno;
		throw std::runtime_error("fork failed with following error:" + errno_to_string(err));
	}

	// Success: Let the parent terminate
	if (pid > 0) { // We are parent: _exit() or exit() ?
		exit(EXIT_SUCCESS);
	}
}


class open_file_descriptor{
public:
	int fd = -1;
	// fd should be a valid and opened file descriptor
	explicit open_file_descriptor(const int fd_ = -1) : fd(fd_){}
	// copy
	open_file_descriptor(const open_file_descriptor&) = delete;
	// move
	open_file_descriptor& operator=(const open_file_descriptor&) = delete;
	open_file_descriptor(open_file_descriptor&& of) noexcept :fd(of.fd){
		of.fd = -1;
	}
	open_file_descriptor& operator=(open_file_descriptor&& other){
		if(this != &other){
			fd = other.fd;
			other.fd = -1;
		}
		return *this;
	}
	~open_file_descriptor(){
		if(fd != -1){
			close(fd);
		}
	}
};


struct syslog_closer{
	syslog_closer() = default;
	syslog_closer(const syslog_closer&) = delete;
	~syslog_closer(){
		closelog();
	}
};

class lock_file{
public:
	int fd = -1;
	int lockres = -1;
	explicit lock_file(const int fd_ = -1) :fd(fd_){
		lockres = lockf(fd_, F_TLOCK, 0);
	}
	static lock_file create_or_throw(const int fd){
		const auto lockres = lockf(fd, F_TLOCK, 0);
		if (lockres< 0) {
			const auto err = errno;
			throw std::runtime_error("lockf failed with following error:" + errno_to_string(err));
		}
		lock_file lf;
		lf.fd = fd;
		lf.lockres = lockres;
		return lf;
	}
	// copy
	lock_file(const lock_file&) = delete;
	lock_file& operator=(const lock_file&) = delete;
	// move
	lock_file(lock_file&& o) noexcept : fd(o.fd), lockres(o.lockres){
		o.fd = -1;
		o.lockres = -1;
	}
	lock_file& operator=(lock_file&& o){
		if(this != &o){
			o.fd = -1;
			o.lockres = -1;
		}
		return *this;
	}

	~lock_file(){
		if(lockres >= 0){
			lockf(fd, F_ULOCK, 0);
		}
	}
};

class locked_file{
public:
	open_file_descriptor openfile_;
	lock_file lock_file_;
	locked_file() = default;
	locked_file(open_file_descriptor openfile__, lock_file lock_file__) :
	    openfile_(std::move(openfile__)), lock_file_(std::move(lock_file__)){
		assert(openfile_.fd == lock_file_.fd);
	}
	// copy
	locked_file(const locked_file&) = delete;
	locked_file& operator=(const locked_file&) = delete;
	// move
	locked_file(locked_file&& o) noexcept : openfile_(std::move(o.openfile_)), lock_file_(std::move(o.lock_file_)){
	}
	locked_file& operator=(locked_file&& o){
		if(this != &o){
			openfile_ = std::move(o.openfile_);
			lock_file_ = std::move(o.lock_file_);
		}
		return *this;
	}
};

inline locked_file create_pid_file(const std::string& pid_file){

	const int fd = open(pid_file.c_str(), O_RDWR|O_CREAT, 0640);
	if(fd == -1){
		const auto err = errno;
		throw std::runtime_error("open failed with following error:" + errno_to_string(err));
	}
	open_file_descriptor file(fd);
	lock_file lf = lock_file::create_or_throw(fd);
	locked_file toreturn(std::move(file), std::move(lf));

	// save PID to file
	const auto pid = std::to_string(getpid()) + "\n";
	const auto write_res = write(fd, pid.c_str(), pid.size());
	if(write_res == static_cast<decltype(write_res)>(-1)){
		const auto err = errno;
		throw std::runtime_error("writing pid to file failed with following error:" + errno_to_string(err));
	}
	return toreturn;

}

inline void daemonize(){

	// Fork off the parent process
	fork_and_close_parent();

	// The child process becomes session leader
	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	// Ignore signal sent from child to parent process
	signal(SIGCHLD, SIG_IGN);

	// Fork off for the second time
	fork_and_close_parent();


	// Set new file permissions
	const auto old_mode = umask(0);
	(void)old_mode;

	// Change the working directory to the root directory
	// or another appropriated directory
	const auto chdir_res = chdir("/");
	if(chdir_res != 0){
		// what to do? we have already forked once -> is it ok to chdir before the first fork?
		const auto err = errno;
		throw std::runtime_error("open failed with following error:" + errno_to_string(err));
	}

	// Close all open file descriptors
	// since close takes a positive int, (and sysconf returns a long) ignore those outside the [0, max_int] range
	constexpr long i_max{std::numeric_limits<int>::max()};
	const auto first_fd = sysconf(_SC_OPEN_MAX);
	const auto first_fd_i = static_cast<int>(myclamp(first_fd, 0l, i_max));
	for (auto fd = first_fd_i; fd > 0; fd--) {
		close(fd);
	}

	// Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2)
	stdin  = fopen("/dev/null", "r");
	stdout = fopen("/dev/null", "w+");
	stderr = fopen("/dev/null", "w+");
}

#endif
