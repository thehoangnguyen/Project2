#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

using namespace std;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 672;


class LTexture
{
	public:
		LTexture();

		~LTexture();

		bool loadFromFile( std::string path );

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
		void moveboss();
		void renderboss();
		void move();
		void handleEvent(SDL_Event& e);
		void render();
		

    private:
		int mPosX, mPosY;
		int mVelX, mVelY;
		int boss_x, boss_y;
		SDL_Rect mCollider_ad;
		SDL_Rect mCollider_boss;
		
};

bool checkCollision(SDL_Rect a, SDL_Rect b);
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

LTexture gFooTexture;
LTexture boss;
LTexture gBackgroundTexture;


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
	mPosY = 20;

	mVelX = 0;
	mVelY = 0;

	mCollider_ad.w = 100;
	mCollider_ad.h = 70;

	mCollider_boss.w = 100;
	mCollider_boss.h = 98;

	boss_x = 500;
	boss_y = 220;
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

	if ((mPosX < 0) || (mPosX + 100 > SCREEN_WIDTH)|| checkCollision(mCollider_ad, mCollider_boss))
	{
		mPosX -= mVelX;
		mCollider_ad.x = mPosX;
	}

	mPosY += mVelY;
	mCollider_ad.y = mPosY;

	if ((mPosY < 0) || (mPosY + 70 > SCREEN_HEIGHT)|| checkCollision(mCollider_ad, mCollider_boss))
	{
		mPosY -= mVelY;
		mCollider_ad.y = mPosY;
	}
}

void Move::render ()
{
	gFooTexture.render( mPosX, mPosY );
}


void Move :: moveboss ()
{
	//boss_x--;
	mCollider_boss.x = boss_x;
	mCollider_boss.y = boss_y;
	//if (boss_x <= 0 ) {
		//boss_x = 1200;
		//mCollider_boss.x = boss_x;
		//boss_y = rand() % 580 + 20;
		//mCollider_boss.y = boss_y;
	//}
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
		printf( "Failed to load Foo' texture image!\n" );
		success = false;
	}

	if( !gBackgroundTexture.loadFromFile( "bg.png" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}
	if ( !boss.loadFromFile("skeleton-fly_03.png") )
	{
		printf("Failed to load background texture image!\n");
		success = false;
	}

	return success;
}

void close()
{
	gFooTexture.free();
	gBackgroundTexture.free();
	boss.free();

	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
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
			Move boss;
			int scrollingOffset = 0;
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
				//ad.move();
				boss.moveboss();
				ad.move();
				--scrollingOffset;
				if( scrollingOffset < -gBackgroundTexture.getWidth() )
				{
					scrollingOffset = 0;
				}
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear( gRenderer );

				gBackgroundTexture.render( scrollingOffset, 0 );
				gBackgroundTexture.render( scrollingOffset + gBackgroundTexture.getWidth(), 0 );							
				
				ad.render();
				boss.renderboss();
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	close();

	return 0;
}