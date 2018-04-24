/**********************************************************************************/
// University of New Mexico - School of Engineering
// Department of Electical and Computer Engineering
// ECE 531: Introduction to IoT - Dr. Lamb, Dr. Papapanagiotou
// Assignment: Creating your own Daemon process!
// Author: Panayioti Kitsos - Date: 4/21/18
//
// Description:
//		This program forks a daemon process and logs the time in second intervals to 
//		/var/log/messages via syslog on our ARM based QEMU machine. Currently, the
//		daemon logs time in both short and long formats. Commenting out one or the other
//		makes it easy to switch between both short (time) and long (date & time) formats.
//
// Bash script (S80sampled):
//		location: /etc/init.d/
//		usage: ./S80sampled {start|stop|restart}
// C program daemon (sampled):
//		location: /usr/sbin/
//		compilation: (make -f makefile-arm2)
// 				/home/USER/buildroot-2018.02/output/host/usr/bin/arm-linux-gcc --sysroot=/home/USER/buildroot-2018.02/output/staging  -c sampled.c -o sampled.o
// 				/home/USER/buildroot-2018.02/output/host/usr/bin/arm-linux-gcc --sysroot=/home/USER/buildroot-2018.02/output/staging  -o sampled sampled.o
// Daemon log:
//		location: /var/log/messages
/**********************************************************************************/
#include<errno.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<syslog.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<signal.h>
#include<time.h>
#include<unistd.h>

#define OK 0
#define ERR_FORK 1
#define ERR_SETSID 1
#define ERR_CHDIR 1
#define ERR_WTF 1
#define WORK_DIR "/"

/* Signal handler that gets passed a signal for handling */
static void _signal_handler(const int signal) {
	switch(signal) {
		case SIGHUP:
			break;
		case SIGTERM:
			syslog(LOG_INFO, "received SIGTERM, exiting.");
			closelog();
			exit(OK);
			break;
		default:
			syslog(LOG_INFO, "received unhandled signal");
	}
}

/* Private method for performing daemon work. May need to remove static for external testing*/
static void _do_work(void) {
	/* Log date and time in second intervals via syslog */
	for(int i = 0; true; i++) {
		time_t rawtime;
		struct tm *timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		/* Uncomment for date & time (long format) */
		syslog(LOG_INFO, "time: %s", asctime(timeinfo));
		/* Uncomment for time (short format) */
		syslog(LOG_INFO, "time: %02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		sleep(1);
	}
}

/* Main function for opening log and forking child process */
int main(void) {
	
	/* Open syslog to write directly to the log w/out wait. The log is opened here 
	   in order to register the fork error to syslog */
	openlog("sampled", LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_DAEMON);
	syslog(LOG_INFO, "starting sampled");

	/* Fork child process so we don't take over syslogd | initd */
	pid_t pid = fork();	
	
	if(pid < 0) {
		/******* Fork Error *******/
		/* Fork error. Log to syslog */
		syslog(LOG_ERR, "ERR_FORK: %s", strerror(errno));
		 return ERR_FORK;
	}
	else if(pid == 0) {
		/****** Child Process *******/
		/* Create session and check if we're the process group leader. If not, throw error */
		if(setsid()< -1) {
			syslog(LOG_ERR, "ERR_SETSID: %s", strerror(errno));
			return ERR_SETSID;
		}

		/* We have !console so it's a good idea to close the open file pointers */ 
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	
		/* Set a sane umask for giving everyone read/write permissions for creating files */
		umask(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

		/* Set working directory for easy navigation */
		if(chdir(WORK_DIR) < 0) {
			syslog(LOG_ERR, "ERR_CHDIR: %s", strerror(errno));
			return ERR_CHDIR;
		}

		/* !interactive user session so we control using signals. */
		signal(SIGTERM, _signal_handler);
		signal(SIGHUP, _signal_handler);

		/* Private method for performing daemons responsibilities */
		_do_work();

		/* Shouldn't really be reached. Work method does work until signal is killed, 
		   which then exits the process */
		return ERR_WTF;
	}
	else {
		/****** Parent Process ******/
		/* We only care about the child process so we exit */
		return OK;
	}
	return OK;
}
