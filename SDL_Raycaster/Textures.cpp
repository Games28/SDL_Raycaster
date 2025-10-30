#include "Textures.h"

void Textures::LoadTextures()
{
	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		upng_t* upng = upng_new_from_file(textureFileNames[i]);
		if (upng != NULL)
		{
			upng_decode(upng);
			if (upng_get_error(upng) == UPNG_EOK)
			{
				textures[i] = upng;
			}
			else
			{
				printf("error decoding texture files %s \n", textureFileNames[i]);
			}
		}
	}


}

void Textures::freeTextures()
{
	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		upng_free(textures[i]);
	}
}
