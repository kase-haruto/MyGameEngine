#include "SpriteAsset.h"
#include <Engine/Assets/Texture/TextureManager.h>

CalyxEngine::SpriteAsset::SpriteAsset(const std::string& filePath)
	: path_(filePath) {
	handle_ = TextureManager::GetInstance()->LoadTexture(filePath);
}