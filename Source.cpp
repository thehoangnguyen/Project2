#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <SDL_mixer.h>

using namespace std;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 672;
int life = 5;
int score_ = -5;


class LTexture
{
	public:
		LTexture();

		~LTexture();

		bool loadFromFile( std::string path );

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
		//Creates image from font string
		bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

		void free();

		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
		void setColor(Uint8 red, Uint8 green, Uint8 blue );
		void setBlendMode (SDL_BlendMode blending );
		void setAlpha ( Uint8 alpha);

		int getWidth();
		int getHeight();

	private:
		SDL_Texture* mTexture;

		int mWidth;
		int mHeight;
};

class Move
{
    public:
		static const int AD_HEIGHT = 100;

		static const int AD_VEL = 1;

		Move();
		void moveboss(SDL_Rect &box, int i);
		void renderboss();
		void move();
		void handleEvent(SDL_Event& e);
		void render();
		void setad();
		void setboss();
		int getPosX();
		int getPosY();
		SDL_Rect box;

    private:
		int mPosX, mPosY;
		int mVelX, mVelY;
		int boss_x, boss_y;
		SDL_Rect mCollider_ad;
		SDL_Rect mCollider_boss;
		
};

bool checkCollision(SDL_Rect a, SDL_Rect b);
bool init();

bool loadMedia();

void close();

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;

Mix_Chunk* collision = NULL;

LTexture gTimeTextTexture;
LTexture gFooTexture;
LTexture boss;
LTexture gBackgroundTexture;
LTexture hearts;
LTexture status;
LTexture again;


LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface;
	loadedSurface = IMG_Load(path.c_str());
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n","bird.bmp", IMG_GetError() );
	}
	else
	{
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0xFF, 0xFF, 0xFF) );

        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n","bird.bmp", SDL_GetError() );
		}
		else
		{
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		SDL_FreeSurface( loadedSurface );
	}

	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture:: setColor (Uint8 red, Uint8 green , Uint8 blue)
{
	SDL_SetTextureColorMod( mTexture, red, green, blue);
}

void LTexture:: setBlendMode (SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode (mTexture, blending);
}

void LTexture:: setAlpha (Uint8 alpha)
{
	SDL_SetTextureAlphaMod( mTexture, alpha);
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	if (clip!= NULL) {
		renderQuad.w = clip -> w;
		renderQuad.h = clip -> h;
	}
	SDL_RenderCopyEx ( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Move :: Move() 
{
	mPosX = 100;
	mPosY = 60;

	mVelX = 0;
	mVelY = 0;

	mCollider_ad.w = 100;
	mCollider_ad.h = 70;

	mCollider_boss.w = 100;
	mCollider_boss.h = 98;

	boss_x = 0;
	boss_y = rand() % 560 + 20;
}

void Move::handleEvent(SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= AD_VEL; break;
		case SDLK_DOWN: mVelY += AD_VEL; break;
		case SDLK_LEFT: mVelX -= AD_VEL; break;
		case SDLK_RIGHT: mVelX += AD_VEL; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += AD_VEL; break;
		case SDLK_DOWN: mVelY -= AD_VEL; break;
		case SDLK_LEFT: mVelX += AD_VEL; break;
		case SDLK_RIGHT: mVelX -= AD_VEL; break;
		}
	}
}

void Move::move()
{

	mPosX += mVelX;
	mCollider_ad.x = mPosX;

	if ((mPosX < 0) || (mPosX + 100 > SCREEN_WIDTH))
	{
		mPosX -= mVelX;
		mCollider_ad.x = mPosX;
	}

	mPosY += mVelY;
	mCollider_ad.y = mPosY;

	if ((mPosY < 0) || (mPosY + 70 > SCREEN_HEIGHT))
	{
		mPosY -= mVelY;
		mCollider_ad.y = mPosY;
	}

}

void Move::render ()
{
	gFooTexture.render( mPosX, mPosY );
}
int Move::getPosX() {
	return mCollider_ad.x;
}
int Move::getPosY() {
	return mCollider_ad.y;
}

void Move :: moveboss (SDL_Rect &box, int i)
{
	boss_x--;
	mCollider_boss.x = boss_x;
	mCollider_boss.y = boss_y;
	if (checkCollision(box, mCollider_boss)) {
		boss_x = 0;
		boss_y = 0;
		mCollider_boss.x = boss_x;
		mCollider_boss.y = boss_y;

		Mix_PlayChannel(-1,collision, 0);

		life--;
	}
	
	if (boss_x <= 0 ) {
		boss_x = 1200+i*300;
		mCollider_boss.x = boss_x;
		boss_y = rand() % 560 + 20;
		mCollider_boss.y = boss_y;
		score_++;
	}
}

void Move::setad() {
	mPosX = 100;
	mPosY = 60;
}
void Move::setboss() {
	boss_x = 0;
	boss_y = rand() % 560 + 20;
}
void Move :: renderboss ()
{
	boss.render(boss_x, boss_y);
}

bool init()
{
	bool success = true;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		gWindow = SDL_CreateWindow( "GAME", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	bool success = true;
	if( !gFooTexture.loadFromFile( "spaceship01.png" ) )
	{
		printf( "Failed to load AD texture image!\n" );
		success = false;
	}

	if( !gBackgroundTexture.loadFromFile( "bg.png" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}
	if ( !boss.loadFromFile("skeleton-fly_03.png") )
	{
		printf("Failed to load boss texture image!\n");
		success = false;
	}
	if (!hearts.loadFromFile("hearts.png"))
	{
		printf("Failed to load hearts texture image!\n");
		success = false;
	}
	if (!status.loadFromFile("set.png"))
	{
		printf("Failed to load status texture image!\n");
		success = false;
	}
	if (!again.loadFromFile("continue.png"))
	{
		printf("Failed to load status texture image!\n");
		success = false;
	}

	collision = Mix_LoadWAV("scratch.wav");
	if (collision == NULL)
	{
		printf("Failed to load collision sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	gFont = TTF_OpenFont("lazy.ttf", 28);
	SDL_Color textColor = { 0, 0, 0, 255 };

	return success;
}

void close()
{
	gFooTexture.free();
	gBackgroundTexture.free();
	boss.free();
	gTimeTextTexture.free();

	TTF_CloseFont(gFont);
	gFont = NULL;

	Mix_FreeChunk(collision);
	collision = NULL;

	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
	Mix_Quit();
}
bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}


int main( int argc, char* args[] )
{
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			bool quit = false;

			SDL_Event e;
			Move ad;
			Move boss[5];
			int scrollingOffset = 0;
			SDL_Color textColor = { 0, 0, 0, 255 };
			
			Uint32 startTime = 0;

			std::stringstream timeText;
			while( !quit )
			{
				while( SDL_PollEvent( &e ) != 0 )
				{
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					ad.handleEvent(e);
				}
				ad.move();
				SDL_Rect box;
				box.x = ad.getPosX();
				box.y = ad.getPosY();
				box.h = 40;
				box.w = 100;

				timeText.str("");
				//timeText << "SCORE: " << (SDL_GetTicks() - startTime) / 1000;
				timeText << "SCORE: " << score_;

				if (!gTimeTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor))
				{
					printf("Unable to render time texture!\n");
				}

				--scrollingOffset;
				if( scrollingOffset < -gBackgroundTexture.getWidth() )
				{
					scrollingOffset = 0;
				}

				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear( gRenderer );

				gBackgroundTexture.render( scrollingOffset, 0 );
				gBackgroundTexture.render( scrollingOffset + gBackgroundTexture.getWidth(), 0 );

				gTimeTextTexture.render(550, 5);
				ad.render();
				for (int i = 0; i < 5; i++) {
					boss[i].moveboss(box, i);
					boss[i].renderboss();

				}
				for (int i = 1; i <=life; i++) {
					hearts.render(30 *i, 20);
				}
				if (life <= 0) {
					status.render(200, 200);
					again.render(290, 400);
					SDL_Delay(100);
					if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
					{
							life = 5;
							for (int i = 0; i < 5; i++) {
								boss[i].setboss();
								ad.setad();
							}
							score_ = -5;
					}
				}
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	close();

	return 0;
}