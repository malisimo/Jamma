#pragma once

#include <string>
#include <memory>
#include "Audible.h"
#include "ActionReceiver.h"
#include "ResourceUser.h"
#include "../audio/AudioBuffer.h"
//#include "../audio/AudioMixer.h"
#include "../resources/WavResource.h"

namespace engine
{
	class LoopParams
	{
	public:
		std::string Wav;
	};

	class Loop :
		public base::Audible, public base::ResourceUser, public base::ActionReceiver
	{
	public:
		Loop(LoopParams loopParams);
		Loop();
		~Loop();

	public:

		virtual bool InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool ReleaseResources() override;
		virtual void Play(std::shared_ptr<audio::AudioBuffer> buf, unsigned int numSamps) override;

	private:
		unsigned int _index;
		LoopParams _loopParams;
		std::weak_ptr<resources::WavResource> _wav;
	};
}
