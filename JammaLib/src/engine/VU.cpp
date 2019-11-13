#include "VU.h"

using namespace engine;
using base::DrawContext;
using base::Drawable;
using base::DrawableParams;
using graphics::GlDrawContext;
using gui::GuiModel;
using utils::Size2d;

const Size2d VU::_LedGap = { 2, 2 };

VU::VU(VuParams params) :
	GuiModel(params),
	_value(audio::FallingValue({ 0.02f })),
	_vuParams(params)
{
}

VU::~VU()
{
}

void VU::Draw3d(DrawContext& ctx)
{
	auto& glCtx = dynamic_cast<GlDrawContext&>(ctx);

	auto val = _value.Next();
	glCtx.SetUniform("Value", val);

	glCtx.PushMvp(glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f + 2.0f * val, 1.0f)));

	GuiModel::Draw3d(glCtx);

	glCtx.PopMvp();
}

double VU::Value() const
{
	return _value.Current();
}

void VU::SetValue(double value)
{
	_value.SetTarget(value);
}

void VU::UpdateModel(float radius)
{
	std::vector<float> verts;
	std::vector<float> uvs;

	auto numLeds = NumLeds(_sizeParams.Size.Height,
		_vuParams.LedHeight);

	for (auto led = 0u; led < numLeds; led++)
	{
		auto [ledVerts, ledUvs] =
			CalcLedGeometry(led,
				radius,
				_sizeParams.Size.Height,
				_vuParams.LedHeight);

		for (auto vert : ledVerts)
			verts.push_back(vert);

		for (auto uv : ledUvs)
			uvs.push_back(uv);
	}

	SetGeometry(verts, uvs);
}

unsigned int VU::NumLeds(unsigned int vuHeight,
	double ledHeight)
{
	return (unsigned int)std::ceil(((double)vuHeight) / ledHeight);
}

std::tuple<std::vector<float>, std::vector<float>>
VU::CalcLedGeometry(unsigned int led,
	float radius,
	unsigned int height,
	double ledHeight)
{
	const float radialThickness = radius / 15.0f;

	auto yToUv = [&height](float y) {
		if (height > 0)
			return y / (float)height;

		return 0.0f;
	};

	auto angle1 = -0.01f;
	auto angle2 = -angle1;
	auto xInner1 = sin(angle1) * (radius - radialThickness);
	auto xInner2 = sin(angle2) * (radius - radialThickness);
	auto xOuter1 = sin(angle1) * (radius + radialThickness);
	auto xOuter2 = sin(angle2) * (radius + radialThickness);
	auto yMin = (float)((led * ledHeight) + (_LedGap.Height / 2.0));
	auto yMax = (float)(((led + 1) * ledHeight) - (_LedGap.Height / 2.0));
	auto zInner1 = cos(angle1) * (radius - radialThickness);
	auto zInner2 = cos(angle2) * (radius - radialThickness);
	auto zOuter1 = cos(angle1) * (radius + radialThickness);
	auto zOuter2 = cos(angle2) * (radius + radialThickness);

	std::vector<float> verts;
	std::vector<float> uvs;

	// Front
	verts.push_back(xOuter1); verts.push_back(yMin); verts.push_back(zOuter1);
	verts.push_back(xOuter2); verts.push_back(yMax); verts.push_back(zOuter2);
	verts.push_back(xOuter1); verts.push_back(yMax); verts.push_back(zOuter1);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));

	verts.push_back(xOuter1); verts.push_back(yMin); verts.push_back(zOuter1);
	verts.push_back(xOuter2); verts.push_back(yMin); verts.push_back(zOuter2);
	verts.push_back(xOuter2); verts.push_back(yMax); verts.push_back(zOuter2);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));

	// Top
	verts.push_back(xOuter1); verts.push_back(yMax); verts.push_back(zOuter1);
	verts.push_back(xInner2); verts.push_back(yMax); verts.push_back(zInner2);
	verts.push_back(xInner1); verts.push_back(yMax); verts.push_back(zInner1);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));

	verts.push_back(xOuter1); verts.push_back(yMax); verts.push_back(zOuter1);
	verts.push_back(xOuter2); verts.push_back(yMax); verts.push_back(zOuter2);
	verts.push_back(xInner2); verts.push_back(yMax); verts.push_back(zInner2);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));

	// Back
	verts.push_back(xInner1); verts.push_back(yMin); verts.push_back(zInner1);
	verts.push_back(xInner1); verts.push_back(yMax); verts.push_back(zInner1);
	verts.push_back(xInner2); verts.push_back(yMax); verts.push_back(zInner2);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));

	verts.push_back(xInner1); verts.push_back(yMin); verts.push_back(zInner1);
	verts.push_back(xInner2); verts.push_back(yMax); verts.push_back(zInner2);
	verts.push_back(xInner2); verts.push_back(yMin); verts.push_back(zInner2);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));

	// Bottom
	verts.push_back(xOuter1); verts.push_back(yMin); verts.push_back(zOuter1);
	verts.push_back(xInner1); verts.push_back(yMin); verts.push_back(zInner1);
	verts.push_back(xInner2); verts.push_back(yMin); verts.push_back(zInner2);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));

	verts.push_back(xOuter1); verts.push_back(yMin); verts.push_back(zOuter1);
	verts.push_back(xInner2); verts.push_back(yMin); verts.push_back(zInner2);
	verts.push_back(xOuter2); verts.push_back(yMin); verts.push_back(zOuter2);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));

	// Left Side
	verts.push_back(xOuter1); verts.push_back(yMin); verts.push_back(zInner1);
	verts.push_back(xOuter1); verts.push_back(yMax); verts.push_back(zOuter1);
	verts.push_back(xOuter1); verts.push_back(yMax); verts.push_back(zInner1);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));

	verts.push_back(xOuter1); verts.push_back(yMin); verts.push_back(zInner1);
	verts.push_back(xOuter1); verts.push_back(yMin); verts.push_back(zOuter1);
	verts.push_back(xOuter1); verts.push_back(yMax); verts.push_back(zOuter1);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));

	// Right Side
	verts.push_back(xOuter2); verts.push_back(yMin); verts.push_back(zInner2);
	verts.push_back(xOuter2); verts.push_back(yMax); verts.push_back(zInner2);
	verts.push_back(xOuter2); verts.push_back(yMax); verts.push_back(zOuter2);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));

	verts.push_back(xOuter2); verts.push_back(yMin); verts.push_back(zInner2);
	verts.push_back(xOuter2); verts.push_back(yMax); verts.push_back(zOuter2);
	verts.push_back(xOuter2); verts.push_back(yMin); verts.push_back(zOuter2);
	uvs.push_back(angle1 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)constants::TWOPI); uvs.push_back(yToUv(yMax));

	return std::make_tuple(verts, uvs);
}
