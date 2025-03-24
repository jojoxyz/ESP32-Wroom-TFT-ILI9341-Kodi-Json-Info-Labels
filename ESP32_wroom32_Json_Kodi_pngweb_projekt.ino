
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


#if defined(USE_ADAFRUIT_GFX)
  #define TFT_CS  5
  #define TFT_DC  26
  #define TFT_RST 27
  #include <Adafruit_ILI9341.h>
  Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
#else
  #include <TFT_eSPI.h>              // Hardware-specific library
  TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
#endif

// Include SPIFFS
#define FS_NO_GLOBALS
#include <FS.h>
#include "SPIFFS.h" // Required for ESP32 only

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "support_functions.h"
#include <WiFi.h>


#define BLACK          0x000000
#define WHITE          0xffffff
#define BLUE           0x0000ff
#define RED            0xff0000
#define GREEN          0x00cc00
#define CYAN           0x003831
#define MAGENTA        0x9900cc
#define YELLOW         0xffff00
#define ORANGE         0xff8000
#define PINK           0xff66ff

#define GREENYELLOW    0xccff33

#define DARK_BLUE      0x000080
#define DARK_RED       0xb30000
#define DARK_GREEN     0x006600
#define DARK_CYAN      0x00665a
#define DARK_MAGENTE   0x4d0066


void setup()  //--------------------------- SETUP ------------------------------
    {
      Serial.begin(115200);

      tft.begin();
      tft.invertDisplay(1);
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



  String filmy = "https://raw.githubusercontent.com/jojoxyz/Logo_Extra/refs/heads/main/filmy.png";
    String hudba = "https://raw.githubusercontent.com/jojoxyz/Logo_Extra/refs/heads/main/music.png";
      String stream = "https://raw.githubusercontent.com/jojoxyz/Logo_Extra/refs/heads/main/stream.png"; 

  int    pela    = 0;
    int    pdur    = 0;

  bool progbar = false;
 
  bool bool_tv = false;
    bool bool_fil = false; 
      bool bool_mus = false;
        bool bool_stre = false;
          bool bool_rec = false;

  String inRaw;
    String inRaw0;
    
void loop()  //--------------------------- LOP ---------------------------------
    { 
//---------------------------------------- WEB bool ---------------------------------
     if ((WiFi.status() == WL_CONNECTED))
        {
          HTTPClient http;

         String addr0   = ("http://192.168.0.150:8081/jsonrpc?request=");
         String comm0   = ("{%22jsonrpc%22:%222.0%22,%22method%22:%22XBMC.GetInfoBooleans%22,%22params%22:{%22booleans%22:[");

         String bool_1  = ("%22Pvr.IsPlayingTv%22," );            // ak sa prehrava TV        = true
         String bool_2  = ("%22Player.HasAudio%22," );            // ak sa prehrava audio     = true
         String bool_3  = ("%22VideoPlayer.Content(movies)%22," ); // ak sa prehrava film      = true    
         String bool_4  = ("%22Player.IsInternetStream%22," );     // ak je internetovy stream = true
         String bool_5  = ("%22Pvr.IsRecordingTV%22" );           // ak sa nahrava tv         = true


         String id0     = ("]},%22id%22:1}" );
          
         String link0   = addr0 + comm0 + bool_1 + bool_2 + bool_3 + bool_4 + bool_5 + id0 ;

         http.begin(link0);
          
         int httpCode0 = http.GET();
         inRaw0 = http.getString();
         
//Serial.println(inRaw0);

         StaticJsonDocument<0> filter;
         filter.set(true);

         StaticJsonDocument<192> doc;

         DeserializationError error = deserializeJson(doc, inRaw0, DeserializationOption::Filter(filter));

         if (error)
           {
//Serial.print("deserializeJson() failed: ");
//Serial.println(error.c_str());
             return;
           }

         JsonObject result = doc["result"];
         
         bool res_Pl_Audio   = result["Player.HasAudio"];
         bool res_Pl_Tv      = result["Pvr.IsPlayingTv"];
         bool res_Pl_Video   = result["VideoPlayer.Content(movies)"];
         bool res_Pl_IntStr  = result["Player.IsInternetStream"];
         bool res_Tv_Rec     = result["Pvr.IsRecordingTV"];

//Serial.println(res_Pl_Tv);
//Serial.println(res_Pl_Audio);
//Serial.println(res_Pl_Video);
Serial.println(res_Pl_IntStr);
Serial.println(res_Tv_Rec);
//Serial.println("");      
//---------------------------------------- WEB ---------------------------------
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
            String label_8  = ("%22VideoPlayer.Cover%22"       );

    //      String label_9  = ("%22Player.Time%22,"        );//+#+#+#+#+#+#+#+

   //       String label_10 = ("%22MusicPlayer.Duration%22,"    );
   //       String label_11 = ("%22MusicPlayer.Cover%22"        );

            String id       = ("]},%22id%22:5}"                 );
          
            String link     = addr + comm + label_1 + label_5 + label_6 + label_8 + id ;

            http.begin(link);
          
            int httpCode = http.GET();
            inRaw = http.getString();
             
//---- Deserialization + Filter ----  https://arduinojson.org/v6/assistant/#/step1  ----

            StaticJsonDocument<0> filter;
            filter.set(true);

            StaticJsonDocument<512> doc;

            DeserializationError error = deserializeJson(doc, inRaw, DeserializationOption::Filter(filter));

            if (error)
              {
//Serial.print("deserializeJson() failed: ");
//Serial.println(error.c_str());
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

//Serial.println("");
//Serial.println(res_ViPl_Cover);
//Serial.println("PDoba trvania PVR            ---   " + res_PVR_EpgEvDur);
//Serial.println("PUplinuty cas                ---   " + res_PVR_EpgEvElaTime);

//--------------------------  PVR_EpgEventDuration to int ------

            int Index_1 = res_PVR_EpgEvDur.indexOf(":");
            if (Index_1 != -1)
              {
                String S_pdur_time_hod = res_PVR_EpgEvDur.substring(0, Index_1);
                String S_pdur_time_min = res_PVR_EpgEvDur.substring(Index_1 + 1);
                String S_pdur_time_sec = res_PVR_EpgEvDur.substring(Index_1 + 4);

                int pdur_time_hod = S_pdur_time_hod.toInt();
                int pdur_time_min = S_pdur_time_min.toInt();
                int pdur_time_sec = S_pdur_time_sec.toInt();
             
                pdur = (pdur_time_hod*60*60) + (pdur_time_min*60) + pdur_time_sec ;

              }
           
//Serial.print("P_dur            ---   ");
//Serial.println(pdur);                   
       
//------------------------  PVR_EpgEventElapsedTime to int ------

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

           }         
           
//Serial.print("P_ela            ---   ");
//Serial.println(pela);

            if ( progbar == true )
              {
                pela = map(pela, 0      , pdur    , 0    , 300   );

                tft.fillRect( 10,        210,         320,          15,    BLACK   );
                tft.drawRect( 10,        210,         300,          15,    CYAN   );  //0x040301  );
                tft.fillRect( 10,        210,         pela,      15,    CYAN   );
              }
   
//Serial.println("Progres bar val              ---   " + pela);

            if (bool_tv == true)
              {
                setPngPosition(65, 0);
                load_png(res_ViPl_Cover); 
                progbar = true;
              }
     
            if (bool_fil == true)
              {
                setPngPosition(65, 0);
                load_png(filmy);
                tft.fillRect( 10, 210, 320, 15, BLACK   );
              }     
 
            if (bool_mus == true)  
              {
                setPngPosition(65, 0);
                load_png(hudba);
                tft.fillRect( 10, 210, 320, 15, BLACK   ); 
              }

            if (bool_stre == true)  
              {
                setPngPosition(65, 0);
                load_png(stream);
                tft.fillRect( 10, 210, 320, 15, BLACK   ); 
              }

            if (res_Tv_Rec == true)
              {
                tft.fillRoundRect(275,5, 40, 40, 3, RED);
                delay(1000);
                tft.fillRoundRect(275,5, 40, 40, 3, BLACK);
              }

            if (res_Tv_Rec == false)
              {
                tft.fillRoundRect(275,5, 40, 40, 3, BLACK);
              }
              
  
            if (res_Pl_Tv == true)
              { bool_stre = false; bool_fil = false; bool_mus = false; bool_tv = true; }      

          else if (res_Pl_Video == true)
                 { progbar = false; bool_stre = false; bool_mus = false; bool_tv = false; bool_fil = true; }

             else if ((res_Pl_Audio == true) && (res_Pl_IntStr == false))
                    { progbar = false; bool_stre = false; bool_tv = false; bool_fil = false; bool_mus = true; }

                else if ((res_Pl_IntStr == true) && (res_Pl_Audio == true))
                       { progbar = false; bool_tv = false; bool_fil = false; bool_mus = false; bool_stre = true; }

                        if ((res_Pl_Tv == false) && (res_Pl_Video == false) && (res_Pl_Audio == false) && (res_Pl_IntStr == false)) 
                          { progbar = false; bool_tv = false; bool_fil = false; bool_mus = false; bool_stre = false; tft.fillScreen(0); }

//Serial.println(progbar);
//Serial.println(bool_tv);
//Serial.println(bool_fil);
//Serial.println(bool_mus);
//Serial.println("");

        }
        delay(1000);       
        }

    }  
