#include "c:\work\settings"


#define OTA_DEV_NAME  "LD_SZ"
#define OTA_DEV_IDX   "1"
#define OTA_HOST      ""



class myOTA
{
public:
  myOTA();
  void cyclic(void);


private:
  const char * ssid = WIFI_SSID;
  const char * password = WIFI_PASS;

};


