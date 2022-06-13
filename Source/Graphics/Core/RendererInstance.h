#pragma once
#include <mutex>

#include "Core/Renderer.h"

class RendererInstance
{

public:
    /**
     * Singletons should not be cloneable.
     */
    RendererInstance(RendererInstance& other) = delete;
    /**
     * Singletons should not be assignable.
     */
    void operator=(const RendererInstance&) = delete;
    /**
     * This is the static method that controls the access to the singleton
     * instance. On the first run, it creates a singleton object and places it
     * into the static field. On subsequent runs, it returns the client existing
     * object stored in the static field.
     */

    static Renderer* GetInstance();

protected:
    RendererInstance();
    ~RendererInstance() = default;
	
private:
    static Renderer* mInstance;
    static std::mutex mMutex;
};