#pragma once

#include "GuiElement.h"
#include "GlUtils.h"

namespace gui
{
	class GuiModelParams : public base::GuiElementParams
	{
	public:
		GuiModelParams() :
			base::GuiElementParams(DrawableParams{ "" },
				MoveableParams(utils::Position2d{ 0, 0 }, utils::Position3d{ 0, 0, 0 }, 1.0),
				SizeableParams{ 1,1 },
				"",
				"",
				"",
				{}),
			ModelTexture(""),
			ModelShader("")
		{
		}

		GuiModelParams(base::GuiElementParams params) :
			base::GuiElementParams(params),
			ModelTexture(""),
			ModelShader("")
		{
		}

	public:
		std::string ModelTexture;
		std::string ModelShader;
	};

	class GuiModel :
		public base::GuiElement
	{
	public:
		GuiModel(GuiModelParams params);

	public:
		void Draw3d(base::DrawContext& ctx) override;

	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool _ReleaseResources() override;

		bool InitTexture(resources::ResourceLib& resourceLib);
		bool InitShader(resources::ResourceLib& resourceLib);
		bool InitVertexArray();

	protected:
		const int VertexCount = 6;

		GuiModelParams _modelParams;
		GLuint _vertexArray;
		GLuint _vertexBuffer[2];
		std::weak_ptr<resources::TextureResource> _modelTexture;
		std::weak_ptr<resources::ShaderResource> _modelShader;
	};
}
