#pragma once
#include "ResourceManager.h"


class TextureManager : public ResourceManager<TextureManager, HBITMAP> {
public:
	TextureManager() : ResourceManager("media/texture_config.txt") {}

	HBITMAP Load(const std::string l_path) {
		HBITMAP hTexture;
		
		if (!(hTexture = (HBITMAP)LoadImageA(NULL, (Utils::GetWorkingDirectory() + l_path).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)))
		{
			std::cerr << "! Failed to load texture: " << l_path << std::endl;
		}
		return hTexture;
	}
};