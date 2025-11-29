#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define CS_PIN 5
#define BUSY_PIN 4
#define RES_PIN 17
#define DC_PIN 16

// 400x300 SSD1683
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(
    GxEPD2_420_GDEY042T81(CS_PIN, DC_PIN, RES_PIN, BUSY_PIN));

void screen_setup()
{
  Serial.begin(115200);
  Serial.println("Inicializando ePaper...");

  // *** ESSENCIAL ***
  display.init(115200, true, 50, false);
  delay(100);

  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  // Atualização completa da tela
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(10, 40);
    display.print("Vai Corinthians!!!");
  } while (display.nextPage());

  display.setPartialWindow(10, 10, 200, 40);
}

void screen_loop()
{
  static int contador = 0;

  display.firstPage();
  do
  {
    display.fillRect(10, 10, 200, 40, GxEPD_WHITE); // limpa só essa área
    display.setCursor(20, 40);
    display.print("Contador: ");
    display.print(contador);
  } while (display.nextPage());

  contador++;
  delay(1000);
}
