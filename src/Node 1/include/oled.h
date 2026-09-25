#ifndef OLED_H
#define OLED_H

#include <stdint.h>
#include <avr/pgmspace.h>

void OLED_Init(void);

void OLED_Reset(void);
void OLED_Home(void);

void OLED_WriteCommand(uint8_t command);
void OLED_WriteData(uint8_t data);

void OLED_GotoLine(uint8_t line);
void OLED_GotoColumn(uint8_t column);
void OLED_Pos(uint8_t line, uint8_t column);
void OLED_Clear_Line(uint8_t line);
void OLED_Print(const char *text);
void OLED_Print_Char(char c);
void OLED_Clear(void);
void OLED_DrawBitmap(const uint8_t *bitmap);
#endif