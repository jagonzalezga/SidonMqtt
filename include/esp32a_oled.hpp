#define SCREEN_WIDTH     128
#define SCREEN_HEIGHT    32
#define OLED_RESET       4
int RSSI;//Received Signal Strength Indicator
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay()
{

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))   // Address 0x3C for 128x32
    { 
        Serial.println(F("SSD1306 Falla en pantalla OLED"));
    }
    delay(2000);
    //display.clearDisplay();
    display.display();
    
}


void textDraw()
{
    display.clearDisplay();

    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(DeviceID().c_str());
    
    // display.setTextSize(0.3);
    // display.setTextColor(WHITE);
    // display.setCursor(0, 12);
    // display.println("RSSI " + WiFi.RSSI());

    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 24);
    display.println(ipStr(WiFi.localIP()));


    // display.setTextSize(0.3);
    // display.setTextColor(WHITE);
    // display.setCursor(0, 24);
    // display.println(rtc4.getTime("%Y/%d/%m  %H:%M:%S"));
    
    display.display();
}

void textDrawGeneric(char* top_,char* middle_,char* botton_)
{

    display.clearDisplay();

    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(top_);

    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 12);
    display.println(middle_);


    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 24);
    display.println(botton_);
    
    display.display();

}

void textDrawError()
{

    display.clearDisplay();

    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(DeviceID().c_str());

    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 12);
    display.println("Network not found");

    display.display();
}

