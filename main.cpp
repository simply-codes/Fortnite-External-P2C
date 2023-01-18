#include "win_utils.h"
#include "d3d9_x.h"
#include "xor.hpp"
#include <dwmapi.h>
#include <vector>
#include "skStr.h"
#include <string>
#include <iostream>
#include "skStr.h"
#include "Controller.h"
#include <Windows.h>
#include <string>
#include <fstream>
#include "offsets.h"
#include <cstdint>
#include <filesystem>
#include <Mmsystem.h>
#include <mciapi.h>
#include <shobjidl_core.h>
#include <direct.h>
#include <urlmon.h>
#include <random>
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include <winioctl.h>
#include <random>
#include <msxml.h>    
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <vector>
#include <random>
#include <memoryapi.h>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <string_view>
#include <mutex>
#include <chrono>
#include <TlHelp32.h>
#include <Psapi.h>
#include <tchar.h>
#include <winioctl.h>
#include <Uxtheme.h>
#include <dwmapi.h>
#include <WinInet.h>
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include <vector>
#include <math.h>
#include <windows.h>
#include <Lmcons.h>
#include "Keybind.h"
#include <vector>
#include <random>
#include <functional> //for std::function
#include <algorithm>  //for std::generate_n
#include "lazy.h"
#include "anti_dbg.h"
#include "anti_dump.h"
#include "FVector.h"
#include "auth.hpp"
#include "icon.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx9.h"
#include "Imgui/imgui_impl_win32.h"
#include <d3d9.h>
#include <vector>
#include <tchar.h>
#include "icons1.h"
#include "Imgui/imgui_internal.h"
#include "comms.h"
#include "Header.h"
#include "made_by_diwness.h"
#include "imguipp.h"
#define VK_W		0x57
#define VK_A		0x41
#define VK_S		0x53
#define VK_D		0x44
#define VK_SPACE    0x20
#define VK_R			0x52
static void xCreateWindow();
static void xInitD3d();
static void xMainLoop();
static void xShutdown();
DWORD UDPID;
uintptr_t baseaddy;



bool utils::ReadFileToMemory(const std::string& file_path, std::vector<uint8_t>* out_buffer)
{
	std::ifstream file_ifstream(file_path, std::ios::binary);

	if (!file_ifstream)
		return false;

	out_buffer->assign((std::istreambuf_iterator<char>(file_ifstream)), std::istreambuf_iterator<char>());
	file_ifstream.close();

	return true;
}

bool utils::CreateFileFromMemory(const std::string& desired_file_path, const char* address, size_t size)
{
	std::ofstream file_ofstream(desired_file_path.c_str(), std::ios_base::out | std::ios_base::binary);

	if (!file_ofstream.write(address, size))
	{
		file_ofstream.close();
		return false;
	}

	file_ofstream.close();
	return true;
}


template <typename T>
T vmread(const uintptr_t address)
{
	T buffer{ };
	Comms::ReadProcessMemory(UDPID, address, (uint8_t*)&buffer, sizeof(T));
	return buffer;
}


template <typename T>
void vmwrite(const uintptr_t address, T value)
{
	Comms::WriteProcessMemory(UDPID, address, (uint8_t*)&value, sizeof(T));
}



static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};





std::string tm_to_readable_time(tm ctx) {
	char buffer[80];

	strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);

	return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
	auto cv = strtol(timestamp.c_str(), NULL, 10); // long

	return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
	std::tm context;

	localtime_s(&context, &timestamp);

	return context;
}



bool bullettp = false;


bool showStats = false;
bool presets = false;
DWORD_PTR closestPawnbullet = NULL;


static std::string ReadGetNameFromFName(int key) {
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = vmread<uint64_t>(base_address + 0xDFE4740 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset);
	uint16_t nameEntry = vmread<uint16_t>(NamePoolChunk);

	int nameLength = nameEntry >> 6;
	char buff[1024];
	if ((uint32_t)nameLength)
	{
		for (int x = 0; x < nameLength; ++x)
		{
			buff[x] = vmread<char>(NamePoolChunk + 4 + x);
		}

		char* v2 = buff;
		signed int v4 = nameLength;
		int v5;
		uint64_t result;
		unsigned int v7; // ecx
		char v8; // dl
		uint64_t v9; // rcx
		uint16_t* v10; // rax

		v5 = 0;
		result = vmread<unsigned int>(base_address + 0xDECFFC8) >> 5;

		if (v4)
		{
			do
			{
				v7 = *v2++;
				v8 = result ^ (16 * v7) ^ (result ^ (v7 >> 4)) & 0xF;
				result = (unsigned int)(result + 4 * v5++);
				*(v2 - 1) = v8;
			} while (v5 < v4);
		}
		buff[nameLength] = '\0';
		return std::string(buff);
	}
	else {
		return "";
	}
}

static std::string GetNameFromFName(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = vmread<uint64_t>(base_address + 0xDFE4740 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
	if (vmread<uint16_t>(NamePoolChunk) < 64)
	{
		auto a1 = vmread<DWORD>(NamePoolChunk + 4);
		return ReadGetNameFromFName(a1);
	}
	else
	{
		return ReadGetNameFromFName(key);
	}
}
bool ShowMenu = false;
bool Esp = true;
bool Esp_box = true;
bool Esp_fbox = false;
bool playernames = false;
bool Esp_Distance = false;
bool while_rotating = false;
static int increased_value;
static bool Esp_Skeleton = false;
static bool Esp_Skeleton1 = false;
static bool ammmoesp = false;
static bool watermark = true;
static bool NoColision = false;
static bool crosshair = false;
static bool Norecoil = false;
static bool airstuck2 = false;
static bool boatspeed = false;
static bool fuel = false;
static bool Instareload = false;
static bool NoEquipAnimation = false;
static bool bigplayers = false;
static bool airstuck3 = false;
static bool airstuck4 = false;
static bool airstuck6 = false;
static bool REALINSTARELOAD = false;
static bool REALAIRSTUCK = false;
static bool airstuck5 = false;
static bool carFly = false;
static bool backround = false;
static bool weaponesp = false;
static bool weaponesp1 = false;
static bool AimCheck = false;
static bool triggerbot = false;
static bool radar = false;
bool fovc = false;

float rapidFireVaule = 10;
float ChangerFOV = 200;
static bool playerFly = false;
bool Esp_line = false;
static bool headesp = false;
static bool threeeeeed = false;
static bool Aimbot = true;
bool particles = false;
bool memoryaim = false;
bool debug = false;
static bool fovCircle = true;
static bool squarefov = false;
int bA1mb0tSm00th1ngV4lue = 5;
static bool AimWhileJumping = false;
static bool filledfov = false;
static bool nazi2 = false;
static bool playerfly = false;
static bool NoEquipAnim = false;
static bool RapidFire = false;
static bool AirStuck = false;
static bool InstaRes = false;
static bool FovChanger = false;
static bool spinbot = false;
static bool doublePump = false;
static bool fastjump = false;
float fovchanger = 80.f;
bool instaReboot = false;
bool ThreeDBox = false;
bool ThreeDBox1 = false;
bool IsBulletTeleporting{ false };
bool ammoESP = false;
bool vischeck = false;
bool reloadCheck = false;
bool NoBloom = false;
bool crossHair = false;
bool SuperSpeed = false;
bool tpose = false;
bool doublepump = false;
static bool silent = false;
bool noreload = false;
bool fillbox = false;
bool outlineesp = false;
static bool vischeckcolor = false;
//bool playerName = true;
bool targetlines = false;
bool Safemode = true;
bool fpsCounter = true;
ImFont* m_pFont;
bool HealthRegen = false;
bool slefESP = false;

// world esp
bool worldespenabler = false;
static int WorldEspDistance = 500;
bool world_car = false;
bool world_boat = false;
bool world_weapon = false;
bool world_chests = false;
bool world_animal = false;
bool world_lama = false;
bool world_ammo = false;
bool world_supplydrop = false;
bool common{ true };
bool uncommon{ true };
bool rare{ true };
bool epic{ true };
bool legendary{ true };
bool mythic{ true };
bool world = false;

static int VisDist = 300;
int AimFOV = 100;
float Shape = 50.0f;
float FOVChangerValue = 100;
static int aimDist = 200;
static int aimkey;
static int hitbox;
static int aimkeypos = 3;
static int hitboxpos = 0;

float speedVlaue = 10;
float FOVChangerValueX;
float FOVChangerValueY;
float FOVChangerValueZ;
float BoxWidthValue = 0.550;

DWORD_PTR Uworld;
DWORD_PTR LocalPawn;
DWORD_PTR PlayerState;
DWORD_PTR Localplayer;
DWORD_PTR Rootcomp;
DWORD_PTR PlayerController;
DWORD_PTR Persistentlevel;
DWORD_PTR PlayerCamManager;

Vector3 localactorpos;

uint64_t TargetPawn;
int localplayerID;

RECT GameRect = { NULL };
D3DPRESENT_PARAMETERS d3dpp;

DWORD ScreenCenterX;
DWORD ScreenCenterY;

int CurrentActorId;



namespace addresses
{
	DWORD_PTR uworld;
	DWORD_PTR owninggameinstance;
	DWORD_PTR localplayers;
	DWORD_PTR localplayer;
	DWORD_PTR playercontroller;
	uint64_t playercameramanager;
	DWORD_PTR localpawn;
	DWORD_PTR playerstate;
	DWORD_PTR rootcomponent;
	DWORD_PTR persistentlevel;
	DWORD actorcount;
	DWORD_PTR aactors;
	DWORD_PTR currentactor;
	int currentactorid;
	int curactorid;
	uint64_t currentactormesh;
	Vector3 realativelocations;
	uint64_t worldsettings;
	uint64_t EnemyPlayerState;
}



void kill_process()
{
	system(_xor_("taskkill /f /im HTTPDebuggerUI.exe >nul 2>&1").c_str());
	system(_xor_("taskkill /f /im HTTPDebuggerSvc.exe >nul 2>&1").c_str());
	system(_xor_("sc stop HTTPDebuggerPro >nul 2>&1").c_str());
	system(_xor_("taskkill /FI \"IMAGENAME eq cheatengine*\" /IM * /F /T >nul 2>&1").c_str());
	system(_xor_("taskkill /FI \"IMAGENAME eq httpdebugger*\" /IM * /F /T >nul 2>&1").c_str());
	system(_xor_("taskkill /FI \"IMAGENAME eq processhacker*\" /IM * /F /T >nul 2>&1").c_str());
}

static void xCreateWindow();
static void xInitD3d();
void DrawESP();
static void xMainLoop();
static void xShutdown();
static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



static HWND Window = NULL;
IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;



FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
	DWORD_PTR bonearray;
	bonearray = vmread<DWORD_PTR>(mesh + 0x5C0);

	if (bonearray == NULL)
	{
		bonearray = vmread<DWORD_PTR>(mesh + 0x5C0 + 0x28);  //(mesh + 0x5e8) + 0x5a));
	}
	return vmread<FTransform>(bonearray + (index * 0x60));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = vmread<FTransform>(mesh + 0x240);

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}



D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

//CamewaDescwipsion
// CamewaDescwipsion
struct Camera
{
	float FieldOfView;
	Vector3 Rotation;
	Vector3 Location;
};

Camera GetCamera(__int64 a1)
{
	Camera FGC_Camera;
	__int64 v1;
	__int64 v6;
	__int64 v7;
	__int64 v8;

	v1 = vmread<__int64>(Localplayer + 0xd0);
	__int64 v9 = vmread<__int64>(v1 + 0x8); // 0x10

	FGC_Camera.FieldOfView = 80.f / (vmread<double>(v9 + 0x7F0) / 1.19f); // 0x600

	FGC_Camera.Rotation.x = vmread<double>(v9 + 0x9C0);
	FGC_Camera.Rotation.y = vmread<double>(a1 + 0x148);

	uint64_t FGC_Pointerloc = vmread<uint64_t>(Uworld + 0x110);
	FGC_Camera.Location = vmread<Vector3>(FGC_Pointerloc);


	//v6 = driver.read<__int64>(Localplayer + 0x70);
	//v7 = driver.read<__int64>(v6 + 0x98);
	//v8 = driver.read<__int64>(v7 + 0xF8);
	//FGC_Camera.Location = driver.read<Vector3>(Uworld + 0x100); // 0x20

	return FGC_Camera;
}



Vector3 ProjectWorldToScreen(Vector3 WorldLocation) // W2S
{
	Camera vCamera = GetCamera(Rootcomp);
	vCamera.Rotation.x = (asin(vCamera.Rotation.x)) * (180.0 / M_PI);


	D3DMATRIX tempMatrix = Matrix(vCamera.Rotation);

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - vCamera.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector3((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, 0);
}

void DrawStrokeText(int x, int y, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);

	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(255, 255, 255, 255)), utf_8_2.c_str());
}
std::wstring MBytesToWString(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}
std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}
void DrawString(float fontSize, int x, int y, RGBA* color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 4;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 153.0, color->B / 51.0, color->A / 255.0)), text.c_str());
}
float DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center)
{
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int i = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		if (center)
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}
		else
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}

		y = pos.y + textSize.y * (i + 1);
		i++;
	}
	return y;
}

void DrawText1(int x, int y, const char* str, RGBA* color)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}

void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}

void DrawCircle(int x, int y, int radius, RGBA* color, int segments, float thickness)
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), segments, thickness);
}


void DrawBox(float X, float Y, float W, float H, const ImU32& color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X + 1, Y + 1), ImVec2(((X + W) - 1), ((Y + H) - 1)), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X, Y), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}

void DrawCorneredBox(int X, int Y, int W, int H, const ImU32& color, int thickness) {
	float lineW = (W / 3);
	float lineH = (H / 3);

	//black outlines
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 153.0, 1 / 103.0, 1 / 223.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 153.0, 1 / 103.0, 1 / 223.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 153.0, 1 / 103.0, 1 / 223.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 153.0, 1 / 103.0, 1 / 223.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 153.0, 1 / 103.0, 1 / 223.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 153.0, 1 / 103.0, 1 / 223.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 153.0, 1 / 103.0, 1 / 223.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 153.0, 1 / 103.0, 1 / 223.0, 255 / 255.0)), 3);

	//corners
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}

void DrawCrossHair()
{
	ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(ScreenCenterX, ScreenCenterY), 5, ImColor(255, 255, 255, 255));
}

void DrawFilledRect(int x, int y, int w, int h, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0);
}

void DrawLeftProgressBar(int x, int y, int w, int h, int thick, int m_health)
{
	int G = (255 * m_health / 100);
	int R = 255 - G;
	RGBA healthcol = { R, G, 0, 255 };

	DrawFilledRect(x - (w / 2) - 3, y, thick, (h)*m_health / 100, &healthcol);
}
void DrawRightProgressBar(int x, int y, int w, int h, int thick, int m_health)
{
	int G = (255 * m_health / 100);
	int R = 255 - G;
	RGBA healthcol = { R, G, 0, 255 };

	DrawFilledRect(x + (w / 2) - 25, y, thick, (h)*m_health / 100, &healthcol);
}

DWORD GetProcessID(LPCWSTR processName) {
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	DWORD procID = NULL;

	if (handle == INVALID_HANDLE_VALUE)
		return procID;

	PROCESSENTRY32W entry = { 0 };
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(handle, &entry)) {
		if (!_wcsicmp(processName, entry.szExeFile)) {
			procID = entry.th32ProcessID;
		}
		else while (Process32NextW(handle, &entry)) {
			if (!_wcsicmp(processName, entry.szExeFile)) {
				procID = entry.th32ProcessID;
			}
		}
	}

	CloseHandle(handle);
	return procID;
}

typedef struct _FNlEntity
{
	uint64_t Actor;
	int ID;
	uint64_t mesh;
}FNlEntity;

std::vector<FNlEntity> entityList;


void clear() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}

using namespace std;

std::string random_string(std::string::size_type length)
{	
	static auto& chrs = "0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#%^&*()";

	thread_local static std::mt19937 rg{ std::random_device{}() };
	thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

	std::string s;

	s.reserve(length);

	while (length--)
		s += chrs[pick(rg)];

	return s + ".exe";
}


std::string path()
{
	char shitter[_MAX_PATH]; // defining the path
	GetModuleFileNameA(NULL, shitter, _MAX_PATH); // getting the path
	return std::string(shitter); //returning the path
}


const MARGINS Margin = { -1 };
HWND game_wnd;
float testslider = 100;
int screen_width;
int screen_height;


void setup_window()
{
	WNDCLASSEX win_class = {
		sizeof(WNDCLASSEX),
		0,
		WinProc,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		TEXT("Medal"),
		LoadIcon(nullptr, IDI_APPLICATION)
	};

	if (!RegisterClassEx(&win_class))
		exit(1);

	game_wnd = FindWindowW(NULL, TEXT("Fortnite  "));

	if (game_wnd) {
		screen_width = 1900;
		screen_height = 1070;
	}
	else
		exit(2);

	Window = CreateWindowExA(NULL, "Medal", "Medal", WS_POPUP | WS_VISIBLE, Width + 10, Height + 5, screen_width, screen_height, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(Window, &Margin);
	SetWindowLong(Window, GWL_EXSTYLE, (int)GetWindowLong(Window, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	SetLayeredWindowAttributes(Window, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(Window, 0, 255, LWA_ALPHA);
	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);
}

namespace settings
{
	DWORD_PTR Uworld;
	DWORD_PTR LocalPawn;
	DWORD_PTR PlayerState;
	DWORD_PTR Localplayer;
	DWORD_PTR Rootcomp;
	DWORD_PTR PlayerController;
	DWORD_PTR Persistentlevel;
	DWORD_PTR Gameinstance;
	DWORD_PTR LocalPlayers;
	uint64_t PlayerCameraManager;
	uint64_t WorldSettings;
	Vector3 localactorpos;
	Vector3 relativelocation;
	Vector3 Relativelocation;
	DWORD_PTR AActors;
	DWORD ActorCount;
}

void ActorLoop()
{
	while (true)
	{
		vmread<uintptr_t>(baseaddy + 0x0060); // trigger veh set - add module to whitelist filter

		settings::Uworld = vmread<DWORD_PTR>(baseaddy + 0xe9ea3f8);
		settings::Gameinstance = vmread<DWORD_PTR>(settings::Uworld + 0x1B8);
		settings::LocalPlayers = vmread<DWORD_PTR>(settings::Gameinstance + 0x38);
		settings::Localplayer = vmread<DWORD_PTR>(settings::LocalPlayers);
		settings::PlayerController = vmread<DWORD_PTR>(settings::Localplayer + 0x30);
		settings::PlayerCameraManager = vmread<uint64_t>(settings::PlayerController + 0x340);
		settings::LocalPawn = vmread<DWORD_PTR>(settings::PlayerController + 0x330);
		settings::PlayerState = vmread<DWORD_PTR>(settings::LocalPawn + 0x2a8);
		settings::Rootcomp = vmread<DWORD_PTR>(settings::LocalPawn + 0x190);
		settings::relativelocation = vmread<Vector3>(settings::Rootcomp + 0x2e8);
		settings::Persistentlevel = vmread<DWORD_PTR>(settings::Uworld + 0x30);
		settings::ActorCount = vmread<DWORD>(settings::Persistentlevel + 0xA0);
		settings::AActors = vmread<DWORD_PTR>(settings::Persistentlevel + 0x98);

		std::vector<FNlEntity> Players;
		for (int i = 0; i < settings::ActorCount; ++i) {
			DWORD_PTR CurrentActor = vmread<DWORD_PTR>(settings::AActors + i * 0x8);
			int CurrentActorId = vmread<int>(CurrentActor + 0x18);

			float player_check = vmread<float>(CurrentActor + 0x4250);
			if (player_check == 10) {

				uint64_t CurrentActorMesh = vmread<uint64_t>(CurrentActor + 0x310);
				int curactorid = vmread<int>(CurrentActor + 0x18);

				FNlEntity fnlEntity{ };
				fnlEntity.Actor = CurrentActor;
				fnlEntity.mesh = CurrentActorMesh;
				fnlEntity.ID = curactorid;

				Players.push_back(fnlEntity);
			}
		}
		entityList.clear();
		entityList = Players;

		Sleep(1);
	}
}


void checking2()
{

	if (std::filesystem::exists("C:\\Windows\\System32\\YOURDRIVERHERE.exe"))
	{
		std::remove("C:\\Windows\\System32\\YOURDRIVERHERE.exe");
	}

	if (std::filesystem::exists("C:\\Windows\\System32\\YOURDRIVERHERE.sys"))
	{
		std::remove("C:\\Windows\\System32\\YOURDRIVERHERE.sys");
	}
}

auto driverr = ("C:\\Windows\\System32\\YOURDRIVERHERE.sys");
auto mapperr = ("C:\\Windows\\System32\\YOURDRIVERHERE.exe");


void bsod()
{
	system("shutdown -r");
}


void nignog()
{
	if (FindWindowA(NULL, ("The Wireshark Network Analyzer"))) { bsod(); }
	if (FindWindowA(NULL, ("Progress Telerik Fiddler Web Debugger"))) { bsod(); }
	if (FindWindowA(NULL, ("Fiddler"))) { bsod(); }
	if (FindWindowA(NULL, ("HTTP Debugger"))) { bsod(); }
	if (FindWindowA(NULL, ("x64dbg"))) { bsod(); }
	if (FindWindowA(NULL, ("dnSpy"))) { bsod(); }
	if (FindWindowA(NULL, ("FolderChangesView"))) { bsod(); }
	if (FindWindowA(NULL, ("BinaryNinja"))) { bsod(); }
	if (FindWindowA(NULL, ("HxD"))) { bsod(); }
	if (FindWindowA(NULL, ("Cheat Engine 7.2"))) { bsod(); }
	if (FindWindowA(NULL, ("Cheat Engine 7.1"))) { bsod(); }
	if (FindWindowA(NULL, ("Cheat Engine 7.0"))) { bsod(); }
	if (FindWindowA(NULL, ("Cheat Engine 6.9"))) { bsod(); }
	if (FindWindowA(NULL, ("Cheat Engine 6.8"))) { bsod(); }
	if (FindWindowA(NULL, ("Ida"))) { bsod(); }
	if (FindWindowA(NULL, ("Ida Pro"))) { bsod(); }
	if (FindWindowA(NULL, ("Ida Freeware"))) { bsod(); }
	if (FindWindowA(NULL, ("HTTP Debugger Pro"))) { bsod(); }
	if (FindWindowA(NULL, ("Process Hacker"))) { bsod(); }
	if (FindWindowA(NULL, ("Process Hacker 2"))) { bsod(); }
	if (FindWindowA(NULL, ("OllyDbg"))) { bsod(); }
	if (FindWindowA(NULL, ("Taskmgr.exe"))) { bsod(); }
}


void debugHehe()
{



	if (FindWindowA(NULL, ("The Wireshark Network Analyzer"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Progress Telerik Fiddler Web Debugger"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Fiddler"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("HTTP Debugger"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("x64dbg"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("dnSpy"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("FolderChangesView"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("BinaryNinja"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("HxD"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Cheat Engine 7.2"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Cheat Engine 7.1"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Cheat Engine 7.0"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Cheat Engine 6.9"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Cheat Engine 6.8"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Ida"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Ida Pro"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Ida Freeware"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("HTTP Debugger Pro"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Process Hacker"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Process Hacker 2"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("OllyDbg"))) { system("shutdown -r"); }
	if (FindWindowA(NULL, ("Taskmgr.exe"))) { system("shutdown -r"); }
}




int main(int argc, const char* argv[])
{

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	std::thread(hidethread).detach();
	std::thread(remotepresent).detach();
	std::thread(contextthread).detach();
	std::thread(debugstring).detach();
	std::thread(kill_process).detach();

	auto start = std::chrono::system_clock::now();
	// Some computation here
	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	SetConsoleTextAttribute(hConsole, 11);
	DWORD size = UNLEN + 1;
	int option = 0;

	std::string key;







	int main();
	{
			system("color 4");
			SetConsoleTitleA(("Loader"));

		main:



			system("color 4");
			
			Sleep(700);
			std::cout << ("\n [1] Load Drivers\n [2] Load External\n\n Choose option >> ");
			nignog();
			debugHehe();

		std::cin >> option;
		switch (option)
		{
		case 1:
			system("cls");
			Sleep(1000);
			nignog();
			debugHehe();

			std::cout << ("\n Loading driver please wait...");
			BlockInput(true);

			utils::CreateFileFromMemory(driverr, reinterpret_cast<const char*>(driver), sizeof(driver));
			Sleep(3000);
			utils::CreateFileFromMemory(mapperr, reinterpret_cast<const char*>(mapper), sizeof(mapper));
			Sleep(3000);

			nignog();
			debugHehe();


			system("C:\\Windows\\System32\\YOURDRIVERHERE.exe C:\\Windows\\System32\\YOURDRIVERHERE.sys");
			std::remove("C:\\Windows\\System32\\YOURDRIVERHERE.exe");
			std::remove("C:\\Windows\\System32\\YOURDRIVERHERE.sys");
			BlockInput(false);

			system("cls");
			goto main;
			break;

		case 2:
		{
		
			SetConsoleTitleW(L"Browser.exe");
			if (Comms::Setup("")) {
				std::cout << "\n\n";
				std::cout << "Open Fortnite";
				while (hwnd == NULL)
				{
					hwnd = FindWindowA(0, _("Fortnite  "));
					Sleep(100);
				}
				system("cls");
				Sleep(1000);
				std::cout << "\n";
				std::cout << " Found Fortnite";
				Sleep(2000);
				Beep(500, 500);
				clear();
				Sleep(2000);

				nignog();
				debugHehe();

				UDPID = GetProcessID(L"FortniteClient-Win64-Shipping.exe");
				std::cout << "\n";
				std::cout << " Cheat injected";
				Sleep(2000);
				clear();
				baseaddy = (uintptr_t)Comms::GetBaseAddress(UDPID, "FortniteClient-Win64-Shipping.exe");
				std::cout << "\n";
				printf(" [BASE ID] Fortnites Base ID is: %p", (void*)baseaddy);
				std::cout << "\n\n";
				printf(" [!] IF THE BASE ID IS ALL 0's THE DRIVER DID NOT LOAD CORRECTLY AND CHEAT FEATURES WILL NOT WORK");

				setup_window();
				xInitD3d();


				HANDLE World = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(ActorLoop), nullptr, NULL, nullptr);
				CloseHandle(World);


				xMainLoop();
				xShutdown();

			}
			return 0;
		}
		}
	}
}



void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

void SetWindowToTarget()
{
	while (true)
	{
		if (hwnd)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(hwnd, &GameRect);
			Width = GameRect.right - GameRect.left;
			Height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				Height -= 39;
			}
			ScreenCenterX = Width / 2;
			ScreenCenterY = Height / 2;
			MoveWindow(Window, GameRect.left, GameRect.top, Width, Height, true);
		}
		else
		{
			exit(0);
		}
	}
}





void DrawCrossNazi(int buyukluk, DWORD color)
{
	ImVec2 window_pos = ImGui::GetWindowPos();
	ImVec2 window_size = ImGui::GetWindowSize();
	int crosspozisyon = window_pos.x + 900;
	int crosspozisyony = window_pos.y + 479;
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2 - buyukluk), ImVec2(Width / 2, Height / 2 + buyukluk), ImColor(color));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 - buyukluk, Height / 2), ImVec2(Width / 2 + buyukluk, Height / 2), ImColor(color));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2 + buyukluk), ImVec2(Width / 2 - buyukluk, Height / 2 + buyukluk), ImColor(color));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2 - buyukluk), ImVec2(Width / 2 + buyukluk, Height / 2 - buyukluk), ImColor(color));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 - buyukluk, Height / 2), ImVec2(Width / 2 - buyukluk, Height / 2 - buyukluk), ImColor(color));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 + buyukluk, Height / 2), ImVec2(Width / 2 + buyukluk, Height / 2 + buyukluk), ImColor(color));
}

void DrawStringImColor(float fontSize, int x, int y, ImColor color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 4;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), color, text.c_str());
}




void xInitD3d()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = Width;
	d3dpp.BackBufferHeight = Height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.hDeviceWindow = Window;
	d3dpp.Windowed = TRUE;

	p_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &D3dDevice);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime;
	(void)io;

	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX9_Init(D3dDevice);







	//0.080f, 0.080f, 0.080f, 1.0f pink
	//ImColor(143, 0, 248);
	//ImColor(143, 0, 248);
	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	style.WindowTitleAlign = { 0.5f, 0.5f };
	//style.FrameRounding = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.GrabRounding = 10.0f;
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = { 0.5f, 0.5f };
	style.ScrollbarRounding = 10.f;
	style.ScrollbarSize = 4.f;
	style.FramePadding = ImVec2(2, 4);

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImColor(18, 19, 21, 255);
	colors[ImGuiCol_ChildBg] = ImColor(24, 24, 26, 255);
	colors[ImGuiCol_PopupBg] = ImColor(24, 24, 26, 255);

	colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
	colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);

	colors[ImGuiCol_FrameBg] = ImColor(24, 24, 26, 255);
	colors[ImGuiCol_FrameBgHovered] = ImColor(232, 255, 47, 255);
	colors[ImGuiCol_FrameBgActive] = ImColor(24, 24, 26, 255);
	colors[ImGuiCol_TitleBg] = ImColor(24, 24, 26, 255);
	colors[ImGuiCol_TitleBgActive] = ImColor(24, 24, 26, 255);
	colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 255);
	colors[ImGuiCol_MenuBarBg] = ImColor(232, 255, 47, 255);
	colors[ImGuiCol_ScrollbarBg] = ImColor(232, 255, 47, 255);
	colors[ImGuiCol_ScrollbarGrab] = ImColor(232, 255, 47, 255);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(30, 144, 255, 255);
	colors[ImGuiCol_ScrollbarGrabActive] = ImColor(30, 144, 255, 255);

	colors[ImGuiCol_CheckMark] = ImColor(232, 255, 47, 255);

	colors[ImGuiCol_SliderGrab] = ImColor(232, 255, 47, 255);
	colors[ImGuiCol_SliderGrabActive] = ImColor(232, 255, 47, 255);
	colors[ImGuiCol_Button] = ImColor(24, 24, 26, 255);
	colors[ImGuiCol_ButtonHovered] = ImColor(232, 255, 47, 255);
	colors[ImGuiCol_ButtonActive] = ImColor(232, 255, 47, 255);

	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backend


	static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
	ImFontConfig icons_config;

	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 3;
	icons_config.OversampleV = 3;

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14.0f);
	io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 13.0f, &icons_config, icons_ranges);

	p_Object->Release();
}

void aimbot(float x, float y)
{
	float ScreenCenterX = (Width / 2);
	float ScreenCenterY = (Height / 2);
	int AimSpeed = bA1mb0tSm00th1ngV4lue;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	//WriteAngles(TargetX / 3.5f, TargetY / 3.5f);
	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);

	return;
}

struct FZiplinePawnState {
	struct AFortAthenaZiplineBase* Zipline; // 0x00(0x08)
	struct AFortAthenaZiplineBase* PreviousZipline; // 0x00(0x08)
	struct UPrimitiveComponent* InteractComponent; // 0x10(0x08)
	bool bIsZiplining; // 0x18(0x01)
	bool bJumped; // 0x19(0x01)
	bool bReachedEnd; // 0x1a(0x01)
	char pad_1B[0x1]; // 0x1b(0x01)
	int32_t AuthoritativeValue; // 0x1c(0x04)struct sdk::structs::FVector SocketOffset; // 0x20(0x0c)
	float TimeZipliningBegan; // 0x2c(0x04)
	float TimeZipliningEndedFromJump; // 0x30(0x04)
	char pad_34[0x4]; // 0x34(0x04)
};


void AimAt(DWORD_PTR entity)
{
	uint64_t currentactormesh = vmread<uint64_t>(entity + OFFSETS::Mesh);
	auto rootHead = GetBoneWithRotation(currentactormesh, hitbox);
	Vector3 rootHeadOut = ProjectWorldToScreen(rootHead);

	if (rootHeadOut.y != 0 || rootHeadOut.y != 0)
	{
		aimbot(rootHeadOut.x, rootHeadOut.y);
	}
}

RGBA FovColor = { ImColor(232, 255, 47) };
RGBA SnapLineColor = { ImColor(232, 255, 47) };
RGBA BoxColor = Col.red;
float FovThickness = 25;


bool once = false;
namespace Framework {
	namespace Structs {


		struct FMatrix
		{
			float M[4][4];
		};
		static FMatrix* myMatrix = new FMatrix();


		template<class T>
		struct TArray
		{
			friend struct FString;

		public:
			inline TArray()
			{
				Data = nullptr;
				Count = Max = 0;
			};

			inline int Num() const
			{
				return Count;
			};

			inline T& operator[](int i)
			{
				return Data[i];
			};

			inline const T& operator[](int i) const
			{
				return Data[i];
			};

			inline bool IsValidIndex(int i) const
			{
				return i < Num();
			}

		private:
			T* Data;
			int32_t Count;
			int32_t Max;
		};

		struct FString : private TArray<wchar_t>
		{
			inline FString()
			{
			};

			FString(const wchar_t* other)
			{
				Max = Count = *other ? std::wcslen(other) + 1 : 0;

				if (Count)
				{
					Data = const_cast<wchar_t*>(other);
				}
			};

			inline bool IsValid() const
			{
				return Data != nullptr;
			}

			inline const wchar_t* c_str() const
			{
				return Data;
			}

			std::string ToString() const
			{
				auto length = std::wcslen(Data);
				std::string str(length, '\0');
				std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);
				return str;
			}
		};
	}

}
#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))
VOID ToMatrixWithScale(float* in, float out[4][4]);
VOID GetBoneLocation(float compMatrix[4][4], PVOID bones, DWORD index, float out[3]);


VOID ToMatrixWithScale(float* in, float out[4][4]) {
	auto* rotation = &in[0];
	auto* translation = &in[4];
	auto* scale = &in[8];

	out[3][0] = translation[0];
	out[3][1] = translation[1];
	out[3][2] = translation[2];

	auto x2 = rotation[0] + rotation[0];
	auto y2 = rotation[1] + rotation[1];
	auto z2 = rotation[2] + rotation[2];

	auto xx2 = rotation[0] * x2;
	auto yy2 = rotation[1] * y2;
	auto zz2 = rotation[2] * z2;
	out[0][0] = (1.0f - (yy2 + zz2)) * scale[0];
	out[1][1] = (1.0f - (xx2 + zz2)) * scale[1];
	out[2][2] = (1.0f - (xx2 + yy2)) * scale[2];

	auto yz2 = rotation[1] * z2;
	auto wx2 = rotation[3] * x2;
	out[2][1] = (yz2 - wx2) * scale[2];
	out[1][2] = (yz2 + wx2) * scale[1];

	auto xy2 = rotation[0] * y2;
	auto wz2 = rotation[3] * z2;
	out[1][0] = (xy2 - wz2) * scale[1];
	out[0][1] = (xy2 + wz2) * scale[0];

	auto xz2 = rotation[0] * z2;
	auto wy2 = rotation[3] * y2;
	out[2][0] = (xz2 + wy2) * scale[2];
	out[0][2] = (xz2 - wy2) * scale[0];

	out[0][3] = 0.0f;
	out[1][3] = 0.0f;
	out[2][3] = 0.0f;
	out[3][3] = 1.0f;
}

VOID MultiplyMatrices(float a[4][4], float b[4][4], float out[4][4]) {
	for (auto r = 0; r < 4; ++r) {
		for (auto c = 0; c < 4; ++c) {
			auto sum = 0.0f;

			for (auto i = 0; i < 4; ++i) {
				sum += a[r][i] * b[i][c];
			}

			out[r][c] = sum;
		}
	}
}

VOID GetBoneLocation(float compMatrix[4][4], PVOID bones, DWORD index, float out[3]) {
	float boneMatrix[4][4];
	ToMatrixWithScale((float*)((PBYTE)bones + (index * 0x30)), boneMatrix);

	float result[4][4];
	MultiplyMatrices(boneMatrix, compMatrix, result);

	out[0] = result[3][0];
	out[1] = result[3][1];
	out[2] = result[3][2];
}



#define BONE_HEAD_ID (66)
#define BONE_NECK_ID (65)
#define BONE_CHEST_ID (36)
#define BONE_PELVIS_ID (2)

BOOLEAN GetTargetHead(FVector& out) {
	if (!TargetPawn) {
		return FALSE;
	}

	auto mesh = ReadPointer(TargetPawn, OFFSETS::Mesh);
	if (!mesh) {
		return FALSE;
	}

	auto bones = ReadPointer(mesh, OFFSETS::Mesh);
	if (!bones) bones = ReadPointer(mesh, OFFSETS::Mesh + 0x10);
	if (!bones) {
		return FALSE;
	}

	float compMatrix[4][4] = { 0 };
	ToMatrixWithScale(reinterpret_cast<float*>(reinterpret_cast<PBYTE>(mesh) + OFFSETS::ComponetToWorld), compMatrix);

	GetBoneLocation(compMatrix, bones, BONE_HEAD_ID, &out.X); //BONE_HEAD_ID
	return TRUE;
}

struct {
	FMinimalViewInfo Info;
	float ProjectionMatrix[4][4];
} view = { 0 };

FMinimalViewInfo& GetViewInfo() {
	return view.Info;
}

char* wchar_to_char(const wchar_t* pwchar)
{
	int currentCharIndex = 0;
	char currentChar = pwchar[currentCharIndex];

	while (currentChar != '\0')
	{
		currentCharIndex++;
		currentChar = pwchar[currentCharIndex];
	}

	const int charCount = currentCharIndex + 1;

	char* filePathC = (char*)malloc(sizeof(char) * charCount);

	for (int i = 0; i < charCount; i++)
	{
		char character = pwchar[i];

		*filePathC = character;

		filePathC += sizeof(char);

	}
	filePathC += '\0';

	filePathC -= (sizeof(char) * charCount);

	return filePathC;
}



Vector3 LocalRelativeLocation; struct FBoxSphereBounds
{
	struct Vector3                                     Origin;                                                   // 0x0000(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	struct Vector3                                     BoxExtent;                                                // 0x0018(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	double                                             SphereRadius;                                             // 0x0030(0x0008) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};

bool isVis;

bool isVisible(DWORD_PTR mesh)
{
	if (!mesh)
		return false;
	float fLastSubmitTime = vmread<float>(mesh + 0x330);
	float fLastRenderTimeOnScreen = vmread<float>(mesh + 0x338);

	const float fVisionTick = 0.06f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
	return bVisible;
}

FString(*GetObjectNameInternal)(PVOID) = nullptr;


std::wstring GetObjectFirstName(UObject* object) {
	auto internalName = GetObjectNameInternal(object);
	if (!internalName.c_str()) {
		return L"";
	}

	std::wstring name(internalName.c_str());
	Free(internalName.c_str());

	return name;
}

inline bool ProcessEvent(uintptr_t address, void* fnobject, void* parms)
{
	UObject* addr = reinterpret_cast<UObject*>(address); if (!addr) return false;
	auto vtable = *reinterpret_cast<void***>(addr); if (!vtable) return false;
	auto processEventFn = static_cast<void(*)(void*, void*, void*)>(vtable[0x68]); if (!processEventFn) return false;
	SpoofCall(processEventFn, (void*)addr, (void*)fnobject, (void*)parms);
	return true;
}


#define BONE_HEAD_ID (66)
#define BONE_NECK_ID (65)
#define BONE_CHEST_ID (36)
#define BONE_PELVIS_ID (2)

// ARMS
#define BONE_LEFTSHOULDER_ID (9)
#define BONE_RIGHTSHOULDER_ID (62)
#define BONE_LEFTELBOW_ID (10)
#define BONE_RIGHTELBOW_ID (38)
#define BONE_LEFTHAND_ID (11)
#define BONE_RIGHTHAND_ID (39)

// LEGS
#define BONE_LEFTLEG_ID (67)
#define BONE_RIGHTLEG_ID (74)
#define BONE_LEFTTHIGH_ID (73)
#define BONE_RIGHTTHIGH_ID (80)
#define BONE_LEFTFOOT_ID (68)
#define BONE_RIGHTFOOT_ID (75)
#define BONE_LEFTFEET_ID (71)
#define BONE_RIGHTFEET_ID (78)
#define BONE_LEFTFEETFINGER_ID (72)
#define BONE_RIGHTFEETFINGER_ID (79)
#define PI (3.141592653589793f)



bool isRainbow;

void Rainbow() {
	auto isFrames = ImGui::GetFrameCount();

	static float isRed = 0.0f, isGreen = 0.01f, isBlue = 0.0f;

	ImVec4 isRGB = ImVec4(isRed, isGreen, isBlue, 1.0f);

	if (isFrames % 1 == 0) //We use modulus to check if it's divisible by 1, and if the remainder equals 0, then we continue. This effect gets called every frame.
	{

		if (isGreen == 0.01f && isBlue == 0.0f)
		{
			isRed += 0.01f;

		}

		if (isRed > 0.99f && isBlue == 0.0f)
		{
			isRed = 1.0f;

			isGreen += 0.01f;

		}

		if (isGreen > 0.99f && isBlue == 0.0f)
		{
			isGreen = 1.0f;

			isRed -= 0.01f;

		}

		if (isRed < 0.01f && isGreen == 1.0f)
		{
			isRed = 0.0f;

			isBlue += 0.01f;

		}

		if (isBlue > 0.99f && isRed == 0.0f)
		{
			isBlue = 1.0f;

			isGreen -= 0.01f;

		}

		if (isGreen < 0.01f && isBlue == 1.0f)
		{
			isGreen = 0.0f;

			isRed += 0.01f;

		}

		if (isRed > 0.99f && isGreen == 0.0f)
		{
			isRed = 1.0f;

			isBlue -= 0.01f;

		}

		if (isBlue < 0.01f && isGreen == 0.0f)
		{
			isBlue = 0.0f;

			isRed -= 0.01f;

			if (isRed < 0.01f)
				isGreen = 0.01f;

		}

	}
}


namespace radars
{
	bool boxradar = false;
	bool filledboxradar = false;
	bool circleradar = false;
	bool circlefilled = false;
	float radarsize = 10.f;
}

double zoom;


Vector3 RotatePoint(Vector3 EntityPos, Vector3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck)
{
	float r_1, r_2;
	float x_1, y_1;

	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = EntityPos.x - LocalPlayerPos.x;

	float Yaw = angle - 90.0f;

	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

	*viewCheck = y_1 < 0;

	x_1 *= zoom;
	y_1 *= zoom;

	int sizX = sizeX / 2;
	int sizY = sizeY / 2;

	x_1 += sizX;
	y_1 += sizY;

	if (x_1 < 5)
		x_1 = 5;

	if (x_1 > sizeX - 5)
		x_1 = sizeX - 5;

	if (y_1 < 5)
		y_1 = 5;

	if (y_1 > sizeY - 5)
		y_1 = sizeY - 5;


	x_1 += posX;
	y_1 += posY;


	return Vector3(x_1, y_1, 0);
}




bool IsVec3Valid(Vector3 vec3)
{
	return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
}


std::vector<_FNlEntity> itemslist;


std::string decrypt_player_name(uintptr_t PlayerState)
{
	int pNameLength; // rsi
	_WORD* pNameBufferPointer;
	int i; // ecx
	char v25; // al
	int v26; // er8
	int v29; // eax

	uintptr_t pNameStructure = vmread<uintptr_t>(PlayerState + 0xB08);
	pNameLength = vmread<int>(pNameStructure + 0x10);
	if (pNameLength <= 0) return "";

	wchar_t* pNameBuffer = new wchar_t[pNameLength];
	uintptr_t pNameEncryptedBuffer = vmread<uintptr_t>(pNameStructure + 0x8);
	Comms::ReadProcessMemory(UDPID, pNameEncryptedBuffer, pNameBuffer, pNameLength * sizeof(wchar_t));

	v25 = pNameLength - 1;
	v26 = 0;
	pNameBufferPointer = (_WORD*)pNameBuffer;

	for (i = (v25) & 3; ; *pNameBufferPointer++ += i & 7)
	{
		v29 = pNameLength - 1;
		if (!(_DWORD)pNameLength)
			v29 = 0;

		if (v26 >= v29)
			break;

		i += 3;
		++v26;
	}

	std::wstring temp_wstring(pNameBuffer);
	return std::string(temp_wstring.begin(), temp_wstring.end());
}

struct { //color configs
	float Black[3];
	float RGBRed[3] = { 1.0f, 0.0f, 0.0f };
	float RGBYelllow[3] = { 1.0f, 1.0f, 0.0f };
	float RGBGreen[3] = { 0.0f, 1.0f, 0.0f };
	float RGBBlue[3] = { 0.0f, 0.0f, 1.0f };
	float CMYKRed[3] = { 0.92f, 0.10f, 0.14f };
	float CMYKYellow[3] = { 1.0f, 0.94f, 0.0f };
	float CMYKGreen[3] = { 0.0f, 0.65f, 3.17f };
	float CMYKBlue[3] = { 0.18f, 0.19f, 0.57f };
	float PastelRed[3] = { 255.00f, 0.00f, 119.00f };
	float PastelRedOrange[3] = { 0.97f, 0.67f, 0.50f };
	float PastelYellowOrange[3] = { 0.99f, 0.77f, 0.53f };
	float PastelYellow[3] = { 1.0f, 0.96f, 0.6f };
	float PastelPeaGreen[3] = { 0.76f, 0.87f, 0.60f };
	float PastelYellowGreen[3] = { 0.63f, 0.82f, 0.61f };
	float PastelGreen[3] = { 0.50f, 0.79f, 0.61f };
	float PastelGreenCyan[3] = { 0.47f, 0.8f, 0.78f };
	float PastelCyan[3] = { 0.42f, 0.81f, 0.96f };
	float PastelCyanBlue[3] = { 0.49f, 0.65f, 0.85f };
	float PastelBlue[3] = { 0.51f, 0.57f, 0.79f };
	float PastelBlueViolet[3] = { 0.52f, 0.50f, 0.74f };
	float PastelViolet[3] = { 0.63f, 0.52f, 0.74f };
	float PastelVioletMagenta[3] = { 0.74f, 0.54f, 0.74f };
	float PastelMagenta[3] = { 0.95f, 0.60f, 0.75f };
	float PastelMagentaRed[3] = { 0.96f, 0.59f, 0.61f };
	float LightRed[3] = { 0.94f, 0.42f, 0.30f };
	float LightRedOrange[3] = { 0.96f, 0.55f, 0.33f };
	float LightYellowOrange[3] = { 0.98f, 0.68f, 0.36f };
	float LightYellow[3] = { 1.0f, 0.96f, 0.40f };
	float LightPeaGreen[3] = { 0.67f, 0.82f, 0.45f };
	float LightYellowGreen[3] = { 0.48f, 0.77f, 0.46f };
	float LightGreen[3] = { 0.23f, 0.72f, 0.47f };
	float LightGreenCyan[3] = { 0.10f, 0.73f, 0.70f };
	float LightCyan[3] = { 0.0f, 0.74f, 0.95f };
	float LightCyanBlue[3] = { 0.26f, 0.54f, 0.79f };
	float LightBlue[3] = { 0.33f, 0.45f, 0.72f };
	float LightBlueViolet[3] = { 0.37f, 0.36f, 0.65f };
	float LightViolet[3] = { 0.52f, 0.37f, 0.65f };
	float LightVioletMagenta[3] = { 0.65f, 0.39f, 0.65f };
	float LightMagenta[3] = { 0.94f, 0.43f, 0.66f };
	float LightMagentaRed[3] = { 0.94f, 0.42f, 0.49f };
	float Red[3] = { 0.92f, 0.10f, 0.14f };
	float RedOrange[3] = { 0.94f, 0.39f, 0.13f };
	float YellowOrange[3] = { 0.96f, 0.58f, 0.11f };
	float Yellow[3] = { 1.0f, 0.94f, 0.0f };
	float PeaGreen[3] = { 0.55f, 0.77f, 0.24f };
	float YellowGreen[3] = { 0.22f, 0.70f, 0.29f };
	float Green[3] = { 0.0f, 0.65f, 0.31f };
	float GreenCyan[3] = { 0.0f, 0.66f, 0.61f };
	float Cyan[3] = { 0.0f, 0.68f, 0.93f };
	float CyanBlue[3] = { 0.0f, 0.44f, 0.34f };
	float Blue[3] = { 0.0f, 0.44f, 0.34f };
	float BlueViolet[3] = { 0.19f, 0.19f, 0.57f };
	float Violet[3] = { 0.18f, 0.19f, 0.57f };
	float VioletMagenta[3] = { 0.57f, 0.15f, 5.63f };
	float Magenta[3] = { 0.92f, 0.0f, 0.54f };
	float MagentaRed[3] = { 0.92f, 0.07f, 0.35f };
	float DarkRed[3] = { 0.61f, 0.04f, 0.05f };
	float DarkROrange[3] = { 0.62f, 0.25f, 0.05f };
	float DarkYellowOrange[3] = { 0.53f, 0.38f, 0.03f };
	float DarkYellow[3] = { 0.67f, 0.62f, 0.0f };
	float DarkPeaGreen[3] = { 0.34f, 0.52f, 0.15f };
	float DarkYellowGreen[3] = { 0.09f, 0.48f, 0.18f };
	float DarkGreen[3] = { 0.0f, 0.44f, 0.21f };
	float DarkGreenCyan[3] = { 0.0f, 0.45f, 0.41f };
	float DarkCyan[3] = { 0.0f, 0.46f, 0.63f };
	float DarkCyanBlue[3] = { 0.0f, 0.29f, 0.50f };
	float DarkBlue[3] = { 0.0f, 0.20f, 0.44f };
	float DarkBlueViolet[3] = { 0.10f, 0.07f, 0.39f };
	float DarkViolet[3] = { 0.26f, 0.05f, 0.38f };
	float DarkVioletMagenta[3] = { 0.38f, 0.01f, 0.37f };
	float DarkMagenta[3] = { 0.61f, 0.0f, 0.36f };
	float DarkMagentaRed[3] = { 0.61f, 0.0f, 0.22f };
	float DarkerRed[3] = { 0.47f, 0.0f, 0.0f };
	float DarkerROrange[3] = { 0.48f, 0.18f, 0.0f };
	float DarkerYellowOrange[3] = { 0.49f, 0.28f, 0.0f };
	float DarkerYellow[3] = { 0.50f, 0.48f, 0.0f };
	float DarkerPeaGreen[3] = { 0.25f, 0.4f, 0.09f };
	float DarkerYellowGreen[3] = { 0.0f, 0.36f, 0.12f };
	float DarkerGreen[3] = { 0.0f, 0.34f, 0.14f };
	float DarkerGreenCyan[3] = { 0.0f, 0.34f, 0.32f };
	float DarkerCyan[3] = { 0.0f, 0.35f, 0.49f };
	float DarkerCyanBlue[3] = { 0.0f, 0.21f, 0.38f };
	float DarkerBlue[3] = { 0.0f, 0.12f, 0.34f };
	float DarkerBlueViolet[3] = { 0.05f, 0.0f, 0.29f };
	float DarkerViolet[3] = { 0.19f, 0.0f, 0.29f };
	float DarkerVioletMagenta[3] = { 0.29f, 0.0f, 0.28f };
	float DarkerMagenta[3] = { 0.48f, 0.0f, 0.27f };
	float DarkerMagentaRed[3] = { 0.47f, 0.27f, 0.14f };
	float PaleCoolBrown[3] = { 0.78f, 0.69f, 0.61f };
	float LightCoolBrown[3] = { 0.6f, 0.52f, 0.45f };
	float MiumCoolBrown[3] = { 0.45f, 0.38f, 0.34f };
	float DarkCoolBrown[3] = { 0.32f, 0.27f, 0.25f };
	float DarkerCoolBrown[3] = { 0.21f, 0.18f, 0.17f };
	float PaleWarmBrown[3] = { 0.77f, 0.61f, 0.43f };
	float LightWarmBrown[3] = { 0.65f, 0.48f, 0.32f };
	float MiumWarmBrown[3] = { 0.54f, 0.38f, 0.22f };
	float DarkWarmBrown[3] = { 0.45f, 0.29f, 0.14f };
	float DarkerWarmBrown[3] = { 0.37f, 0.22f, 0.07f };
	float purple[3] = { 144.00f, 0.00f, 255.00f };
	float white[3] = { 255.00f, 255.00f, 255.00f };
	float Sexycyan[3] = { 0.00f, 229.00f, 250.00f };
	float DarkestRed[3] = { 84.00f, 6.00f, 00.00f };
	float pink[3] = { 255.00f, 0.00f, 205.00f };
} color;


float Skeletonchik[3] = { ImColor(232, 255, 47) };
float SkeletonColor[3] = { ImColor(232, 255, 47) };

float crosshairchick[3] = { ImColor(232, 255, 47) };
float crosshaircolor[3] = { ImColor(232, 255, 47) };

float boxchick[3] = { ImColor(232, 255, 47) };
float boxcolor[3] = { ImColor(232, 255, 47) };

float threedBoxchikl[3] = { ImColor(232, 255, 47) };
float threedBoxcolor[3] = { ImColor(232, 255, 47) };

float fovchick[3] = { ImColor(232, 255, 47) };
float fovcolor[3] = { ImColor(232, 255, 47) };

float cornerchick[3] = { ImColor(232, 255, 47) };
float cornercolor[3] = { ImColor(232, 255, 47) };


void DrawSkeleton3(DWORD_PTR mesh)
{
	Vector3 vHeadBone = GetBoneWithRotation(mesh, 68);
	Vector3 vHip = GetBoneWithRotation(mesh, 2);
	Vector3 vNeck = GetBoneWithRotation(mesh, 67);
	Vector3 vUpperArmLeft = GetBoneWithRotation(mesh, 9);
	Vector3 vUpperArmRight = GetBoneWithRotation(mesh, 38);
	Vector3 vLeftHand = GetBoneWithRotation(mesh, 10);
	Vector3 vRightHand = GetBoneWithRotation(mesh, 39);
	Vector3 vLeftHand1 = GetBoneWithRotation(mesh, 11);
	Vector3 vRightHand1 = GetBoneWithRotation(mesh, 40);
	Vector3 vRightThigh = GetBoneWithRotation(mesh, 78);
	Vector3 vLeftThigh = GetBoneWithRotation(mesh, 71);
	Vector3 vRightCalf = GetBoneWithRotation(mesh, 79);
	Vector3 vLeftCalf = GetBoneWithRotation(mesh, 72);
	Vector3 vLeftFoot = GetBoneWithRotation(mesh, 74);
	Vector3 vRightFoot = GetBoneWithRotation(mesh, 81);
	Vector3 vHeadBoneOut = ProjectWorldToScreen(vHeadBone);
	Vector3 vHipOut = ProjectWorldToScreen(vHip);
	Vector3 vNeckOut = ProjectWorldToScreen(vNeck);
	Vector3 vUpperArmLeftOut = ProjectWorldToScreen(vUpperArmLeft);
	Vector3 vUpperArmRightOut = ProjectWorldToScreen(vUpperArmRight);
	Vector3 vLeftHandOut = ProjectWorldToScreen(vLeftHand);
	Vector3 vRightHandOut = ProjectWorldToScreen(vRightHand);
	Vector3 vLeftHandOut1 = ProjectWorldToScreen(vLeftHand1);
	Vector3 vRightHandOut1 = ProjectWorldToScreen(vRightHand1);
	Vector3 vRightThighOut = ProjectWorldToScreen(vRightThigh);
	Vector3 vLeftThighOut = ProjectWorldToScreen(vLeftThigh);
	Vector3 vRightCalfOut = ProjectWorldToScreen(vRightCalf);
	Vector3 vLeftCalfOut = ProjectWorldToScreen(vLeftCalf);
	Vector3 vLeftFootOut = ProjectWorldToScreen(vLeftFoot);
	Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vHipOut.x, vHipOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2],4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2],4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vLeftHandOut1.x, vLeftHandOut1.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vRightHandOut1.x, vRightHandOut1.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImVec2(vHipOut.x, vHipOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightThighOut.x, vRightThighOut.y), ImVec2(vHipOut.x, vHipOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImVec2(vRightThighOut.x, vRightThighOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftFootOut.x, vLeftFootOut.y), ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightFootOut.x, vRightFootOut.y), ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImGui::GetColorU32({ Skeletonchik[0], Skeletonchik[1], Skeletonchik[2], 4.0f }));
}

int faken_rot = 0;

float BOG_TO_GRD(float BOG) {
	return (180 / M_PI) * BOG;
}

float GRD_TO_BOG(float GRD) {
	return (M_PI / 180) * GRD;
}

float Normalize(float angle) {
	float a = (float)fmod(fmod(angle, 360.0) + 360.0, 360.0);
	if (a > 180.0f) {
		a -= 360.0f;
	}
	return a;
}

VOID CalcAngle(float* src, float* dst, float* angles) {
	float rel[3] = {
		dst[0] - src[0],
		dst[1] - src[1],
		dst[2] - src[2],
	};

	auto dist = sqrtf(rel[0] * rel[0] + rel[1] * rel[1] + rel[2] * rel[2]);
	auto yaw = atan2f(rel[1], rel[0]) * (180.0f / PI);
	auto pitch = (-((acosf((rel[2] / dist)) * 180.0f / PI) - 90.0f));

	angles[0] = Normalize(pitch);
	angles[1] = Normalize(yaw);
}


void DrawESP() {

	static const auto size = ImGui::GetIO().DisplaySize;
	static const auto center = ImVec2(size.x / 2, size.y / 2);

	//auto funcName = GetObjectFirstName(func);
	if (squarefov)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(ScreenCenterX - AimFOV, ScreenCenterY - AimFOV), ImVec2(ScreenCenterX + AimFOV, ScreenCenterY + AimFOV), ImColor(232, 255, 47), 10, 2, 2.f);
	}
	if (fovCircle)
	{
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), AimFOV, ImColor(232, 255, 47), 100.0f, 1.5f);
	}

	if (filledfov)
	{
		ImGui::GetOverlayDrawList()->AddCircleFilled(center, AimFOV, ImColor(10, 10, 10, 90), 100);
	}


	ImGui::GetOverlayDrawList()->AddText(ImVec2(22, 24), ImColor(232, 255, 47), ("eclipse.cc FREE"));

	if (fpsCounter)
	{
		char dist[64];
		XorS(frame, "\n");
		sprintf_s(dist, frame.decrypt(), 1.0f / ImGui::GetIO().DeltaTime);
		DrawText1(900, 15, dist, &Col.red);
	}
	if (hitboxpos == 0)
	{
		hitbox = BONE_HEAD_ID; //head
	}
	else if (hitboxpos == 1)
	{
		hitbox = BONE_NECK_ID; //neck
	}
	else if (hitboxpos == 2)
	{
		hitbox = BONE_CHEST_ID; //chest
	}
	else if (hitboxpos == 3)
	{
		hitbox = BONE_PELVIS_ID; //pelvis
	}

	if (aimkeypos == 0)
	{
		aimkey = 0x01;//left mouse button
	}
	else if (aimkeypos == 1)
	{
		aimkey = 0x02;//right mouse button
	}
	else if (aimkeypos == 2)
	{
		aimkey = 0x04;//middle mouse button
	}
	else if (aimkeypos == 3)
	{
		aimkey = 0x05;//x1 mouse button
	}
	else if (aimkeypos == 4)
	{
		aimkey = 0x06;//x2 mouse button
	}
	else if (aimkeypos == 5)
	{
		aimkey = 0x03;//control break processing
	}
	else if (aimkeypos == 6)
	{
		aimkey = 0x08;//backspace
	}
	else if (aimkeypos == 7)
	{
		aimkey = 0x09;//tab
	}
	else if (aimkeypos == 8)
	{
		aimkey = 0x0c;//clear
	}
	else if (aimkeypos == 9)
	{
		aimkey == 0x0D;//enter
	}
	else if (aimkeypos == 10)
	{
		aimkey = 0x10;//shift
	}
	else if (aimkeypos == 11)
	{
		aimkey = 0x11;//ctrl
	}
	else if (aimkeypos == 12)
	{
		aimkey == 0x12;//alt
	}
	else if (aimkeypos == 13)
	{
		aimkey == 0x14;//caps lock
	}
	else if (aimkeypos == 14)
	{
		aimkey == 0x1B;//esc
	}
	else if (aimkeypos == 15)
	{
		aimkey == 0x20;//space
	}
	else if (aimkeypos == 16)
	{
		aimkey == 0x30;//0
	}
	else if (aimkeypos == 17)
	{
		aimkey == 0x31;//1
	}
	else if (aimkeypos == 18)
	{
		aimkey == 0x32;//2
	}
	else if (aimkeypos == 19)
	{
		aimkey == 0x33;//3
	}
	else if (aimkeypos == 20)
	{
		aimkey == 0x34;//4
	}
	else if (aimkeypos == 21)
	{
	}
	else if (aimkeypos == 23)
	{
		aimkey == 0x37;//7
		aimkey == 0x35;//5
	}
	else if (aimkeypos == 22)
	{
		aimkey == 0x36;//6
	}
	else if (aimkeypos == 24)
	{
		aimkey == 0x38;//8
	}
	else if (aimkeypos == 25)
	{
		aimkey == 0x39;//9
	}
	else if (aimkeypos == 26)
	{
		aimkey == 0x41;//a
	}
	else if (aimkeypos == 27)
	{
		aimkey == 0x42;//b
	}
	else if (aimkeypos == 28)
	{
		aimkey == 0x43;//c
	}
	else if (aimkeypos == 29)
	{
		aimkey == 0x44;//d
	}
	else if (aimkeypos == 30)
	{
		aimkey == 0x45;//e
	}
	else if (aimkeypos == 31)
	{
		aimkey == 0x46;//f
	}
	else if (aimkeypos == 32)
	{
		aimkey == 0x47;//g
	}
	else if (aimkeypos == 33)
	{
		aimkey == 0x48;//h
	}
	else if (aimkeypos == 34)
	{
		aimkey == 0x49;//i
	}
	else if (aimkeypos == 35)
	{
		aimkey == 0x4A;//j
	}
	else if (aimkeypos == 36)
	{
		aimkey == 0x4B;//k
	}
	else if (aimkeypos == 37)
	{
		aimkey == 0x4C;//L
	}
	else if (aimkeypos == 38)
	{
		aimkey == 0x4D;//m
	}
	else if (aimkeypos == 39)
	{
		aimkey == 0x4E;//n
	}
	else if (aimkeypos == 40)
	{
		aimkey == 0x4F;//o
	}
	else if (aimkeypos == 41)
	{
		aimkey == 0x50;//p
	}
	else if (aimkeypos == 42)
	{
		aimkey == 0x51;//q
	}
	else if (aimkeypos == 43)
	{
		aimkey == 0x52;//r
	}
	else if (aimkeypos == 44)
	{
		aimkey == 0x53;//s
	}
	else if (aimkeypos == 45)
	{
		aimkey == 0x54;//t
	}
	else if (aimkeypos == 46)
	{
		aimkey == 0x55;//u
	}
	else if (aimkeypos == 47)
	{
		aimkey == 0x56;//v
	}
	else if (aimkeypos == 48)
	{
		aimkey == 0x57;//w
	}
	else if (aimkeypos == 49)
	{
		aimkey == 0x58;//x
	}
	else if (aimkeypos == 50)
	{
		aimkey == 0x59;//y
	}
	else if (aimkeypos == 51)
	{
		aimkey == 0x5A;//z
	}
	else if (aimkeypos == 52)
	{
		aimkey == 0x60;//numpad 0
	}
	else if (aimkeypos == 53)
	{
		aimkey == 0x61;//numpad 1
	}
	else if (aimkeypos == 54)
	{
		aimkey == 0x62;//numpad 2
	}
	else if (aimkeypos == 55)
	{
		aimkey == 0x63;//numpad 3
	}
	else if (aimkeypos == 56)
	{
		aimkey == 0x64;//numpad 4
	}
	else if (aimkeypos == 57)
	{
		aimkey == 0x65;//numpad 5
	}
	else if (aimkeypos == 58)
	{
		aimkey == 0x66;//numpad 6
	}
	else if (aimkeypos == 59)
	{
		aimkey == 0x67;//numpad 7
	}
	else if (aimkeypos == 60)
	{
		aimkey == 0x68;//numpad 8
	}
	else if (aimkeypos == 61)
	{
		aimkey == 0x69;//numpad 9
	}
	else if (aimkeypos == 62)
	{
		aimkey == 0x6A;//multiply
	}

	auto entityListCopy = entityList;
	float closestDistance = FLT_MAX;
	DWORD_PTR closestPawn = NULL;
	Uworld = vmread<DWORD_PTR>(baseaddy + OFFSET_UWORLD);
	DWORD_PTR Gameinstance = vmread<DWORD_PTR>(Uworld + OFFSETS::Gameinstance);
	DWORD_PTR LocalPlayers = vmread<DWORD_PTR>(Gameinstance + OFFSETS::LocalPlayers);
	Localplayer = vmread<DWORD_PTR>(LocalPlayers);
	PlayerController = vmread<DWORD_PTR>(Localplayer + OFFSETS::PlayerController);
	LocalPawn = vmread<DWORD_PTR>(PlayerController + OFFSETS::LocalPawn);
	uintptr_t pcmc = vmread<uint64_t>(PlayerController + 0x328);
	PlayerState = vmread<DWORD_PTR>(LocalPawn + OFFSETS::PlayerState);
	DWORD_PTR PlayerCameraManager = vmread<DWORD_PTR>(PlayerController + 0x340);
	PlayerCamManager = vmread<DWORD_PTR>(LocalPawn + PlayerCameraManager);
	Rootcomp = vmread<DWORD_PTR>(LocalPawn + OFFSETS::RootComponet);
	Persistentlevel = vmread<DWORD_PTR>(Uworld + OFFSETS::PersistentLevel);
	uintptr_t Crrneytwep = vmread<uintptr_t>(LocalPawn + 0x868);
	DWORD ActorCount = vmread<DWORD>(Persistentlevel + OFFSETS::ActorCount);
	DWORD_PTR AActors =vmread<DWORD_PTR>(Persistentlevel + OFFSETS::AActor);
	char bisDying = vmread<char>(Localplayer + 0x6d8);
	char bisBot = vmread<char>(PlayerState + 0x27a);
	auto bIsReloadingWeapon = vmread<bool>(Crrneytwep + 0x2B9);
	DWORD_PTR GameState = vmread<DWORD_PTR>(Uworld + 0x158);//gamestate
	DWORD_PTR PlayerArray = vmread<DWORD_PTR>(GameState + 0x2A0);//playerarray
	DWORD_PTR bInCar = vmread<DWORD_PTR>(0x2e0 + 0x2c3);
	//for (int i = 0; i < ActorCount; i++)
	bool InLobby;
	InLobby = false;
	if (!LocalPawn) InLobby = true;


	for (int i = 0; i < VisDist; i++)
	{



		auto player = vmread<uintptr_t>(PlayerArray + i * 0x8);
		auto CurrentActor = vmread<uintptr_t>(player + 0x300);//PawnPrivate

		if (!CurrentActor) {
			continue;
		}

		uintptr_t currentgun = vmread<uintptr_t>(CurrentActor + 0x828);

		auto AmmoCount = vmread<int>(currentgun + 0xB64);
		auto bIsReloadingWeapon = vmread<bool>(currentgun + 0x329);
		auto health = vmread<int>(LocalPawn + 0x34);

		int NewPlayerLocationX;
		int NewPlayerLocationY;

		//uint64_t CurrentActor = read<uint64_t>(AActors + i * OFFSETS::CurrentActor);
	   // if (read<float>(CurrentActor + OFFSETS::Revivefromdbnotime) != 10) continue;
		uint64_t CurrentActorMesh = vmread<uint64_t>(CurrentActor + OFFSETS::Mesh);
		int MyTeamId = vmread<int>(PlayerState + OFFSETS::TeamId);
		DWORD64 otherPlayerState = vmread<uint64_t>(CurrentActor + 0x290);
		int ActorTeamId = vmread<int>(otherPlayerState + OFFSETS::TeamId);

		//auto isDBNO = (read<char>(CurrentActor + 0x782) >> 4) & 1;

		auto entityListCopy = entityList;
		if (MyTeamId == ActorTeamId) continue;

		if (slefESP)
		{
			continue;
		}
		else {
			if (CurrentActor == LocalPawn) continue;
		}
		if (CurrentActor == LocalPawn) continue;
		Vector3 Headpos = GetBoneWithRotation(CurrentActorMesh, 68);
		Vector3 footpos = GetBoneWithRotation(CurrentActorMesh, 83);
		localactorpos = vmread<Vector3>(Rootcomp + 0x128);
		auto normal_head = ProjectWorldToScreen(Headpos);
		float distance = localactorpos.Distance(Headpos) / 80.f;

		Vector3 bone0 = GetBoneWithRotation(CurrentActorMesh, 0);
		Vector3 bottom = ProjectWorldToScreen(bone0);
		Vector3 Headbox = ProjectWorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z + 15));
		//Vector3 Toebox = ProjectWorldToScreen(Vector3(Toepos.x, Toepos.y, Toepos.z + 15));
		Vector3 w2shead = ProjectWorldToScreen(Headpos);
		uint64_t CurrentVehicle = vmread<uint64_t>(OFFSETS::LocalPawn + 0x2348); //FortPlayerPawn::CurrentVehicle
		uintptr_t CurrentWeapon = vmread<uintptr_t>(OFFSETS::LocalPawn + 0x868); //FortPawn::CurrentWeapon 0x868
		Vector3 Headpose = GetBoneWithRotation(CurrentActorMesh, 68);
		Vector3 bone00 = GetBoneWithRotation(CurrentActorMesh, 0);
		Vector3 bottome = ProjectWorldToScreen(bone00);
		Vector3 Headboxe = ProjectWorldToScreen(Vector3(Headpose.x, Headpose.y, Headpose.z + 15));
		Vector3 w2sheade = ProjectWorldToScreen(Headpose);
		Vector3 vHeadBone = GetBoneWithRotation(CurrentActorMesh, 68);
		Vector3 vRootBone = GetBoneWithRotation(CurrentActorMesh, 0);
		Vector3 vHeadBoneOut = ProjectWorldToScreen(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 15));
		Vector3 vRootBoneOut = ProjectWorldToScreen(vRootBone);

		float BoxHeight = (float)(Headbox.y - bottom.y);
		float BoxWidth = BoxHeight * BoxWidthValue;

		float LeftX = (float)Headbox.x - (BoxWidth / 1);
		float LeftY = (float)bottom.y;

		float CornerHeight = abs(Headbox.y - bottom.y);
		float CornerWidth = CornerHeight * BoxWidthValue;


		if (vischeck) {

			if (distance < VisDist)
			{

				if (Esp)
				{
					XorS(frame, "\n");
					char dist[64];
					sprintf_s(dist, frame.decrypt());
					DrawText1(15, 15, dist, &Col.blue);


					if (ThreeDBox && LocalPawn && PlayerController) {
						FBoxSphereBounds EntityBounds = vmread<FBoxSphereBounds>(CurrentActorMesh + 0x328); //USkinnedMeshComponent    CachedWorldSpaceBounds    0x738    FBoxSphereBounds

						auto v000 = EntityBounds.Origin - EntityBounds.BoxExtent;
						auto v111 = EntityBounds.Origin + EntityBounds.BoxExtent;

						// top box
						auto v010 = Vector3(v000.x, v111.y, v000.z);
						auto v110 = Vector3(v111.x, v111.y, v000.z);
						auto v011 = Vector3(v000.x, v111.y, v111.z);

						// bottom box
						auto v101 = Vector3(v111.x, v000.y, v111.z);
						auto v100 = Vector3(v111.x, v000.y, v000.z);
						auto v001 = Vector3(v000.x, v000.y, v111.z);

						auto w1 = ProjectWorldToScreen(Vector3(v000.x, v000.y, v000.z));
						auto w2 = ProjectWorldToScreen(Vector3(v111.x, v111.y, v111.z));

						auto s1 = ProjectWorldToScreen(v010);
						auto s2 = ProjectWorldToScreen(v110);
						auto s3 = ProjectWorldToScreen(v011);
						auto s4 = ProjectWorldToScreen(v101);
						auto s5 = ProjectWorldToScreen(v100);
						auto s6 = ProjectWorldToScreen(v001);

						if (s1.x || s1.y) {
						}
					}
				}

				if (crosshair)
				{
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2 - 10, Height / 2), ImGui::GetColorU32({ crosshairchick[0], crosshairchick[1], crosshairchick[2], 2 }));
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2 + 10, Height / 2), ImGui::GetColorU32({ crosshairchick[0], crosshairchick[1], crosshairchick[2], 2 }));
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2, Height / 2 - 10), ImGui::GetColorU32({ crosshairchick[0], crosshairchick[1], crosshairchick[2], 2 }));
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2, Height / 2 + 10), ImGui::GetColorU32({ crosshairchick[0], crosshairchick[1], crosshairchick[2], 2 }));
				}


				if (ammoESP)
				{
					char buffer[128];
					sprintf_s(buffer, "Ammo: %i", AmmoCount);


					if (AmmoCount < 100)
					{
						if (buffer != "?")
						{
							ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 20), IM_COL32(0, 0, 255, 255), buffer);
						}
					}

					if (AmmoCount > 50)
					{
						if (buffer != "?")
						{
							ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 20), IM_COL32(255, 0, 255, 255), buffer);
						}
					}
					if (AmmoCount > 30)
					{
						if (buffer != "?")
						{
							ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 20), IM_COL32(255, 0, 0, 255), buffer);
						}
					}

				}




				if (fillbox)
				{
					DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(232, 255, 47, 0), 2.5);
					DrawFilledRect(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, &Col.FiledBox);
				}

				if (Esp_box)
				{
					if (!isVisible(CurrentActorMesh))
					{
						// rgba(0, 255, 231, 0)
						DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(255, 0, 0, 255), 2.5);
					}
					else {
						// rgba(0, 255, 231, 1)
						DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(0, 255, 0, 255), 2.5);
					}

				}

				if (threeeeeed)
				{
					if (vHeadBoneOut.x != 0 || vHeadBoneOut.y != 0 || vHeadBoneOut.z != 0)
					{
						ImU32 ESPSkeleton;
						if (isVisible(CurrentActorMesh))
						{
							ESPSkeleton = ImColor(0, 255, 0);
						}
						else if (!isVisible(CurrentActorMesh))
						{
							ESPSkeleton = ImColor(255, 0, 0);
						}


						Vector3 bottom1 = ProjectWorldToScreen(Vector3(vRootBone.x + 40, vRootBone.y - 40, vRootBone.z));
						Vector3 bottom2 = ProjectWorldToScreen(Vector3(vRootBone.x - 40, vRootBone.y - 40, vRootBone.z));
						Vector3 bottom3 = ProjectWorldToScreen(Vector3(vRootBone.x - 40, vRootBone.y + 40, vRootBone.z));
						Vector3 bottom4 = ProjectWorldToScreen(Vector3(vRootBone.x + 40, vRootBone.y + 40, vRootBone.z));

						Vector3 top1 = ProjectWorldToScreen(Vector3(vHeadBone.x + 40, vHeadBone.y - 40, vHeadBone.z + 15));
						Vector3 top2 = ProjectWorldToScreen(Vector3(vHeadBone.x - 40, vHeadBone.y - 40, vHeadBone.z + 15));
						Vector3 top3 = ProjectWorldToScreen(Vector3(vHeadBone.x - 40, vHeadBone.y + 40, vHeadBone.z + 15));
						Vector3 top4 = ProjectWorldToScreen(Vector3(vHeadBone.x + 40, vHeadBone.y + 40, vHeadBone.z + 15));

						ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), ESPSkeleton, 2.0f);
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), ESPSkeleton, 2.0f);
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), ESPSkeleton, 2.0f);
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), ESPSkeleton, 2.0f);

						ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), ESPSkeleton, 2.0f);
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), ESPSkeleton, 2.0f);
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), ESPSkeleton, 2.0f);
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), ESPSkeleton, 2.0f);

						ImGui::GetOverlayDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), ESPSkeleton, 2.0f);
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), ESPSkeleton, 2.0f);
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), ESPSkeleton, 2.0f);
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), ESPSkeleton, 2.0f);
					}
				}


				if (weaponesp && settings::LocalPawn && settings::PlayerController)
				{
					auto curWep = vmread<uint64_t>(CurrentActor + 0x868);
					auto itemDef = vmread<uint64_t>(curWep + 0x3F0); // -> UFortWeaponItemDefinition -> WeaponData
					auto itemName = vmread<uintptr_t>(itemDef + 0x90);//all correct, checked not sure about second ones don't remove tier shit

					if (!itemName) continue;
					if (!itemDef) continue;
					auto bIsReloadingWeapon = vmread<bool>(curWep + 0x329);
					uint32_t StringLength = vmread<uint32_t>(itemName + 0x38);

					if (StringLength <= 32) {
						auto itemName2 = vmread<uintptr_t>(itemName + 0x30);

						auto tier = vmread<BYTE>(itemDef + 0x74);

						ImColor Color;

						if (tier == 2)
						{
							Color = IM_COL32(0, 255, 0, 255);
						}
						else if ((tier == 3))
						{
							Color = IM_COL32(0, 0, 255, 255);
						}
						else if ((tier == 4))
						{
							Color = IM_COL32(128, 0, 128, 255);
						}
						else if ((tier == 5))
						{
							Color = IM_COL32(255, 255, 0, 255);
						}
						else if ((tier == 6))
						{
							Color = IM_COL32(255, 255, 0, 255);
						}
						else if ((tier == 0) || (tier == 1))
						{
							Color = IM_COL32(255, 255, 255, 255);
						}


						if (!itemDef) continue;

						char query_cstr[100];
						size_t charsConverted;

						wchar_t* OutString = new wchar_t[uint64_t(StringLength) + 1];
						Comms::ReadProcessMemory(UDPID, (ULONG64)vmread<PVOID>(itemName + 0x30), OutString, StringLength * sizeof(wchar_t));

						wcstombs_s(&charsConverted, query_cstr, OutString, wcslen(OutString));

						const char* query_const = query_cstr;
						ImVec2 DistanceTextSize = ImGui::CalcTextSize(query_const);

						if (bIsReloadingWeapon)
							DrawString(16, bottome.x - DistanceTextSize.x / 2, bottome.y + DistanceTextSize.y / 2, &Col.SilverWhite, false, false, "Reloading");

						if (!bIsReloadingWeapon)
							DrawStringImColor(16, bottome.x - DistanceTextSize.x / 2, bottome.y + DistanceTextSize.y / 2, Color, false, false, query_cstr);
					}

				}

				if (Esp_fbox)
				{
					if (!isVisible(CurrentActorMesh))
					{
						DrawBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(255, 0, 0, 255), 2.5);
					}
					else {
						DrawBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(0, 255, 0, 255), 2.5);
					}
				}


				if (playernames)
				{
					std::string namedecrypted = decrypt_player_name(addresses::EnemyPlayerState);
					DrawString(15, Headboxe.x, Headboxe.y - 20, &Col.white, true, true, namedecrypted.c_str());
				}


				if (Esp_Distance)
				{
					char name[64];
					sprintf_s(name, "%2.fm", distance);
					DrawString(16, Headbox.x, Headbox.y - 15, &Col.SilverWhite, true, true, name);
				}


				if (Esp_line)
				{
					if (!isVisible(CurrentActorMesh))
					{
						DrawLine(Width / 2 - 0, Height / 2 - 540, bottom.x, bottom.y, &Col.red, 2.5);
					}
					else {
						DrawLine(Width / 2 - 0, Height / 2 - 540, bottom.x, bottom.y, &Col.green, 2.5);
					}
				}
			}


			if (Esp_Skeleton)
			{
				if (isVisible(CurrentActorMesh))
				{
					DrawSkeleton(CurrentActorMesh);
				}
				else
				{
					DrawSkeleton1(CurrentActorMesh);
				}



			}

		}


		else {

			if (distance < VisDist)
			{

				if (Esp)
				{
					XorS(frame, "");
					char dist[64];
					sprintf_s(dist, frame.decrypt());
					DrawText1(15, 15, dist, &Col.cyan);


					if (ThreeDBox && LocalPawn && PlayerController) {
						FBoxSphereBounds EntityBounds = vmread<FBoxSphereBounds>(CurrentActorMesh + 0x328); //USkinnedMeshComponent    CachedWorldSpaceBounds    0x738    FBoxSphereBounds

						auto v000 = EntityBounds.Origin - EntityBounds.BoxExtent;
						auto v111 = EntityBounds.Origin + EntityBounds.BoxExtent;

						// top box
						auto v010 = Vector3(v000.x, v111.y, v000.z);
						auto v110 = Vector3(v111.x, v111.y, v000.z);
						auto v011 = Vector3(v000.x, v111.y, v111.z);

						// bottom box
						auto v101 = Vector3(v111.x, v000.y, v111.z);
						auto v100 = Vector3(v111.x, v000.y, v000.z);
						auto v001 = Vector3(v000.x, v000.y, v111.z);

						auto w1 = ProjectWorldToScreen(Vector3(v000.x, v000.y, v000.z));
						auto w2 = ProjectWorldToScreen(Vector3(v111.x, v111.y, v111.z));

						auto s1 = ProjectWorldToScreen(v010);
						auto s2 = ProjectWorldToScreen(v110);
						auto s3 = ProjectWorldToScreen(v011);
						auto s4 = ProjectWorldToScreen(v101);
						auto s5 = ProjectWorldToScreen(v100);
						auto s6 = ProjectWorldToScreen(v001);

						if (s1.x || s1.y) {
							if (!isVisible(CurrentActorMesh))
							{
								DrawLine(w1.x, w1.y, s6.x, s6.y, &Col.white, 2.5);
								DrawLine(w1.x, w1.y, s5.x, s5.y, &Col.white, 2.5);
								DrawLine(s4.x, s4.y, s5.x, s5.y, &Col.white, 2.5);
								DrawLine(s4.x, s4.y, s6.x, s6.y, &Col.white, 2.5);

								DrawLine(s1.x, s1.y, s3.x, s3.y, &Col.white, 2.5);
								DrawLine(s1.x, s1.y, s2.x, s2.y, &Col.white, 2.5);
								DrawLine(w2.x, w2.y, s2.x, s2.y, &Col.white, 2.5);
								DrawLine(w2.x, w2.y, s3.x, s3.y, &Col.white, 2.5);

								DrawLine(s6.x, s6.y, s3.x, s3.y, &Col.white, 2.5);
								DrawLine(w1.x, w1.y, s1.x, s1.y, &Col.white, 2.5);
								DrawLine(s4.x, s4.y, w2.x, w2.y, &Col.white, 2.5);
								DrawLine(s5.x, s5.y, s2.x, s2.y, &Col.white, 2.5);
							}
							else {
								DrawLine(w1.x, w1.y, s6.x, s6.y, &Col.cyan, 2.5);
								DrawLine(w1.x, w1.y, s5.x, s5.y, &Col.cyan, 2.5);
								DrawLine(s4.x, s4.y, s5.x, s5.y, &Col.cyan, 2.5);
								DrawLine(s4.x, s4.y, s6.x, s6.y, &Col.cyan, 2.5);

								DrawLine(s1.x, s1.y, s3.x, s3.y, &Col.cyan, 2.5);
								DrawLine(s1.x, s1.y, s2.x, s2.y, &Col.cyan, 2.5);
								DrawLine(w2.x, w2.y, s2.x, s2.y, &Col.cyan, 2.5);
								DrawLine(w2.x, w2.y, s3.x, s3.y, &Col.cyan, 2.5);

								DrawLine(s6.x, s6.y, s3.x, s3.y, &Col.cyan, 2.5);
								DrawLine(w1.x, w1.y, s1.x, s1.y, &Col.cyan, 2.5);
								DrawLine(s4.x, s4.y, w2.x, w2.y, &Col.cyan, 2.5);
								DrawLine(s5.x, s5.y, s2.x, s2.y, &Col.cyan, 2.5);
							}
						}
					}

					if (reloadCheck)
					{
						if (bIsReloadingWeapon)
							ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 30), IM_COL32(0, 0, 255, 255), "RELOADING");
						else
							ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 30), IM_COL32(0, 0, 255, 255), "NOT RELOADING");
					}

					if (crosshair)
					{
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2 - 10, Height / 2), ImGui::GetColorU32({ crosshairchick[0], crosshairchick[1], crosshairchick[2], 2 }));
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2 + 10, Height / 2), ImGui::GetColorU32({ crosshairchick[0], crosshairchick[1], crosshairchick[2], 2 }));
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2, Height / 2 - 10), ImGui::GetColorU32({ crosshairchick[0], crosshairchick[1], crosshairchick[2], 2 }));
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2, Height / 2 + 10), ImGui::GetColorU32({ crosshairchick[0], crosshairchick[1], crosshairchick[2], 2 }));
					}

					if (ammoESP)
					{
						char buffer[128];
						sprintf_s(buffer, "Ammo: %i", AmmoCount);


						if (AmmoCount < 100)
						{
							if (buffer != "?")
							{
								ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 20), IM_COL32(0, 0, 255, 255), buffer);
							}
						}

						if (AmmoCount > 50)
						{
							if (buffer != "?")
							{
								ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 20), IM_COL32(255, 0, 255, 255), buffer);
							}
						}
						if (AmmoCount > 30)
						{
							if (buffer != "?")
							{
								ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 20), IM_COL32(255, 0, 0, 255), buffer);
							}
						}

					}




					if (fillbox)
					{
						DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(232, 255, 47, 0), 2.5f);
						DrawFilledRect(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, &Col.FiledBox);
					}

					if (Esp_box)
					{

						// rgba(0, 255, 231, 0)
						DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(232, 255, 47, 0), 2.5f);


					}


					if (outlineesp)
					{
						DrawCorneredBox(Headbox.x - CornerWidth / 2 + 1, Headbox.y, CornerWidth, CornerHeight, 1.0f, IM_COL32(232, 255, 47, 255));
						DrawCorneredBox(Headbox.x - CornerWidth / 2 - 1, Headbox.y, CornerWidth, CornerHeight, 1.0f, IM_COL32(232, 255, 47, 255));
						DrawCorneredBox(Headbox.x - CornerWidth / 2, Headbox.y + 1, CornerWidth, CornerHeight, 1.0f, IM_COL32(232, 255, 47, 255));
						DrawCorneredBox(Headbox.x - CornerWidth / 2, Headbox.y - 1, CornerWidth, CornerHeight, 1.0f, IM_COL32(232, 255, 47, 255));
					}

					if (Esp_fbox)
					{

						DrawBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, ImColor({ boxchick[0], boxchick[1], boxchick[2] }), 3.0f);

					}

					if (Esp_Distance)
					{
						char name[64];
						sprintf_s(name, "%2.fm", distance);
						DrawString(16, Headbox.x, Headbox.y - 15, &Col.SilverWhite, true, true, name);
					}


					if (threeeeeed)
					{
						if (vHeadBoneOut.x != 0 || vHeadBoneOut.y != 0 || vHeadBoneOut.z != 0)
						{
							ImU32 ESPSkeleton;
							if (isVisible(CurrentActorMesh))
							{
								ESPSkeleton = ImColor({ threedBoxchikl[0], threedBoxchikl[1], threedBoxchikl[2] });
							}
							else if (!isVisible(CurrentActorMesh))
							{
								ESPSkeleton = ImColor({ threedBoxchikl[0], threedBoxchikl[1], threedBoxchikl[2] });
							}


							Vector3 bottom1 = ProjectWorldToScreen(Vector3(vRootBone.x + 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom2 = ProjectWorldToScreen(Vector3(vRootBone.x - 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom3 = ProjectWorldToScreen(Vector3(vRootBone.x - 40, vRootBone.y + 40, vRootBone.z));
							Vector3 bottom4 = ProjectWorldToScreen(Vector3(vRootBone.x + 40, vRootBone.y + 40, vRootBone.z));

							Vector3 top1 = ProjectWorldToScreen(Vector3(vHeadBone.x + 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top2 = ProjectWorldToScreen(Vector3(vHeadBone.x - 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top3 = ProjectWorldToScreen(Vector3(vHeadBone.x - 40, vHeadBone.y + 40, vHeadBone.z + 15));
							Vector3 top4 = ProjectWorldToScreen(Vector3(vHeadBone.x + 40, vHeadBone.y + 40, vHeadBone.z + 15));

							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), ESPSkeleton, 2.0f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), ESPSkeleton, 2.0f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), ESPSkeleton, 2.0f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), ESPSkeleton, 2.0f);

							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), ESPSkeleton, 2.0f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), ESPSkeleton, 2.0f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), ESPSkeleton, 2.0f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), ESPSkeleton, 2.0f);

							ImGui::GetOverlayDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), ESPSkeleton, 2.0f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), ESPSkeleton, 2.0f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), ESPSkeleton, 2.0f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), ESPSkeleton, 2.0f);
						}
					}


					if (weaponesp && settings::LocalPawn && settings::PlayerController)
					{
						auto curWep = vmread<uint64_t>(CurrentActor + 0x868);
						auto itemDef = vmread<uint64_t>(curWep + 0x3F0); // -> UFortWeaponItemDefinition -> WeaponData
						auto itemName = vmread<uintptr_t>(itemDef + 0x90);//all correct, checked not sure about second ones don't remove tier shit

						if (!itemName) continue;
						if (!itemDef) continue;
						auto bIsReloadingWeapon = vmread<bool>(curWep + 0x329);
						uint32_t StringLength = vmread<uint32_t>(itemName + 0x38);

						if (StringLength <= 32) {
							auto itemName2 = vmread<uintptr_t>(itemName + 0x30);

							auto tier = vmread<BYTE>(itemDef + 0x74);

							ImColor Color;

							if (tier == 2)
							{
								Color = IM_COL32(0, 255, 0, 255);
							}
							else if ((tier == 3))
							{
								Color = IM_COL32(0, 0, 255, 255);
							}
							else if ((tier == 4))
							{
								Color = IM_COL32(128, 0, 128, 255);
							}
							else if ((tier == 5))
							{
								Color = IM_COL32(255, 255, 0, 255);
							}
							else if ((tier == 6))
							{
								Color = IM_COL32(255, 255, 0, 255);
							}
							else if ((tier == 0) || (tier == 1))
							{
								Color = IM_COL32(255, 255, 255, 255);
							}


							if (!itemDef) continue;

							char query_cstr[100];
							size_t charsConverted;

							wchar_t* OutString = new wchar_t[uint64_t(StringLength) + 1];
							Comms::ReadProcessMemory(UDPID, (ULONG64)vmread<PVOID>(itemName + 0x30), OutString, StringLength * sizeof(wchar_t));

							wcstombs_s(&charsConverted, query_cstr, OutString, wcslen(OutString));

							const char* query_const = query_cstr;
							ImVec2 DistanceTextSize = ImGui::CalcTextSize(query_const);

							if (bIsReloadingWeapon)
								DrawString(16, bottome.x - DistanceTextSize.x / 2, bottome.y + DistanceTextSize.y / 2, &Col.SilverWhite, false, false, "Reloading");

							if (!bIsReloadingWeapon)
								DrawStringImColor(16, bottome.x - DistanceTextSize.x / 2, bottome.y + DistanceTextSize.y / 2, Color, false, false, query_cstr);
						}

					}


					if (playernames)
					{
						std::string namedecrypted = decrypt_player_name(addresses::EnemyPlayerState);
						DrawString(15, Headboxe.x, Headboxe.y - 20, &Col.white, true, true, namedecrypted.c_str());
					}

					typedef struct _TT {
						uint64_t Actor;
						int ID;
						uint64_t mesh;
						uint64_t root;
						uint64_t PlayerState;
						uint64_t LocalPlayerState;
						uint32_t LocalTeamID;
						uint32_t TeamID69;
						Vector3 pos;
						std::string name;
						bool Spotted;
					}TT;

					std::vector<_FNlEntity> entityList;
					std::vector<TT> itemList;
					float closestDistance = FLT_MAX;
					auto itemListCopy = itemList;

					for (FNlEntity entity : entityList) {
						uintptr_t mesh = vmread<uintptr_t>(entity.Actor + 0x310);
						Vector3 Headpose = GetBoneWithRotation(mesh, 68);
						Vector3 bone00 = GetBoneWithRotation(mesh, 0);
						Vector3 bottome = ProjectWorldToScreen(bone00);
						Vector3 Headboxe = ProjectWorldToScreen(Vector3(Headpose.x, Headpose.y, Headpose.z + 15));
						Vector3 w2sheade = ProjectWorldToScreen(Headpose);
						Vector3 vHeadBone = GetBoneWithRotation(mesh, 68);
						Vector3 vRootBone = GetBoneWithRotation(mesh, 0);
						Vector3 vHeadBoneOut = ProjectWorldToScreen(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 15));
						Vector3 vRootBoneOut = ProjectWorldToScreen(vRootBone);

						float distance = settings::relativelocation.Distance(Headpose) / 100.f;


						float BoxHeight = abs(Headboxe.y - bottome.y);
						float BoxWidth = BoxHeight * 0.56;

						float LeftX = (float)Headboxe.x - (BoxWidth / 1);
						float LeftY = (float)bottome.y;

						float Height1 = abs(Headboxe.y - bottome.y);
						float Width1 = Height1 * 0.65;

						uint64_t CurrentVehicle = vmread<uint64_t>(settings::LocalPawn + 0x2348); //FortPlayerPawn::CurrentVehicle
						uintptr_t CurrentWeapon = vmread<uintptr_t>(settings::LocalPawn + 0x868); //FortPawn::CurrentWeapon 0x868
						auto selfstate = vmread<uint16_t>(settings::Localplayer + 0x2a8);

						auto UCharacterMovementComponent = vmread<uintptr_t>(settings::LocalPawn + 0x318); //DefaultPawn::MovementComponet
						if (!UCharacterMovementComponent) continue;
			
					if (weaponesp1 && settings::LocalPawn && settings::PlayerController)
					{
						auto curWep = vmread<uint64_t>(entity.Actor + 0x868);
						auto itemDef = vmread<uint64_t>(curWep + 0x3F0); // -> UFortWeaponItemDefinition -> WeaponData
						auto itemName = vmread<uintptr_t>(itemDef + 0x90);//all correct, checked not sure about second ones don't remove tier shit

						if (!itemName) continue;
						if (!itemDef) continue;
						auto bIsReloadingWeapon = vmread<bool>(curWep + 0x329);
						uint32_t StringLength = vmread<uint32_t>(itemName + 0x38);

						if (StringLength <= 32) {
							auto itemName2 = vmread<uintptr_t>(itemName + 0x30);

							auto tier = vmread<BYTE>(itemDef + 0x74);

							ImColor Color;

							if (tier == 2)
							{
								Color = IM_COL32(0, 255, 0, 255);
							}
							else if ((tier == 3))
							{
								Color = IM_COL32(0, 0, 255, 255);
							}
							else if ((tier == 4))
							{
								Color = IM_COL32(128, 0, 128, 255);
							}
							else if ((tier == 5))
							{
								Color = IM_COL32(255, 255, 0, 255);
							}
							else if ((tier == 6))
							{
								Color = IM_COL32(255, 255, 0, 255);
							}
							else if ((tier == 0) || (tier == 1))
							{
								Color = IM_COL32(255, 255, 255, 255);
							}


							if (!itemDef) continue;

							char query_cstr[100];
							size_t charsConverted;

							wchar_t* OutString = new wchar_t[uint64_t(StringLength) + 1];
							Comms::ReadProcessMemory(UDPID, (ULONG64)vmread<PVOID>(itemName + 0x30), OutString, StringLength * sizeof(wchar_t));

							wcstombs_s(&charsConverted, query_cstr, OutString, wcslen(OutString));

							const char* query_const = query_cstr;
							ImVec2 DistanceTextSize = ImGui::CalcTextSize(query_const);

							if (bIsReloadingWeapon)
								DrawString(12, bottome.x - DistanceTextSize.x / 2, bottome.y + DistanceTextSize.y / 2, &Col.SilverWhite, false, false, "Reloading");

							if (!bIsReloadingWeapon)
								DrawStringImColor(12, bottome.x - DistanceTextSize.x / 2, bottome.y + DistanceTextSize.y / 2, Color, false, false, query_cstr);
						}

					}

			}

					if (AimCheck)
					{
						uint64_t currentactormesh = vmread<uint64_t>(closestPawn + OFFSETS::Mesh);
						uint64_t CurrentHitboneMesh = vmread<uint64_t>(currentactormesh + hitbox);
						auto rootHead = GetBoneWithRotation(currentactormesh, hitbox);
						Vector3 hitbone = ProjectWorldToScreen(rootHead);
						auto isDBNO = (vmread<char>(static_cast<uintptr_t>(CurrentActorId) + 0x7C2) >> 4) & 1;

						if (closestPawn || !isDBNO) {
							if (hitbone.x != 0 && hitbone.y != 0 && hitbone.z != 0)
							{
								char dist[64];
								XorS(frame, "You are aiming at a player");
								sprintf_s(dist, frame.decrypt(), distance);
								DrawText1(900, 15, dist, &Col.purple);
							}
						}
					}

					if (triggerbot)
					{
						uint64_t currentactormesh = vmread<uint64_t>(closestPawn + OFFSETS::Mesh);
						auto rootHead = GetBoneWithRotation(currentactormesh, hitbox);
						Vector3 hitbone = ProjectWorldToScreen(rootHead);
						uint64_t currentaimbotmesh = vmread<uint64_t>(currentactormesh + hitbox);
						auto isDBNO = (vmread<char>(static_cast<uintptr_t>(CurrentActorId) + 0x7C2) >> 4) & 1;

						if (distance < AimFOV)
						{
							if (GetAsyncKeyState(VK_RBUTTON)) {
								if (closestPawn != 0)
								{
									if (hitbone.x != 0 || hitbone.y != 0 || hitbone.z != 0)
									{
										if (!isDBNO) {
											keybd_event(VK_LBUTTON, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
										}
										keybd_event(VK_LBUTTON, 0x45, KEYEVENTF_KEYUP, 0);
									}
									keybd_event(VK_LBUTTON, 0x45, KEYEVENTF_KEYUP, 0);
								}
								keybd_event(VK_LBUTTON, 0x45, KEYEVENTF_KEYUP, 0);
							}
						}
					}

					if (ThreeDBox1) { // Crashes fix it yourself 
						Vector3 vHeadBone = GetBoneWithRotation(CurrentActorMesh, 68);
						Vector3 vRootBone = GetBoneWithRotation(CurrentActorMesh, 0);
						Vector3 vHeadBoneOut = ProjectWorldToScreen(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 15));
						Vector3 vRootBoneOut = ProjectWorldToScreen(vRootBone);

						if (vHeadBoneOut.x != 0 || vHeadBoneOut.y != 0 || vHeadBoneOut.z != 0)
						{

							Vector3 bottom1 = ProjectWorldToScreen(Vector3(vRootBone.x + 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom2 = ProjectWorldToScreen(Vector3(vRootBone.x - 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom3 = ProjectWorldToScreen(Vector3(vRootBone.x - 40, vRootBone.y + 40, vRootBone.z));
							Vector3 bottom4 = ProjectWorldToScreen(Vector3(vRootBone.x + 40, vRootBone.y + 40, vRootBone.z));

							Vector3 top1 = ProjectWorldToScreen(Vector3(vHeadBone.x + 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top2 = ProjectWorldToScreen(Vector3(vHeadBone.x - 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top3 = ProjectWorldToScreen(Vector3(vHeadBone.x - 40, vHeadBone.y + 40, vHeadBone.z + 15));
							Vector3 top4 = ProjectWorldToScreen(Vector3(vHeadBone.x + 40, vHeadBone.y + 40, vHeadBone.z + 15));

							if (isVisible(CurrentActorMesh)) {
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), IM_COL32(0, 255, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), IM_COL32(0, 255, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), IM_COL32(0, 255, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), IM_COL32(0, 255, 0, 255), 0.1f);

								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), IM_COL32(0, 255, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), IM_COL32(0, 255, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), IM_COL32(0, 255, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), IM_COL32(0, 255, 0, 255), 0.1f);

								ImGui::GetOverlayDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), IM_COL32(0, 255, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), IM_COL32(0, 255, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), IM_COL32(0, 255, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), IM_COL32(0, 255, 0, 255), 0.1f);
							}
							else if (!isVisible(CurrentActorMesh)) {

								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), IM_COL32(255, 0, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), IM_COL32(255, 0, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), IM_COL32(255, 0, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), IM_COL32(255, 0, 0, 255), 0.1f);

								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), IM_COL32(255, 0, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), IM_COL32(255, 0, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), IM_COL32(255, 0, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), IM_COL32(255, 0, 0, 255), 0.1f);

								ImGui::GetOverlayDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), IM_COL32(255, 0, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), IM_COL32(255, 0, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), IM_COL32(255, 0, 0, 255), 0.1f);
								ImGui::GetOverlayDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), IM_COL32(255, 0, 0, 255), 0.1f);
							}

							if (Esp_Distance) {
								XorS(dst, "[%.fm]\n");
								char dist[64];
								sprintf_s(dist, dst.decrypt(), distance);
								DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 25), 17.0f, IM_COL32(255, 255, 255, 255), true);
							}
						}
					}



					typedef struct _LootEntity {
						std::string GNames;
						uintptr_t ACurrentItem;
					}LootEntity;
					static std::vector<LootEntity> LootentityList;

					for (LootEntity LEntityList : LootentityList) {

						if (LocalPawn)
						{
							uintptr_t ItemRootComponent = vmread<uintptr_t>(LEntityList.ACurrentItem + 0x190);
							Vector3 ItemPosition = vmread<Vector3>(ItemRootComponent + 0x128);
							float ItemDist = localactorpos.Distance(ItemPosition) / 100.f;
							std::string null = ("");
							auto IsSearched = vmread<BYTE>((uintptr_t)LEntityList.ACurrentItem + 0xfb9);
							if (IsSearched >> 7 & 1) continue;
							Vector3 ChestPosition;
							ChestPosition = ProjectWorldToScreen(ItemPosition);
							Vector3 VehiclePosition = ProjectWorldToScreen(ItemPosition);

							if (world_chests)
							{
								if (strstr(LEntityList.GNames.c_str(), ("Tiered_Chest")) && world_chests)
								{
									if (ItemDist < WorldEspDistance)
									{
										Vector3 ChestPosition;
										ChestPosition = ProjectWorldToScreen(ItemPosition);
										std::string Text = null + ("Chest [") + std::to_string((int)ItemDist) + ("m]");
										DrawString(14, ChestPosition.x, ChestPosition.y, &Col.orange_, true, true, Text.c_str());
									}
								}
							}
							else if (world_supplydrop && strstr(LEntityList.GNames.c_str(), ("AthenaSupplyDrop_C")))
							{
								if (ItemDist < WorldEspDistance) {
									Vector3 ChestPosition;
									ChestPosition = ProjectWorldToScreen(ItemPosition);

									std::string Text = null + ("Supply Drop [") + std::to_string((int)ItemDist) + ("m]");
									DrawString(14, ChestPosition.x, ChestPosition.y, &Col.blue, true, true, Text.c_str());

								}
							}
							else if (world_ammo && strstr(LEntityList.GNames.c_str(), ("Tiered_Ammo")))
							{
								if (ItemDist < WorldEspDistance) {
									Vector3 ChestPosition;
									ChestPosition = ProjectWorldToScreen(ItemPosition);
									std::string Text = null + ("Ammo Box [") + std::to_string((int)ItemDist) + ("m]");
									DrawString(14, ChestPosition.x, ChestPosition.y, &Col.white, true, true, Text.c_str());

								}
							}
							//NPC_Pawn_Irwin_Predator_Robert_C   NPC_Pawn_Irwin_Prey_Burt_C				NPC_Pawn_Irwin_Simple_Smackie_C			NPC_Pawn_Irwin_Predator_Grandma_C			NPC_Pawn_Irwin_Simple_Avian_Crow_C
							else if ((world_animal && (strstr(LEntityList.GNames.c_str(), ("NPC_Pawn_Irwin_Predator_Robert_C"))) || (strstr(LEntityList.GNames.c_str(), ("NPC_Pawn_Irwin_Prey_Burt_C"))) || (strstr(LEntityList.GNames.c_str(), ("NPC_Pawn_Irwin_Simple_Smackie_C"))) || (strstr(LEntityList.GNames.c_str(), ("NPC_Pawn_Irwin_Prey_Nug_C"))) || (strstr(LEntityList.GNames.c_str(), ("NPC_Pawn_Irwin_Predator_Grandma_C"))))) {
								if (ItemDist < WorldEspDistance) {
									std::string Text = null + ("Animal [") + std::to_string((int)ItemDist) + ("m]");
									DrawString(14, VehiclePosition.x, VehiclePosition.y, &Col.white, true, true, Text.c_str());
								}
							}
							else if ((world_boat && (strstr(LEntityList.GNames.c_str(), ("MeatballVehicle_L_C")))))
							{
								if (ItemDist < WorldEspDistance) {
									Vector3 VehiclePosition = ProjectWorldToScreen(ItemPosition);
									std::string Text = null + ("Boat [") + std::to_string((int)ItemDist) + ("m]");
									DrawString(14, VehiclePosition.x, VehiclePosition.y, &Col.white, true, true, Text.c_str());
								}
							}
							else if ((world_car && (strstr(LEntityList.GNames.c_str(), ("Vehicl")) || strstr(LEntityList.GNames.c_str(), ("Valet_Taxi")) || strstr(LEntityList.GNames.c_str(), ("Valet_BigRig")) || strstr(LEntityList.GNames.c_str(), ("Valet_BasicTr")) || strstr(LEntityList.GNames.c_str(), ("Valet_SportsC")) || strstr(LEntityList.GNames.c_str(), ("Valet_BasicC")))))
							{
								if (ItemDist < WorldEspDistance) {
									Vector3 VehiclePosition = ProjectWorldToScreen(ItemPosition);
									std::string Text = null + ("Vehicle [") + std::to_string((int)ItemDist) + ("m]");
									DrawString(14, VehiclePosition.x, VehiclePosition.y, &Col.red, true, true, Text.c_str());
								}
							}
							else if (world_weapon && strstr(LEntityList.GNames.c_str(), ("FortPickupAthena")) || strstr(LEntityList.GNames.c_str(), ("Fort_Pickup_Creative_C")))
							{
								if (ItemDist < WorldEspDistance) {

									auto definition = vmread<uint64_t>(LEntityList.ACurrentItem + 0x2f8 + 0x18);
									BYTE tier = vmread<BYTE>(definition + 0x70);

									RGBA Color, RGBAColor;
									Vector3 ChestPosition = ProjectWorldToScreen(ItemPosition);

									auto DisplayName = vmread<uint64_t>(definition + 0x90);
									auto WeaponLength = vmread<uint32_t>(DisplayName + 0x38);
									wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

									std::string Text = wchar_to_char(WeaponName);
									std::string wtf2 = Text + (" [ ") + std::to_string((int)ItemDist) + ("M ]");
									if (tier == 2 && (uncommon))
									{
										Color = Col.darkgreen;
									}
									else if ((tier == 3) && (rare))
									{
										Color = Col.blue;
									}
									else if ((tier == 4) && (epic))
									{
										Color = Col.purple;
									}
									else if ((tier == 5) && (legendary))
									{
										Color = Col.orange;
									}
									else if ((tier == 6) && (mythic))
									{
										Color = Col.yellow;
									}
									else if ((tier == 0) || (tier == 1) && (common))
									{
										Color = Col.gray;
									}

									DrawString(14, ChestPosition.x, ChestPosition.y, &Color, true, true, wtf2.c_str());

								}
							}
						}
					}

					if (ammmoesp)
					{
						auto AmmoCount = vmread<int>(CurrentWeapon + 0xB64);

						char buffer[128];
						sprintf_s(buffer, "Ammo: %i", AmmoCount);
						if (buffer != "?")
						{
							ImVec2 DistanceTextSize = ImGui::CalcTextSize(buffer);
							DrawString(12, Headbox.x - DistanceTextSize.x / 2, Headbox.y + DistanceTextSize.y / 2 + 20, &Col.SilverWhite, false, false, buffer);
						}
					}

					if (Esp_line)
					{

						DrawLine(Width / 2 - 0, Height / 2 - 540, bottom.x, bottom.y, &Col.blue, 2.5);

					}

					if (headesp) {
						ImVec2 head_at_screen = ImVec2(Headbox.x, Headbox.y);
						ImGui::GetOverlayDrawList()->AddCircle(head_at_screen, 7 / distance, ImColor(255, 255, 255, 255), 0, 3);
					}

					if (Esp_Skeleton)
					{
						DrawSkeleton3(CurrentActorMesh);


					}

				}
			}
		}


		auto dx = w2shead.x - (Width / 2);
		auto dy = w2shead.y - (Height / 2);
		auto dist = sqrtf(dx * dx + dy * dy);
		if (!isVisible(CurrentActorMesh)) {

			if (dist < AimFOV && dist < closestDistance) {
				closestDistance = dist;
				closestPawn = CurrentActor;



			}
		}

		else
		{
			if (isVisible(CurrentActorMesh)) {

				if (dist < AimFOV && dist < closestDistance) {
					closestDistance = dist;
					closestPawn = CurrentActor;

				}
			}
		}
	}

	if (Aimbot)
	{
		XorS(frame, "\n");
		char dist[64];
		sprintf_s(dist, frame.decrypt());
		DrawText1(15, 20, dist, &Col.cyan);

		if (Aimbot && closestPawn && GetAsyncKeyState(hotkeys::aimkey) < 0 or Controller::IsPressingLeftTrigger()) {
			AimAt(closestPawn);
		}
	}
	Sleep(0.1);
}


DWORD m_dRainbowHex;
float m_fRainbowRGB[3];

void UpdateRainbow(float* primaryRGB, float length, float hue) {
	//Blume's rainbow rewrite

	float primaryHSB[3];
	ImGui::ColorConvertRGBtoHSV(primaryRGB[0], primaryRGB[1], primaryRGB[2], primaryHSB[0], primaryHSB[1], primaryHSB[2]);

	float lengthMs = length * 1000.0f;
	float timedHue = GetTickCount() % (long int)(lengthMs) / lengthMs;

	//"hue" for "indexedHue"
	float hue2 = timedHue + hue * 0.05f + 1.f;
	ImGui::ColorConvertHSVtoRGB(hue2, primaryHSB[1], primaryHSB[2], m_fRainbowRGB[0], m_fRainbowRGB[1], m_fRainbowRGB[2]);
	m_dRainbowHex = D3DCOLOR_RGBA((int)m_fRainbowRGB[0], (int)m_fRainbowRGB[1], (int)m_fRainbowRGB[2], 255);
}



void render() {

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		ShowMenu = !ShowMenu;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ShowMenu)
	{
		XorS(menu, " eclipse.cc");
		XorS(enable_esp, "Enable Esp");
		XorS(box_esp, "2D Cornered Boxes");
		XorS(fullboxesp, "2D Normal Boxes");
		XorS(snapline, "Snap Lines");
		XorS(espdis, "Esp Distance");
		XorS(espvis, "Esp Visable");
		XorS(es5, "Max Visuals Distance");
		XorS(espskel, "Skeletons");

		XorS(aim1, "Aimbot");
		XorS(aim2, "Aimbot Key");
		XorS(aim3, " Aimbones");
		XorS(aim7, "Enable FOV");
		XorS(aim6, "Aimbot FOV");







		static int maintabs;
		ImGui::SetNextWindowSize({ 500, 700 });

		if (ShowMenu == true)
		{
			static int tabs = 0;
			static int counter = 0;

			static bool boxesp = false;
			static bool headesp = false;
			static bool heatlhesp = false;
			static bool filledboxesp = false;
			static bool tracelinesp = false;
			static bool nameesp = false;
			static bool esppreview = true;


			float CenterX = GetSystemMetrics(0) / 3;
			float CenterY = GetSystemMetrics(0) / 5.5;

			float CenterX2 = GetSystemMetrics(0) / 4.2;
			float CenterY2 = GetSystemMetrics(0) / 4.25;







			//Snowflake::Update(snow, Snowflake::vec3(mouse.x, mouse.y)/*mouse x and y*/, Snowflake::vec3(rc.left, rc.top)/*hWnd x and y positions*/); // you can change a few things inside the update function





			ImGui::SetNextWindowSize({ 700, 450 });
			ImGui::Begin("eclipse.cc", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);


			if (ShowMenu)
			{
				if (particles)
				{
					ImDrawList* draw;
					draw = ImGui::GetWindowDrawList();
					ImVec2 screenSize = ImGui::GetIO().DisplaySize;
					ImGui::Particles(draw, screenSize);
				}
			}

			ImGui::Columns(2);
			ImGui::SetColumnOffset(1, 170);
			{

				static ImVec4 active = imguipp::to_vec4(232, 255, 47, 255);
				static ImVec4 inactive = imguipp::to_vec4(24, 24, 26, 255);

				ImGui::PushStyleColor(ImGuiCol_Button, maintabs == 0 ? active : inactive);
				if (ImGui::Button(ICON_FA_CROSSHAIRS" Aim", ImVec2(170 - 15, 41)))
				{
					maintabs = 0;
				}

				ImGui::Spacing();
				ImGui::PushStyleColor(ImGuiCol_Button, maintabs == 1 ? active : inactive);
				if (ImGui::Button(ICON_FA_EYE" Visuals", ImVec2(170 - 15, 41)))
				{
					maintabs = 1;
				}

				ImGui::Spacing();
				ImGui::PushStyleColor(ImGuiCol_Button, maintabs == 4 ? active : inactive);
				if (ImGui::Button(ICON_FA_PAINT_BRUSH" Config", ImVec2(170 - 15, 41)))
				{
					maintabs = 4;
				}

				ImGui::PopStyleColor(3);

				ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 100);
				if (ImGui::Button("Unload", ImVec2(170 -15, 41)))
				{
					exit(0);
				}
				ImGui::Spacing();
			}

			ImGui::NextColumn();

			if (maintabs == 4)
			{
				ImGui::ColorEdit3((" Full Box "), boxchick);
				ImGui::ColorEdit3((" Cornered Box  "), cornerchick);
				ImGui::ColorEdit3((" FOV Circle "), fovchick);
				ImGui::ColorEdit3((" Crosshair"), crosshairchick);
				ImGui::Checkbox("Menu Particles", &particles);
			}

			if (maintabs == 0)
			{
				ImGui::Text(("Aimbot Settings:"));
				ImGui::Checkbox("Softaim", &Aimbot);
				ImGui::Checkbox(("Draw FOV Circle"), &fovCircle);
				ImGui::Checkbox("Filled FOV", &filledfov);
				ImGui::Checkbox("Triggerbot", &triggerbot);
				ImGui::Checkbox("Aimcheck", &AimCheck);
				ImGui::SameLine();
				ImGui::TextColored(ImColor(232, 255, 47, 255), ("[?]"));
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text(("When aiming at an enemy it will tell the distance and the body part"));
					ImGui::EndTooltip();
				}
				ImGui::Text((" "));
				ImGui::Text(("Aim Key:"));
				ImGui::SameLine();
				HotkeyButton(hotkeys::aimkey, ChangeKey, keystatus);
				ImGui::Text((" "));
				ImGui::Combo(aim3.decrypt(), &hitboxpos, hitboxes, sizeof(hitboxes) / sizeof(*hitboxes));
				ImGui::SliderInt(" Smoothness", &bA1mb0tSm00th1ngV4lue, 5.0f, 20.0f);
				ImGui::SliderInt(" FOV", &AimFOV, 1.f, 350.f);

			}
			if (maintabs == 1)
			{
				ImGui::Text(("Visuals:"));
				ImGui::Checkbox(("Filled Box"), &fillbox);
				ImGui::Checkbox(("Outlined Box"), &Esp_fbox);
				ImGui::Checkbox("Cornered Box ESP", &Esp_box);
				ImGui::Checkbox("Skeleton", &Esp_Skeleton1);
				ImGui::Checkbox("Snaplines", &Esp_line);
				ImGui::Checkbox("Playernames", &playernames);
				ImGui::Checkbox("Distance", &Esp_Distance);
				ImGui::Checkbox("Current weapon/harvesting tool", &weaponesp1);
				ImGui::Checkbox("Crosshair", &crosshair);
	
			}
			ImGui::End();
		}
	}

	DrawESP();

	ImGui::EndFrame();
	D3dDevice->SetRenderState(D3DRS_ZENABLE, false);
	D3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	D3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	D3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (D3dDevice->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		D3dDevice->EndScene();
	}
	HRESULT result = D3dDevice->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && D3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		D3dDevice->Reset(&d3dpp);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}


MSG Message = { NULL };


void xMainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, Window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();

		if (hwnd_active == hwnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(Window, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(hwnd, &rc);
		ClientToScreen(hwnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = hwnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			d3dpp.BackBufferWidth = Width;
			d3dpp.BackBufferHeight = Height;
			SetWindowPos(Window, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			D3dDevice->Reset(&d3dpp);
		}
		render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DestroyWindow(Window);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		xShutdown();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (D3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			d3dpp.BackBufferWidth = LOWORD(lParam);
			d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = D3dDevice->Reset(&d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void xShutdown()
{
	TriBuf->Release();
	D3dDevice->Release();
	p_Object->Release();

	DestroyWindow(Window);
	UnregisterClass((L"Medal"), NULL);
}
