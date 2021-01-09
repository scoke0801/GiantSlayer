#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <chrono>
#include <cassert>
#include <algorithm>
#pragma warning(disable  : 4996)    // mbstowcs unsafe###
using namespace std;

#define SERVERPORT 9000
#define BUFSIZE 4096

#define width 480 
#define height 960

#define FPS 1 / 60
