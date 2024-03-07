#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../display/disp1color.h"
#include "demo.h"

void demo_text() {
    while (1) {
        disp1color_FillScreenbuff(0xFF);

        disp1color_DrawString(5, 5, FONTID_32F, "01234567890:");
        disp1color_DrawString(5, 45, FONTID_24F, "01234567890:");

        disp1color_DrawString(5, 81, FONTID_10X16F, "01234567890:+-*/");
        disp1color_DrawString(5, 97, FONTID_10X16F, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        disp1color_DrawString(5, 113, FONTID_10X16F, "abcdefghijklmnopqrstuvwxyz");
        disp1color_DrawString(5, 129, FONTID_10X16F, "ÀÁÂÃÄÅ¨ÆÇÈÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞ‗");
        disp1color_DrawString(5, 145, FONTID_10X16F, "אבגדהו¸זחטךכלםמןנסעףפץצקרשתûü‎‏ÿ");

        disp1color_DrawString(5, 180, FONTID_6X8M, "01234567890:+-*/");
        disp1color_DrawString(5, 190, FONTID_6X8M, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        disp1color_DrawString(5, 200, FONTID_6X8M, "abcdefghijklmnopqrstuvwxyz");
        disp1color_DrawString(5, 210, FONTID_6X8M, "ÀÁÂÃÄÅ¨ÆÇÈÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞ‗");
        disp1color_DrawString(5, 220, FONTID_6X8M, "אבגדהו¸זחטךכלםמןנסעףפץצקרשתûü‎‏ÿ");
    
        disp1color_UpdateFromBuff();

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
