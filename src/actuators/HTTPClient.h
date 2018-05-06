/*
 * HTTPClient acts as the actuator by sending commands to the mower via http.
 *
 *  Created on: Nov 23, 2016
 *      Author: edi 
 */


#ifndef SRC_HTTPCLIENT_H_
#define SRC_HTTPCLIENT_H_

#include <string>

using namespace std;

namespace robosheep {

class HTTPClient {
private:
	string mowerURL;

public:
	HTTPClient(string pMowerURL);
	virtual ~HTTPClient();
	void sendMessage(string message);
	void sendMotorSpeeds(int motorSpeed1, int motorSpeed2);
};

}  // namespace robosheep

#endif /* SRC_HTTPCLIENT_H_ */
