/*
 * HTTPClient.h
 *
 *  Created on: Nov 23, 2016
 *      Author: edi
 */

#ifndef SRC_HTTPCLIENT_H_
#define SRC_HTTPCLIENT_H_

#include <string>

using namespace std;

class HTTPClient {
private:
	string mowerURL;

public:
	HTTPClient(string pMowerURL);
	virtual ~HTTPClient();
	void sendMessage(string message);
};


#endif /* SRC_HTTPCLIENT_H_ */
