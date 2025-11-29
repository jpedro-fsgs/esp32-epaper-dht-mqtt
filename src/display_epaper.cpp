#include "display_epaper.h"
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <vector>

#define CS_PIN 5
#define BUSY_PIN 4
#define RES_PIN 17
#define DC_PIN 16

// 400x300 SSD1683
static GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(
    GxEPD2_420_GDEY042T81(CS_PIN, DC_PIN, RES_PIN, BUSY_PIN));

void display_setup()
{
  // Não inicializa Serial aqui (feito em main)
  display.init(115200, true, 50, false);
  delay(100);

  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
}

// Desenha o layout estático (rótulos) uma vez
void drawStaticLayout()
{
    display.setFullWindow();
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);

        display.setCursor(20, 30);
        display.print("Temp:");

        display.setCursor(20, 60);
        display.print("Umid:");

        display.setCursor(20, 90);
        display.print("Luz:");

        // Coloca o rótulo de mensagem um pouco acima da área dinâmica
        // A área de mensagem agora ocupará o restante da tela abaixo deste rótulo.
        display.setCursor(20, 130);
        display.print("Mensagem:");

    } while (display.nextPage());
}

// Atualiza apenas os valores numéricos (área pequena por campo)
void updateValues(float temperatura, float umidade, float luminosidade)
{
    // Temperatura area
    display.setPartialWindow(60, 12, 160, 30);
    display.firstPage();
    do
    {
        display.fillRect(60, 12, 160, 30, GxEPD_WHITE);
        display.setCursor(60, 30);
        display.printf("%.1f C", temperatura);
    } while (display.nextPage());

    // Umidade area
    display.setPartialWindow(60, 42, 160, 30);
    display.firstPage();
    do
    {
        display.fillRect(60, 42, 160, 30, GxEPD_WHITE);
        display.setCursor(60, 60);
        display.printf("%.1f %%", umidade);
    } while (display.nextPage());

    // Luminosidade area
    display.setPartialWindow(60, 72, 160, 30);
    display.firstPage();
    do
    {
        display.fillRect(60, 72, 160, 30, GxEPD_WHITE);
        display.setCursor(60, 90);
        display.printf("%.1f %%", luminosidade);
    } while (display.nextPage());
}

// Atualiza apenas a área da mensagem MQTT
void updateMessage(const String &msgIn)
{
    // Área de mensagem: ocupa o resto da tela abaixo do rótulo
    const int msgX = 20;
    const int msgTopLabelY = 130; // onde o rótulo "Mensagem:" fica
    const int msgY = msgTopLabelY + 10; // começa alguns pixels abaixo do rótulo
    const int msgW = display.width() - msgX * 2;
    const int msgH = display.height() - msgY - 10; // margem inferior

    // Função local para quebrar texto em linhas por contagem de caracteres
    // (usa aproximação de largura de caractere para evitar dependência de
    //  getTextBounds, que pode não estar disponível da forma esperada)
    auto wrapText = [&](const String &text, int maxWidth, std::vector<String> &outLines)
    {
        outLines.clear();
        const int approxCharWidth = 7; // aproximação em pixels por caractere para FreeMonoBold9pt7b
        int maxChars = maxWidth / approxCharWidth;
        if (maxChars < 10) maxChars = 10; // segurança

        String word = "";
        String line = "";
        for (size_t i = 0; i < (size_t)text.length(); ++i)
        {
            char c = text[i];
            bool isSpace = (c == ' ' || c == '\n' || c == '\t' || c == '\r');
            if (isSpace || i == (size_t)text.length() - 1)
            {
                if (i == (size_t)text.length() - 1 && !isSpace) word += c;

                if (word.length() > 0)
                {
                    String test = (line.length() ? (line + " " + word) : word);
                    if ((int)test.length() <= maxChars)
                    {
                        line = test;
                    }
                    else
                    {
                        if (line.length())
                        {
                            outLines.push_back(line);
                            // se a palavra ainda for maior que a linha, dividir por caracteres
                            if ((int)word.length() > maxChars)
                            {
                                size_t pos = 0;
                                while (pos < word.length())
                                {
                                    String part = word.substring(pos, pos + maxChars);
                                    outLines.push_back(part);
                                    pos += maxChars;
                                }
                                line = "";
                            }
                            else
                            {
                                line = word;
                            }
                        }
                        else
                        {
                            // linha vazia e palavra maior que max -> dividir
                            if ((int)word.length() > maxChars)
                            {
                                size_t pos = 0;
                                while (pos < word.length())
                                {
                                    String part = word.substring(pos, pos + maxChars);
                                    outLines.push_back(part);
                                    pos += maxChars;
                                }
                                line = "";
                            }
                            else
                            {
                                line = word;
                            }
                        }
                    }
                    word = "";
                }

                if (c == '\n')
                {
                    if (line.length()) { outLines.push_back(line); line = ""; }
                }
            }
            else
            {
                word += c;
            }
        }

        if (word.length())
        {
            String test = (line.length() ? (line + " " + word) : word);
            if ((int)test.length() <= maxChars)
            {
                line = test;
            }
            else
            {
                if (line.length()) { outLines.push_back(line); line = ""; }
                size_t pos = 0;
                while (pos < word.length())
                {
                    String part = word.substring(pos, pos + maxChars);
                    outLines.push_back(part);
                    pos += maxChars;
                }
            }
        }

        if (line.length()) outLines.push_back(line);
    };

    // Prepara linhas
    std::vector<String> lines;
    wrapText(msgIn, msgW, lines);

    // calcula altura da fonte/linha (aproximação)
    int lineHeight = 14; // ajuste fino se necessário
    int maxLines = msgH / lineHeight;

    // Se exceder, corta as linhas extras e coloca reticências na última
    if ((int)lines.size() > maxLines)
    {
        lines.resize(maxLines);
        int approxCharWidth = 7;
        int maxCharsLine = msgW / approxCharWidth;
        if (maxCharsLine < 10) maxCharsLine = 10;
        String &last = lines.back();
        if ((int)last.length() > maxCharsLine - 3)
        {
            last = last.substring(0, maxCharsLine - 3) + "...";
        }
        else
        {
            last += "...";
        }
    }

    // Desenha a área (janela parcial)
    display.setPartialWindow(msgX, msgY, msgW, msgH);
    display.firstPage();
    do
    {
        display.fillRect(msgX, msgY, msgW, msgH, GxEPD_WHITE);

        int cursorY = msgY + (lineHeight - 2); // posição inicial do texto (baseline aproximado)
        for (size_t i = 0; i < lines.size(); ++i)
        {
            display.setCursor(msgX, cursorY + i * lineHeight);
            display.print(lines[i]);
        }
    } while (display.nextPage());
}
