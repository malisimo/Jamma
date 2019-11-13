#pragma once

#include <vector>
#include <memory>
#include "../audio/FallingValue.h"
#include "../gui/GuiModel.h"

namespace engine
{
	class VuParams :
		public gui::GuiModelParams
	{
	public:
		VuParams() :
			gui::GuiModelParams(),
			LedHeight(10.0)
		{
		}

		VuParams(gui::GuiModelParams params) :
			gui::GuiModelParams(params),
			LedHeight(10.0)
		{
		}

	public:
		double LedHeight;
	};

	class VU :
		public virtual gui::GuiModel
	{
	public:
		VU(VuParams params);
		~VU();

		// Copy
		VU(const VU&) = delete;
		VU& operator=(const VU&) = delete;

	public:
		void Draw3d(base::DrawContext& ctx) override;

		double Value() const;
		void SetValue(double value);
		void UpdateModel(float radius);

	protected:
		static unsigned int NumLeds(unsigned int vuHeight, double ledHeight);
		static std::tuple<std::vector<float>, std::vector<float>>
			CalcLedGeometry(unsigned int led,
				float radius,
				unsigned int height,
				double ledHeight);

	protected:
		static const utils::Size2d _LedGap;

		audio::FallingValue _value;
		VuParams _vuParams;
	};
}
