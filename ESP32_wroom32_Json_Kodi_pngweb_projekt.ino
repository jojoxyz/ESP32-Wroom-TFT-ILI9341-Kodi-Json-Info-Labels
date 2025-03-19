
// KODI INFO PANEL LCD - ILI9341 - ESP32 Wroom32

/*
   SD not used in this projekt

   SD_CS     -    5     
   SD0 MISO  -   19

 --LCD--
 
   LED       -   32
   SCK       -   18
   SDI MOSI  -   23
   LCD_RS    -    2
   LCD_RST   -    4
   LCD_CS    -   15

   GND
   VCC  3,3V
 
 */

#define USE_LINE_BUFFER  // Enable for faster rendering

#define WIFI_SSID "++++++++++++" // Wifi Network +++++
#define WIFI_PASS "++++++++++++" // Password +++++

#if defined(USE_ADAFRUIT_GFX)   // touch, not used in this projekt, intgred in png scripts
  #define TFT_CS  5
  #define TFT_DC  26
  #define TFT_RST 27
  #include <Adafruit_ILI9341.h>
  Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
#else
  #include <TFT_eSPI.h>              // Hardware-specific library
  TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
#endif

// Include SPIFFS for PNG from web
#define FS_NO_GLOBALS
#include <FS.h>
#include "SPIFFS.h" // Required for ESP32 only

// Client for kodi json rpc
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "support_functions.h"
#include <WiFi.h>


#define BLACK      0x0000
#define CYAN       0x022fb8
#define DARK_CYAN  0x001a00


void setup()  //--------------------------- SETUP ------------------------------
    {
      Serial.begin(115200);

      tft.begin();
      tft.invertDisplay(1);   // For China Klon
      tft.setRotation(3);
      tft.fillScreen(0);
  
      if (!SPIFFS.begin(true))
        {
Serial.println("SPIFFS Mount Failed");
        return;
        }

      WiFi.begin(WIFI_SSID, WIFI_PASS);
      while (WiFi.status() != WL_CONNECTED)
        {
Serial.print(".");
          delay(1000);
        }
Serial.println("\n WiFi connected.\n");
    }

  int    pela    = 0;
  int    pdur    = 0;
  int    vdur    = 0;
  int    vela    = 0;
  int    mdur    = 0;
  int    mela    = 0;  
  int    bar_val = 0;
  String Logo_tv    ;


void web()  //------------------------------- WEB ------------------------------
    {

      if ((WiFi.status() == WL_CONNECTED))
        {
          HTTPClient http;

         String addr     = ("http://192.168.0.150:8081/jsonrpc?request=");
         String comm     = ("{%22jsonrpc%22:%222.0%22,%22method%22:%22XBMC.GetInfoLabels%22,%22params%22:{%22labels%22:[");

         String label_1  = ("%22System.Date%22,"             );
   //    String label_2  = ("%22VideoPlayer.ChannelName%22," );
   //    String label_3  = ("%22VideoPlayer.Title%22,"       );          
   //    String label_4  = ("%22VideoPlayer.Year%22,"        );          
         String label_5  = ("%22PVR.EpgEventDuration(hh:mm:ss)%22,"    );       
         String label_6  = ("%22PVR.EpgEventElapsedTime(hh:mm:ss)%22," ); // = "VideoPlayer.Time" and "MusicPlayer.Time"
   //    String label_7  = ("%22PVR.EpgEventFinishTime%22,"  );          
         String label_8  = ("%22VideoPlayer.Cover%22,"       );
         String label_9  = ("%22VideoPlayer.Time%22,"        );
         String label_10 = ("%22MusicPlayer.Duration%22"     );  // !!! ,

         String id       = ("]},%22id%22:5}"                 );
          
         String link     = addr + comm + label_1 + label_5 + label_6 + label_8 + label_9 + label_10 + id ;

         http.begin(link); // set full server json rpc address 
          
         int httpCode = http.GET();
         String inRaw = http.getString();


//---- Deserialization + Filter ----  https://arduinojson.org/v6/assistant/#/step1  ----

         StaticJsonDocument<0> filter;
         filter.set(true);

         StaticJsonDocument<512> doc;

         DeserializationError error = deserializeJson(doc, inRaw, DeserializationOption::Filter(filter));

         if (error)
           {
Serial.print("deserializeJson() failed: ");
Serial.println(error.c_str());
             return;
           }

         JsonObject result = doc["result"];

         String res_PVR_EpgEvDur     = result["PVR.EpgEventDuration(hh:mm:ss)"];
         String res_PVR_EpgEvElaTime = result["PVR.EpgEventElapsedTime(hh:mm:ss)"];
   //    String res_PVR_EpgEvFiTime  = result["PVR.EpgEventFinishTime"];
         String res_Sys_Date         = result["System.Date"];
   //    String res_ViPl_ChName      = result["VideoPlayer.ChannelName"];
   //    String res_ViPl_Title       = result["VideoPlayer.Title"];
   //    String res_ViPl_Year        = result["VideoPlayer.Year"];
         String res_ViPl_Cover       = result["VideoPlayer.Cover"];
   //      String res_ViPl_Dur         = result["VideoPlayer.Time"];
   //      String res_MuPl_Dur         = result["MusicPlayer.Duration"];
        
         Logo_tv = res_ViPl_Cover;

//Serial.println(res_ViPl_Cover);
//Serial.println(res_PVR_EpgEvDur);       
//Serial.println(res_PVR_EpgEvElaTime);
//Serial.println(res_ViPl_Dur);       
//Serial.println(res_MuPl_Dur);       

/*

//----------------------- int from string ---------------------------------------
 
//------  VideoPlayerDuration to int ------

         int Index_v1 = res_ViPl_Dur.indexOf(":");
         if (Index_v1 != -1)
           {
             String S_vdur_time_hod = res_ViPl_Dur.substring(0, Index_v1);
             String S_vdur_time_min = res_ViPl_Dur.substring(Index_v1 + 1);
             String S_vdur_time_sec = res_ViPl_Dur.substring(Index_v1 + 4);
             
             int vdur_time_hod = S_vdur_time_hod.toInt();
             int vdur_time_min = S_vdur_time_min.toInt();
             int vdur_time_sec = S_vdur_time_sec.toInt();

             vdur = (vdur_time_hod*60*60) + (vdur_time_min*60) + vdur_time_sec ;

//Serial.print(vdur_time_hod);
//Serial.print(" : ");
//Serial.print(vdur_time_min);
//Serial.print(" : ");
//Serial.println(vdur_time_sec);
//Serial.println(vdur);
           }

//------  MusikPlayerDuration to int ------

         int Index_m1 = res_MuPl_Dur.indexOf(":");
         if (Index_m1 != -1)
           {
             String S_mdur_time_hod = res_MuPl_Dur.substring(0, Index_m1);
             String S_mdur_time_min = res_MuPl_Dur.substring(Index_m1 + 1);
             String S_mdur_time_sec = res_MuPl_Dur.substring(Index_m1 + 4);
             
             int mdur_time_hod = S_mdur_time_hod.toInt();
             int mdur_time_min = S_mdur_time_min.toInt();
             int mdur_time_sec = S_mdur_time_sec.toInt();

             mdur = (mdur_time_hod*60*60) + (mdur_time_min*60) + mdur_time_sec ;

//Serial.print(mdur_time_hod);
//Serial.print(" : ");
//Serial.print(mdur_time_min);
//Serial.print(" : ");
//Serial.println(mdur_time_sec);
//Serial.println(mdur);
           }
*/
             
//------  PVR_EpgEventDuration to int ------

         int Index_1 = res_PVR_EpgEvDur.indexOf(":");
         if (Index_1 != -1)
           {
             String S_pdur_time_hod = res_PVR_EpgEvDur.substring(0, Index_1);    // search string  h:m:s
             String S_pdur_time_min = res_PVR_EpgEvDur.substring(Index_1 + 1);
             String S_pdur_time_sec = res_PVR_EpgEvDur.substring(Index_1 + 4);

             int pdur_time_hod = S_pdur_time_hod.toInt();   // string to int values
             int pdur_time_min = S_pdur_time_min.toInt();
             int pdur_time_sec = S_pdur_time_sec.toInt();
             
             pdur = (pdur_time_hod*60*60) + (pdur_time_min*60) + pdur_time_sec ; // output to sec
                     
//Serial.print(pdur_time_hod);
//Serial.print(" : ");
//Serial.print(pdur_time_min);
//Serial.print(" : ");
//Serial.println(pdur_time_sec);
//Serial.println(pdur);
           }
           
//------  PVR_EpgEventElapsedTime to int ------

        int Index_2 = res_PVR_EpgEvElaTime.indexOf(":");
         if (Index_2 != -1)
           {
             String S_pela_time_hod = res_PVR_EpgEvElaTime.substring(0, Index_2);
             String S_pela_time_min = res_PVR_EpgEvElaTime.substring(Index_2 + 1);
             String S_pela_time_sec = res_PVR_EpgEvElaTime.substring(Index_2 + 4);

             int pela_time_hod = S_pela_time_hod.toInt();
             int pela_time_min = S_pela_time_min.toInt();
             int pela_time_sec = S_pela_time_sec.toInt();

             pela = (pela_time_hod*60*60) + (pela_time_min*60)+ pela_time_sec ;
                      
//Serial.print(pela_time_hod);
//Serial.print(" : ");
//Serial.print(pela_time_min);
//Serial.print(" : ");
//Serial.println(pela_time_sec);
//Serial.println(pela);
           }
        }      
      delay(1000);
         
    }


void logo()  //--------------------------- LOGO --------------------------------
    {
      setPngPosition(65, 0);
      load_png(Logo_tv);        
    }

void ProgBar()  //------------------------ PROGRES BAR -------------------------
    {
      web();
      bar_val = pela;


//----- Syntax -----
//-----         map(value  , fromLow, fromHigh, toLow, toHigh); ----
      bar_val = map(bar_val, 0      , pdur    , 0    , 300   );

//-----              x          Y         x Lonog      y Hihgt
      tft.fillRect( 10,        210,         320,          15,    BLACK   );
      tft.drawRect( 10,        210,         300,          15,    CYAN   );
      tft.fillRect( 10,        210,         bar_val,      15,    CYAN   );

//Serial.print("Brogres bar val    ---   ");
//Serial.println(bar_val);      
//Serial.println("   |   ");

      delay(1000);

    }

  bool logo_0    = false;
//  bool bit_vid   = false;
//  bool bit_mus   = false;

void enable()  //------------------------ Enable BOOL --------------------------
    {

//----- No logo = Black Screan ----

      if ( pdur    >= 1      )  { logo_0 = true;     }
      if ( logo_0  == true   )  { logo(); ProgBar(); }

      if ( pdur    <= 0      )  { logo_0 = false;    }          
      if ( logo_0  == false  )  { tft.fillScreen(0); }
  
    }

    
void loop()  //--------------------------- LOP ---------------------------------
    {
      enable();
      web();
    }
