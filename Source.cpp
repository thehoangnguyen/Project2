#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

using namespace std;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 672;
const int MOVING_BOSS = 4;
SDL_Rect movingboss [MOVING_BOSS];


class LTexture
{
	public:
		LTexture();

		~LTexture();

		bool loadFromFile( std::string path );

		void free();

		void render( int x, int y, SDL_Rect* clip = NULL );
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

void LTexture::render( int x, int y, SDL_Rect* clip )
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	if (clip!= NULL) {
		renderQuad.w = clip -> w;
		renderQuad.h = clip -> h;
	}
	SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
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

	if( !gBackgroundTexture.loadFromFile( "flappy-bird.bmp" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}
	//boss.loadFromFile("Untitled.png");
	if ( !boss.loadFromFile("Untitled.png") )
	{

	}
	else{
		movingboss[ 0 ].x =   0;
		movingboss[ 0 ].y =   0;
		movingboss[ 0 ].w =  211;
		movingboss[ 0 ].h = 255;

		movingboss[ 1 ].x =  207;
		movingboss[ 1 ].y =   0;
		movingboss[ 1 ].w =  211;
		movingboss[ 1 ].h = 255;
		
		movingboss[ 2 ].x = 435;
		movingboss[ 2 ].y =   0;
		movingboss[ 2 ].w =  211;
		movingboss[ 2 ].h = 255;

		movingboss[ 3 ].x = 655;
		movingboss[ 3 ].y =   0;
		movingboss[ 3 ].w =  211;
		movingboss[ 3 ].h = 255;
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
			int frame =0;

			while( !quit )
			{
				while( SDL_PollEvent( &e ) != 0 )
				{
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
				}
				
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear( gRenderer );
				gBackgroundTexture.render( 0, 0 );
				SDL_Rect* currentClip = & movingboss[frame/4];
				boss.render( 800,150 , currentClip );

				gFooTexture.render( 100, 100 );
				SDL_RenderPresent( gRenderer );
				++frame;


				if (frame /4 >= MOVING_BOSS) frame=0;
			}
		}
	}

	close();

	return 0;
}