#include<stdio.h>
#include<string.h>
#include<getopt.h>
#include<curl/curl.h>
#include<stdlib.h>

#define OK 0

// function for usage info
void usage(void) {
				printf("\n[HELP] Usage: ./test [URL] 'url' [VERB] 'content'\n\n");
				printf("----------------------- HELP OPTIONS -----------------------------------------\n\n");
				printf(" FLAGS		ARGUMENTS	EXAMPLE ARG FORMAT			SUMMARY\n");
				printf("------------------------------------------------------------------------------\n");
				printf("-h/--help		N/A			N/A						help options\n");
				printf("-g/--get 		N/A			N/A						[VERB] http get option\n");
				printf("-o/--post	'content'	'Post this stuff.'			[VERB] http post option\n");
				printf("-p/--put 	'content'	'Put this stuff.'			[VERB] http put option\n");
				printf("-d/--delete	'content'	'Delete this stuff/'		[VERB] http delete option\n");
				printf("-u/--url	'url'		'http://www.cnn.com'		[URL] http url of server\n");
				printf("						'http://localhost:PORT' \n");
				printf("						'http://IPADDRESS:PORT'\n\n");
				printf("------------------------------------------------------------------------------\n");
				printf("			Example USAGE\n");
				printf("------------------------------------------------------------------------------\n");
				printf("HTTP GET -		./hw --url 'http://httpbin.org/get' --get\n" );
				printf("HTTP POST -		./hw --url 'http://httpbin.org/post' --post 'Post this content'\n");
				printf("HTTP PUT -		./hw --url 'http://httpbin.org/put' --put 'Put this content'\n");
				printf("HTTP DELETE -	./hw --url 'http://httpbin.org/delete' --delete 'Delete this content\n\n");	
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
				url = optarg;
				printf("[HTTP URL] url: %s\n", url);
				break;
			case 'g':
				printf("[HTTP GET]\n");
				curl_get(url, curl, res);
				break;
			case 'o':
				content = optarg;
				printf("[HTTP POST] content: %s\n", content);
				curl_post(url, content, curl, res);
				break;
			case 'p':
				content = optarg;
				printf("[HTTP PUT] content: %s\n", content);
				curl_put(url, content, curl, res);
				break;
			case 'd':
				content = optarg;
				printf("[HTTP DELETE] content: %s\n", content);
				curl_delete(url, content, curl, res);
				break;
			case '?':
				// no command line options, error already raised by getopt_long 
				usage();
				break;
			default:
				abort();
		}
	}
	
	return OK;
}
