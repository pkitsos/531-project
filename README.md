# IoT Thermostat Project
This is a repository for my IoT Thermostat project as part of ECE 531: Introducion to IoT.

In this project, I setup a linux VM running Ubuntu for my host x86_64 development environment using a cross compiler. I installed buildroot and configured an ARM based embedded linux machine to simulate my client thermostat device using QEMU.

__Network and Communication on ARM__  
To set up my network communication on the ARM machine, I used curl and the libcurl library to implement a C program that performs basic HTTP requests like GET, PUT, POST, and DELETE. This program utilizes getopt (provided by busybox) to parse the command line for both url and verb flags and their required arguments. The program also returns the http request code for each action and takes into account error checking.   
Program name: requestor.c   
Compile with: make -f makefile-arm2   

__Daemon__  
I implemented a C program that spawns a daemon process using fork() and effectively logs the time in second intervals to /var/log/messages via syslog. The program takes into account error checking when forking and utilizes a bash script for signal handling. In addition, the daemon begins at startup.  
C daemon program: /usr/sbin/sampled.c  
Daemon bash script: /etc/init.d/S80sampled  

__Cloud Integration__ 
Now that the client side is done, I am working on implementing my host server on an AWS EC2 instance. From here, I will be configuring a REST API to allow me to interact between host and client using HTTP requests.

__Thermocouple__   
This module was provided to us by our professor, Dr. Lamb. The code effectively acts as the underlying thermocouple that we will be using in the configuration of our project.

__Hardening__   
As an assignment, students were tasked to explore a variety of firmware files for reverse engineering and security purposes. In this case, I was tasked with the analysis of several versions of firmware for the TP-LINK NC220 IP camera. To perform this analysis, I used tools like binwalk and radare2 to learn more about the system and identify low hanging fruit that could be exploited. In turn, the knowledge aquired from this assignment was used to help me harden my thermostat project.
