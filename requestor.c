/***************************************************************************************************************************/
// University of New Mexico - School of Engineering
// Department of Electical and Computer Engineering
// ECE 531: Introduction to IoT - Dr. Lamb, Dr. Papapanagioutou
// Assignment: Network Communication on ARM
// Author: Panayioti Kitsos - Date: 4/18/18
// Compile with:
// 			Host machine: make all
//			QEMU machine: make -f makefile-arm
/***************************************************************************************************************************/
#include<stdio.h>
#include<string.h>
#include<getopt.h>
#include<curl/curl.h>
#include<stdlib.h>

#define OK 0

// function for usage info
void usage(void) {
				printf("\nUSAGE: ./test [URL] '<url>' [VERB] '<content>'                           \n\n");
				printf("%40s\n", "HELP MENU");				
				printf("---------------------------------------------------------------------------------------------\n");
				printf("%-15s%-15s%-35s%-25s\n","FLAGS", "ARGUMENTS", "EXAMPLE ARG FORMAT", "SUMMARY");
				printf("---------------------------------------------------------------------------------------------\n");
				printf("%-15s%-15s%-35s%-25s\n","-h/--help", "N/A", "N/A", "help and usage options");
				printf("%-15s%-15s%-35s%-25s\n","-g/--get", "N/A", "N/A", "[VERB] http get option");
				printf("%-15s%-15s%-35s%-25s\n","-o/--post", "'content'", "'Post this text'", "[VERB] http post option");
				printf("%-15s%-15s%-35s%-25s\n","-p/--put", "'content'", "'Put this text'", "[VERB] http put option");
				printf("%-15s%-15s%-35s%-25s\n","-d/--delete", "'content'", "'Delete this text'", "[VERB] http delete option");
				printf("%-15s%-15s%-35s%-25s\n","-u/--url", "'url'", "'http://localhost:8080'", "[URL] http url option");
				printf("%50s\n", "'http://www.cnn.com'");
				printf("%53s\n\n", "'http://IPADDRESS:PORT'");
				printf("%44s\n", "EXAMPLE USAGE");				
				printf("---------------------------------------------------------------------------------------------\n");
				printf("%-20s%-30s\n","HTTP REQUEST", "COMMAND");
				printf("---------------------------------------------------------------------------------------------\n");
				printf("%-20s%-30s\n","HTTP GET", "./hw --url 'http://httpbin.org/get' --get");
				printf("%-20s%-30s\n","HTTP POST", "./hw --url 'http://httpbin.org/post' --post 'This text gets posted!'");
				printf("%-20s%-30s\n","HTTP PUT", "./hw --url 'http://httpbin.org/put --put 'This text gets put!'");
				printf("%-20s%-30s\n","HTTP DELETE", "./hw --url 'http://httpbin.org/delete' --delete 'This text gets deleted!'");
}

// function for HTTP GET
void curl_get(char *url, CURL *curl, CURLcode res) {
	long http_code = 0;
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			fprintf(stderr, "[CURL] Could not execute HTTP GET: %s\n", curl_easy_strerror(res));
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		printf("[HTTP CODE]: %ld\n", http_code);
		curl_easy_cleanup(curl);
	}
}

// function for HTTP PUT
void curl_put(char *url, char *content, CURL *curl, CURLcode res) {
	long http_code = 0;
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			fprintf(stderr, "[CURL] Could not execute HTTP PUT: %s\n", curl_easy_strerror(res));
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		printf("[HTTP CODE]: %ld\n", http_code);
		curl_easy_cleanup(curl);
	}
}

// function for HTTP POST
void curl_post(char *url, char *content, CURL *curl, CURLcode res) {
	long http_code = 0;
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(content));
		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			fprintf(stderr, "[CURL] Could not execute HTTP POST: %s\n", curl_easy_strerror(res));
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		printf("[HTTP CODE]: %ld\n", http_code);
		curl_easy_cleanup(curl);
	}
}

// function for HTTP DELETE
void curl_delete(char *url, char *content, CURL *curl, CURLcode res) {
	long http_code = 0;
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			fprintf(stderr, "[CURL] Could not execute HTTP DELETE: %s\n", curl_easy_strerror(res));
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		printf("[HTTP CODE]: %ld\n", http_code);
		curl_easy_cleanup(curl);
	}
}

// main function using getopt for accepting command line options
int main(int argc, char **argv) {
	
	int c;
	int gflag = 0, uflag = 0, oflag = 0, pflag = 0, dflag = 0;
	char *content, *url; 
	CURL *curl;
	CURLcode res;

	while(1) {
		static struct option long_options[] = 
		{
			{"help", no_argument, 0, 'h'},
			{"get", no_argument, 0, 'g'},
			{"url ", required_argument, 0 , 'u'},
			{"post", required_argument, 0, 'o'},
			{"put", required_argument, 0, 'p'},
			{"delete", required_argument, 0, 'd'},
			{0,0,0,0}
		};
	
		// getopt stores option index here
		int long_index = 0;

		c = getopt_long_only(argc, argv, "hgu:o:p:d:", \
			long_options, &long_index);

		// raise usage error if no command line options entered
		if(argc <= 1) {
			printf("WARNING: Missing command line options!\n");
			usage();
			exit(-1);
		}

		// detect end of options	
		if(c == -1)
			break;

		switch(c) {
			case 'h':
				usage();
				break;
			case 'u':
				if(uflag >= 1) {
					printf("WARNING: Only URL allowed one url!\n");
					usage();
					exit(1);
				} else {
					uflag++;
					//printf("[HTTP URL] url: %s\n", url);
					url = optarg;
					break;
				}
			case 'g':
				if (gflag >= 1) {
					printf("WARNING: Only one VERB option allowed!\n");
					usage();
					exit(1);
				} else {
					gflag++;
					pflag++;
					oflag++;
					dflag++;
					printf("[HTTP GET]\n");
					curl_get(url, curl, res);
					break;	
				}
				//printf("[HTTP GET]\n");
				//curl_get(url, curl, res);
				//break;
			case 'o':
				if(oflag) {
					printf("WARNING: Only one VERB option allowed!\n");
					usage();
					exit(1);
				} else {
					oflag++;
					gflag++;
					pflag++;
					dflag++;
					content = optarg;
					printf("[HTTP POST] content: %s\n", content);
					curl_post(url, content, curl, res);
					break;
				}
			case 'p':
				if(pflag >= 1) {
					printf("WARNING: Only one VERB option allowed!\n");
					usage();
					exit(1);
				} else {
					pflag++;
					oflag++;
					gflag++;
					dflag++;
					content = optarg;
					printf("[HTTP PUT] content: %s\n", content);
					curl_put(url, content, curl, res);
					break;
				}
			case 'd':
				if(dflag >= 1) {
					printf("WARNING: Only one VERB option allowed!\n");
					usage();
					exit(1);
				} else {
					dflag++;
					oflag++;
					gflag++;
					dflag++;
					content = optarg;
					printf("[HTTP DELETE] content: %s\n", content);
					curl_delete(url, content, curl, res);
					break;
				}
			case '?':
				// invalid command line options, error already raised by getopt_long 
				printf("WARNING: Invalid command line options!\n");
				usage();
				break;
			default:
				abort();
		}
	}
	
	return OK;
}
