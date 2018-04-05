#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "Config/ConfigFile.h"
using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
void executeError();
/*
 ./PullApiData -[name] [YYYYMMDD] [hhmm]
*/

int main(int argc, char* argv[])
{
	string name; // 받아올 api 이름
	ConfigFile* config;
	vector<string> var;
	if(argc < 1)
	{
		executeError();
	}
	else
	{
		name = argv[1];
		name.erase(0);
	}
	// conf 파일에서 key와 변수 갯수를 가져옴
	try
	{
		config = new ConfigFile("conf/" + name + ".conf");
	}
	catch(ConfigFile::file_not_found &e)
	{
		cout << "name Error" << endl;
 	}
	string key;
	int numOfVar = 0;
	key = config->read<string>("KEY");
	numOfVar = config->read<int>("NUMOFVAR");

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "base_date=20180402&base_time=0600&nx=60&ny=127&numOfRows=10&pageSize=10&pageNo=1&startPage=1&_type=xml&serviceKey=aq%2Bd7pEryGFmGFAAIFv8VQps%2FF5YNIGe4RZX%2F2SW4h1%2BGHoWs6c4M9QptIPsQPZ2yHhm5iBOnoKKS89LJtlDNA%3D%3D");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		std::cout << readBuffer << std::endl;
	}
	return 0;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    *((std::string*)userp) += "\n";
    return size * nmemb;
}

void executeError()
{
	cout <<
			"ERROR!" << endl <<
			"-FORECAST_GRIB YYYYMMDD hhmm" << endl;
}
