#pragma once
#include <Windows.h>
#include <stdio.h>
#include <string>


class Arduino
{
private:
	HANDLE hSerial;
	void send(int x, int y, int a, int b, int c);
public:
	Arduino(std::wstring comport);
	~Arduino();
	void mouseEvent(int x, int y, int leftMouse, int rightMouse, int MiddleMouse);
};

