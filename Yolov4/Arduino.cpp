#include "Arduino.h"


Arduino::~Arduino()
{
    CloseHandle(hSerial);
}

void Arduino::send(int x, int y, int a, int b, int c)
{
    WriteFile(hSerial, &x, 1, NULL, NULL);
    WriteFile(hSerial, &y, 1, NULL, NULL);
    WriteFile(hSerial, &a, 1, NULL, NULL);
    WriteFile(hSerial, &b, 1, NULL, NULL);
    WriteFile(hSerial, &c, 1, NULL, NULL);
}


void Arduino::mouseEvent(int x, int y, int leftMouse, int rightMouse, int middleMouse)
{
    send(x, y, leftMouse, rightMouse, middleMouse);
}

void Arduino::Init(std::wstring comport)
{
    hSerial = CreateFile(comport.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hSerial, &dcbSerialParams);

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    SetCommState(hSerial, &dcbSerialParams);
}
