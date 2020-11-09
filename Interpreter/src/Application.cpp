#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <SDL.h>

#include "Interpreter.h"

#define WIDTH  640
#define HEIGHT 320

#define WHITE  0xFFFFFF

struct Timer
{
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> end;
	std::chrono::duration<float> duration;

	Timer()
	{
		start = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;
		float ms = duration.count() * 1000.0f;
		std::cout << "Timer " << ms << "ms" << std::endl;
	}
};

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "Could not SDL_Init: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* window;
	SDL_Renderer* renderer;

	if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) != 0)
	{
		std::cout << "Could not SDL_CreateWindowAndRenderer: " << SDL_GetError() << std::endl;
		return 1;
	}

	if (window == NULL)
	{
		std::cout << "Could not SDL_CreateWindow window: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_SetWindowTitle(window, "Interpreter");

	if (renderer == NULL)
	{
		std::cout << "Could not SDL_CreateWindow renderer: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_AudioSpec wavSpec;
	Uint32 wavLength;
	Uint8* wavBuffer;

	SDL_LoadWAV("s.wav", &wavSpec, &wavBuffer, &wavLength);

	SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

	if (deviceId == 0)
	{
		std::cout << "Could not SDL_OpenAudioDevice: " << SDL_GetError() << std::endl;
		return 1;
	}

	std::ifstream input((char*)argv[1], std::ios::binary);

	std::vector<char> bytes(
		(std::istreambuf_iterator<char>(input)),
		(std::istreambuf_iterator<char>()));

	input.close();

	Interpreter i = Interpreter();
	i.load_rom(reinterpret_cast<char*>(bytes.data()), bytes.size());

	unsigned char keypad[16] = {
						SDLK_x, SDLK_1, SDLK_2, SDLK_3,
						SDLK_q, SDLK_w, SDLK_e, SDLK_a,
						SDLK_s, SDLK_d, SDLK_z, SDLK_c,
						SDLK_4, SDLK_r, SDLK_f, SDLK_v
	};

	SDL_Texture* texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		64, 32);

	if (texture == NULL)
	{
		std::cout << "Could not SDL_CreateTexture: " << SDL_GetError() << std::endl;
		return 1;
	}

	int pixels[2048];
	SDL_Event e;

	while (true)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				exit(0);


			if (e.type == SDL_KEYUP)
			{
				for (int j = 0; j < 16; ++j)
				{
					if (e.key.keysym.sym == keypad[j])
						i.key_buffer[j] = 0;
				}
			}

			if (e.type == SDL_KEYDOWN)
			{
				for (int j = 0; j < 16; ++j)
				{
					if (e.key.keysym.sym == keypad[j])
						i.key_buffer[j] = 1;
				}
			}
		}

		{
			Timer timer;
			i.cycle();
		}

		if (i.should_draw)
		{
			i.should_draw = false;

			for (int j = 0; j < 2048; j++)
				pixels[j] = WHITE * (i.buffer[j] != 0);

			SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(int));

			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}

		if (i.st > 1)
		{
			int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
			SDL_PauseAudioDevice(deviceId, 0);
		}

		std::this_thread::sleep_for(std::chrono::microseconds(800));
	}

	return 0;
}