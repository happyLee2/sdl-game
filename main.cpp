#include <iostream>
#include "SDL.h"
#include "SDL_image.h"


int main(int argc, char* argv[])
{

	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 640;

	SDL_Window* pWindow = nullptr;
	SDL_Renderer* pRenderer = nullptr;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "Initialized! " << std::endl;
		pWindow = SDL_CreateWindow("Game", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
		if (pWindow)
		{
			std::cout << "Window is created!" << std::endl;
		}
		
		pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
		if (pRenderer)
		{
			SDL_SetRenderDrawColor(pRenderer, 0, 255, 25, 255);
		}
	}

	SDL_Surface* pSurface = IMG_Load("assets/images/background/skybackground.png");
	if (pSurface == NULL)
	{
		std::cout << "load_texture::IMG_Load( " << "assets/images/background/skybackground.png" << " )" << std::endl;
		return false;
	}
	SDL_Texture* ptBackground = SDL_CreateTextureFromSurface(pRenderer, pSurface);
	if (ptBackground == NULL)
	{
		std::cout << "Error SDL_CreateTextureFromSurface(Game::_pRenderer, pSurface);" << std::endl;
		return false;
	}

	SDL_FreeSurface(pSurface);
	SDL_Rect stt = { 0, 0, 800, 640 };
	SDL_Rect arv = { 0, 0, 800, 640 };

	SDL_Surface* pPlane = IMG_Load("assets/images/player character/player_character.png");
	SDL_Texture* ptPlane = SDL_CreateTextureFromSurface(pRenderer, pPlane);
	SDL_FreeSurface(pPlane);
	SDL_Rect sttPlane = { 0, 0, 64, 64 };
	SDL_Rect arvPlane = { 380, 500, 64, 64 };

	SDL_Surface* pEnemy = IMG_Load("assets/images/enemy/enemy.png");
	SDL_Texture* ptEnemy = SDL_CreateTextureFromSurface(pRenderer, pEnemy);
	SDL_FreeSurface(pEnemy);
	SDL_Rect sttEnemy = { 0, 0, 87, 68 };
	SDL_Rect arvEnemy = { 360, 100, 87, 68 };

	SDL_Surface* pEnemy2 = IMG_Load("assets/images/enemy/enemy2.png");
	SDL_Texture* ptEnemy2 = SDL_CreateTextureFromSurface(pRenderer, pEnemy2);
	SDL_FreeSurface(pEnemy2);
	SDL_Rect sttEnemy2 = { 0, 0, 87, 68 };
	SDL_Rect arvEnemy2 = { 500, 100, 87, 68 };

	SDL_Surface* pEnemy3 = IMG_Load("assets/images/enemy/enemy3.png");
	SDL_Texture* ptEnemy3 = SDL_CreateTextureFromSurface(pRenderer, pEnemy3);
	SDL_FreeSurface(pEnemy3);
	SDL_Rect sttEnemy3 = { 0, 0, 87, 68 };
	SDL_Rect arvEnemy3 = { 150, 150, 87, 68 };

	SDL_Surface* pNormalSkill = IMG_Load("assets/images/skill/modifiedplayer_skill.png");
	SDL_Texture* ptNormalSkill = SDL_CreateTextureFromSurface(pRenderer, pNormalSkill);
	SDL_FreeSurface(pNormalSkill);
	SDL_Rect sttSkill = { 0, 0, 64, 126 };
	SDL_Rect arvSkill = { 380, 350, 64, 126 };

	SDL_Surface* pRock = IMG_Load("assets/images/obstacle/rock.png");
	SDL_Texture* ptRock = SDL_CreateTextureFromSurface(pRenderer, pRock);
	SDL_FreeSurface(pRock);
	SDL_Rect sttRock = { 0, 0, 32, 32 };
	SDL_Rect arvRock = { 50, 100, 32, 32 };

	SDL_Surface* pRock2 = IMG_Load("assets/images/obstacle/rock2.png");
	SDL_Texture* ptRock2 = SDL_CreateTextureFromSurface(pRenderer, pRock2);
	SDL_FreeSurface(pRock2);
	SDL_Rect sttRock2 = { 0, 0, 100, 100 };
	SDL_Rect arvRock2 = { 500, 500, 100, 100 };

	while (true)
	{
		SDL_RenderClear(pRenderer);

		SDL_RenderCopy(pRenderer, ptBackground, &stt, &arv);
		SDL_RenderCopy(pRenderer, ptPlane, &sttPlane, &arvPlane);
		SDL_RenderCopy(pRenderer, ptEnemy, &sttEnemy, &arvEnemy);
		SDL_RenderCopy(pRenderer, ptEnemy2, &sttEnemy2, &arvEnemy2);
		SDL_RenderCopy(pRenderer, ptEnemy3, &sttEnemy3, &arvEnemy3);
		SDL_RenderCopy(pRenderer, ptNormalSkill, &sttSkill, &arvSkill);
		SDL_RenderCopy(pRenderer, ptRock, &sttRock, &arvRock);
		SDL_RenderCopy(pRenderer, ptRock2, &sttRock2, &arvRock2);

		SDL_RenderPresent(pRenderer);
	}

	return 0;
}