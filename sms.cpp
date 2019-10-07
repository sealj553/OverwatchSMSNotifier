//#define DONTSEND

#include "sms.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <sstream>
#include <fstream>

#include <curl/curl.h>
#include <yaml-cpp/yaml.h>

using std::string;
using std::stringstream;
using std::ifstream;
using std::cout;
using std::endl;

const int BUFSIZE = 512;
char writeBuf[BUFSIZE] = { 0 };

string account_sid;
string auth_token;
string from_number;
string to_number;
string message;

string url;

#define LOAD_PARAM(name, config) name = config[ #name ].as<std::string>();

void load_config() {
	try {
		YAML::Node config = YAML::LoadFile("config.yaml");

		LOAD_PARAM(account_sid, config);
		LOAD_PARAM(auth_token, config);
		LOAD_PARAM(from_number, config);
		LOAD_PARAM(to_number, config);
		LOAD_PARAM(message, config);
	}
	catch (...) {}
}

void sms_init() {
	load_config();

	url += "https://api.twilio.com/2010-04-01/Accounts/";
	url += account_sid;
	url += "/Messages.json";

	snprintf(writeBuf, BUFSIZE, "To=%s&From=%s&Body=%s", to_number.c_str(), from_number.c_str(), message.c_str());
}

void sms_send() {
#ifdef DONTSEND
	cout << "fake sent" << endl;
	return;
#endif
	CURL* curl = curl_easy_init();
	if (!curl) {
		cout << "Unable to initialize curl" << endl;
		return;
	}
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_CAINFO, "./cacert.pem");
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_USERNAME, account_sid.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, auth_token.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, writeBuf);

	cout << "using "        << url         << endl;
	cout << "account_sid: " << account_sid << endl;
	cout << "auth_token: "  << auth_token  << endl;
	cout << "message: "     << message     << endl;
	cout << "from_number: " << from_number << endl;
	cout << "to_number: "   << to_number   << endl;

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		cout << "Operation failed: " << res << endl;
		curl_easy_cleanup(curl);
		return;
	}

	cout << "Sent" << endl;
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	cout << "Response: " << http_code << endl;

	curl_easy_cleanup(curl);
}
