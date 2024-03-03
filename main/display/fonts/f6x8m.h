//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Author: Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _F6X8M_H
#define _F6X8M_H

#include <stdint.h>

// ����� ������������, 6�8 ��������
#define f6x8_MONO_WIDTH         6
#define f6x8_MONO_HEIGHT        8

// ���-�� �������� � ������� ������
#define f6x8m_NOFCHARS           256


// ������� ���������� ��������� �� ���������� ������� Char
uint8_t *f6x8m_GetCharTable(uint8_t Char);

#endif 
