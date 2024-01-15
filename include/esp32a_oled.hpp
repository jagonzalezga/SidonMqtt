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
        for (;;); // No continuar al siguiente proceso
    }
    display.display();
    display.clearDisplay();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void textDraw()
{
    display.clearDisplay();

    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(DeviceID().c_str());

    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 12);
    display.println(ipStr(WiFi.localIP()));


    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 24);
    display.println("Modo cliente");
    
    display.display();
}

void textDrawGeneric()
{

    display.clearDisplay();

    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Modo AP");

    display.setTextSize(0.3);
    display.setTextColor(WHITE);
    display.setCursor(0, 12);
    display.println(ipStr(WiFi.softAPIP()));
    
    display.display();

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

