/*
 * HTTPClient.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: edi
 */

#include "HTTPClient.h"

#include <curl/curl.h>


using namespace std;


HTTPClient::HTTPClient() {
	curl_global_init(CURL_GLOBAL_ALL);
}

HTTPClient::~HTTPClient() {
	// TODO Auto-generated destructor stub
}

void HTTPClient::sendMessage(string message)
{
	CURL* myHandle;
	CURLcode result;
	string url = "http://localhost:8000/"+ message;
	myHandle = curl_easy_init();
	curl_easy_setopt(myHandle, CURLOPT_URL, url.c_str());
	result = curl_easy_perform(myHandle);
	curl_easy_cleanup(myHandle);
	printf("\nHTTPClient::sendMessage: sent: '%s'\n -> %d", url.c_str(), result);

}







