#pragma once
#include <Windows.h>
#include <string>
#include <codecvt>
#include "Overlay.h"

enum Mode
{
	Track, 
	Flick
};
namespace Globals
{
	extern Overlay Over;
	extern bool Menu;
}
namespace Settings
{
	extern float CONFIDENCE_THRESHOLD;
	extern float NMS_THRESHOLD;
	extern int NUM_CLASSES;
	extern float speed;
	extern float flickSpeed;

	extern float horizontal_offset;          // Fractional offset(between 0 and 1) from left side of the detection.
	extern float vertical_offset;            // Fractional offset(between 0 and 1) from top top side of the detection.
	extern int screen_dimensions[];
	extern int input_dimensions[];
	extern bool degub;
	extern Mode mode;
}

static std::wstring s2ws(const std::string& str)
{
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

static std::string ws2s(const std::wstring& wstr)
{
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

static const char alphanum[] = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
static int stringLength = sizeof(alphanum) - 1;


//key for string obvuscaation
const char key = 'B';


static auto wide_to_char(const WCHAR* source) {
	const auto wide_char_file_path_length = wcslen(source);
	auto destination_buffer = std::make_unique<char[]>(wide_char_file_path_length + 1);

	auto array_index = 0;
	while (source[array_index] != '\0') {
		destination_buffer.get()[array_index] = static_cast<CHAR>(source[array_index]);
		array_index++;
	}

	destination_buffer.get()[array_index] = '\0';
	return destination_buffer;
}