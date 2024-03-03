//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Author: Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _F10X16F_H
#define _F10X16F_H

#include <stdint.h>

// ����� ������������, 6�8 ��������
#define f10x16_FLOAT_WIDTH         10
#define f10x16_FLOAT_HEIGHT        16

// ���-�� �������� � ������� ������
#define f10x16f_NOFCHARS           256


// ������� ���������� ��������� �� ���������� ������� Char
uint8_t *f10x16f_GetCharTable(uint8_t Char);

#endif 
