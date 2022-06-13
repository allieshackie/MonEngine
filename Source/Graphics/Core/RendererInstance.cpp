#include "RendererInstance.h"
Renderer* RendererInstance::mInstance{ nullptr };
std::mutex RendererInstance::mMutex;

RendererInstance::RendererInstance()
{
	mInstance = new Renderer();
}

/**
 * The first time we call GetInstance we will lock the storage location
 * and then we make sure again that the variable is null and then we
 * set the value. RU:
 */
Renderer* RendererInstance::GetInstance()
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (mInstance == nullptr)
    {
        mInstance = new Renderer();
    }
    return mInstance;
}