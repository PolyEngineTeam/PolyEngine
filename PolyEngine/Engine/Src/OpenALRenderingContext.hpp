#pragma once

#include <BaseObject.hpp>

#include <al.h> 
#include <alc.h>

namespace Poly {

	class OpenALRenderingContext : public BaseObject<>  
	{
	public:
		bool Init();
		void Deinit();

		void EndFrame();

	private:
		ALCdevice* Device;
		ALCcontext* Context;
	};

} //namespace Poly