#include <windows.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
const int zero = 0;
const float midrad = 15.0f;
 int windowheight = 600;
 int windowwidth = 800;
 const int MARGIN = 45;
 const int PADDING = 20;
class Particle
{
public:
	float x, y;
	float vx, vy;
	float radius;
	COLORREF color;
	Particle(float startX, float startY, float startVX, float startVY,
		float startRadius, COLORREF startColor)
		: x(startX), y(startY),
		vx(startVX), vy(startVY),
		radius(startRadius), color(startColor) {}
};
class barrier
{
public:
	float x1, y1;
	float width, height;
	COLORREF color1;
	barrier(float startx1, float starty1,float startwidth, float startheight, float startcolor1)
		: x1(startx1), y1(starty1),
		width(startwidth), height(startheight), color1(startcolor1) {}
};
std::vector<Particle> particles;
std::vector<barrier> barriers;
const float ballradius = 15.0f;
HDC     backDC = nullptr;   
HBITMAP backBmp = nullptr;   
HBITMAP oldBmp = nullptr;

void InitBackBuffer(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	backDC = CreateCompatibleDC(hdc);
	backBmp = CreateCompatibleBitmap(hdc, windowwidth, windowheight);
	oldBmp = (HBITMAP)SelectObject(backDC, backBmp);
	ReleaseDC(hwnd, hdc);
}
void Initbarrier()
{
	barriers.clear();
	for (int i = 0; i < 25; ++i) {
		float x1 = (float)(rand() % windowwidth);
		float y1 = (float)(rand() % windowheight);
		float width = 40;
		float height = 40;
		if (x1 < MARGIN) {
			i--; continue;
		}
		if (y1 < MARGIN) {
			i--; continue;
		}
			if (x1 + width > windowwidth - MARGIN) {
				i--; continue;
			}
			

			if (y1 + height > windowheight - MARGIN) { i--; continue; }
		bool overlaps = false;
		for (auto& other : barriers) {
			if (x1 < other.x1 + other.width + PADDING &&
				x1 + width + PADDING > other.x1 &&
				y1 < other.y1 + other.height + PADDING &&
				y1 + height + PADDING > other.y1) {

				overlaps = true;
				break;
			}
		}

		// Если не пересекается — добавляем
		if (!overlaps) {
			COLORREF z = RGB(255, 0, 0);
			barriers.push_back(barrier(x1, y1, width, height, z));
		}
	
	}
}
void InitParticles(){
	particles.clear();

for (int i = 0; i < 10; ++i) {

	float x = (float)(rand() % windowwidth);
	float y = 0;
	float r = 8.0f + (rand() % 15);
	float coef = r / midrad;
	float vx = (((rand() % 200) - 100) / 20.0f);
	float vy = (((rand() % 200) - 100) / 20.0f)*1000;             
	COLORREF c = RGB(rand() % 256, rand() % 256, rand() % 256);
	particles.push_back(Particle(x, y, vx, vy, r, c));
}
}
void Update() {
	for (auto& p : particles) {
		const float cof = 0.85f;
		p.vy += 0.3f*(p.radius/midrad);         // гравитация
		p.x += p.vx;
		p.y += p.vy;		
		// Левая стена
		if ((p.x - p.radius) < zero) {
			p.x = p.radius;
			p.vx = -p.vx*cof;
		}
		// Правая стена
		if (p.x + p.radius > windowwidth) {
			p.x = windowwidth - p.radius;
			p.vx = -p.vx*cof;
		}
		// Верхняя стена
		if ((p.y - p.radius) < 0) {
			p.y = p.radius;
			p.vy = -p.vy*cof;
		}
		// Нижняя стена
		if (p.y + p.radius > windowheight) {
			p.y = windowheight - p.radius;
			p.vy = -p.vy*cof;
			p.vx *= 0.9f;
			if (fabsf(p.vy) < 1.0f) p.vy = 0.0f;
			if (fabsf(p.vx) < 0.3f) p.vx = 0.0f;

		}
 
		for (auto& b : barriers) {
			// Левая грань куба
			if (p.x + p.radius > b.x1 && p.x < b.x1 + b.width &&
				p.y + p.radius > b.y1 && p.y - p.radius < b.y1 + b.height) {
				if (p.x < b.x1) {
					p.x = b.x1 - p.radius;
					p.vx = -fabsf(p.vx)*cof;
				}
			}
			// Правая грань
			if (p.x - p.radius < b.x1 + b.width && p.x > b.x1 &&
				p.y + p.radius > b.y1 && p.y - p.radius < b.y1 + b.height) {
				if (p.x > b.x1 + b.width) {
					p.x = b.x1 + b.width + p.radius;
					p.vx = fabsf(p.vx)*cof;
				}
			}
			// Верхняя грань
			if (p.y + p.radius > b.y1 && p.y < b.y1 + b.height &&
				p.x + p.radius > b.x1 && p.x - p.radius < b.x1 + b.width) {
				if (p.y < b.y1) {
					p.y = b.y1 - p.radius;
					p.vy = -fabsf(p.vy)*cof;
				}
			}
			// Нижняя грань
			if (p.y - p.radius < b.y1 + b.height && p.y > b.y1 &&
				p.x + p.radius > b.x1 && p.x - p.radius < b.x1 + b.width) {
				if (p.y > b.y1 + b.height) {
					p.y = b.y1 + b.height + p.radius;
					p.vy = fabsf(p.vy)*cof;
				}
				
			}
		}
	}
}

void Draw() {

	HBRUSH bgBrush = CreateSolidBrush(RGB(20, 20, 40));
	RECT FullRect = { 0, 0, windowwidth, windowheight };
	FillRect(backDC, &FullRect, bgBrush);
	DeleteObject(bgBrush);
	for (auto& b : barriers)
	{
		HBRUSH brush1 = CreateSolidBrush(b.color1);
		HPEN pen1 = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		HGDIOBJ oldBrush = SelectObject(backDC, brush1);
		HGDIOBJ oldPen = SelectObject(backDC, pen1);

		Rectangle(backDC,
			(int)b.x1, (int)b.y1,
			(int)(b.x1 + b.width), (int)(b.y1 + b.height));

		SelectObject(backDC, oldBrush);
		SelectObject(backDC, oldPen);
		DeleteObject(brush1);
		DeleteObject(pen1);
	}
	for (auto& p : particles) {
		HBRUSH brush = CreateSolidBrush(p.color);
		HPEN   pen = CreatePen(PS_SOLID, 2, RGB(0,0,0));
		HGDIOBJ oldBrush = SelectObject(backDC, brush);
		HGDIOBJ oldPen = SelectObject(backDC, pen);

		Ellipse(backDC,
			(int)(p.x - p.radius), (int)(p.y - p.radius),
			(int)(p.x + p.radius), (int)(p.y + p.radius));

		SelectObject(backDC, oldBrush);
		SelectObject(backDC, oldPen);
		DeleteObject(brush);
		DeleteObject(pen);
	}
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_CREATE:
		InitBackBuffer(hwnd);
		InitParticles();
		Initbarrier();
		SetTimer(hwnd, 1, 7, nullptr); 
		return 0;
	case WM_TIMER:
		Update();        
		InvalidateRect(hwnd, nullptr, FALSE); 
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		Draw();                        
		BitBlt(hdc, 0, 0, windowwidth, windowheight,
			backDC, 0, 0, SRCCOPY);  

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		return 0;


	case WM_DESTROY:
		KillTimer(hwnd, 1);
		SelectObject(backDC, oldBmp);
		DeleteObject(backBmp);
		DeleteDC(backDC);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
	windowwidth = GetSystemMetrics(SM_CXSCREEN);
	windowheight = GetSystemMetrics(SM_CYSCREEN);
	srand((unsigned)time(nullptr));
	const wchar_t CLASS_NAME[] = L"ParticleSimClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr; 

	RegisterClass(&wc);

	RECT rect = { 0, 0, windowwidth, windowheight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	int screenW = GetSystemMetrics(SM_CXSCREEN); // Ширина экрана
	int screenH = GetSystemMetrics(SM_CYSCREEN); // Высота экрана
	HWND hwnd = CreateWindowEx(
		0, CLASS_NAME, L"Particle Simulator",
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		screenW, screenH,
		nullptr,
		nullptr, hInstance, nullptr
	);

	ShowWindow(hwnd, nCmdShow);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
