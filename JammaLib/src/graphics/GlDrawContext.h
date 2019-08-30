#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <any>
#include <glm/glm.hpp>
#include <vector>
#include "../utils/CommonTypes.h"
#include "DrawContext.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace graphics
{
	class GlDrawContext :
		public base::DrawContext
	{
	public:
		GlDrawContext();
		~GlDrawContext();

	public:
		auto GetContextType() -> base::DrawContext::ContextType
		{
			return ContextType::OPENGL;
		}

		std::optional<std::any> GetUniform(std::string name);
		void SetUniform(const std::string& name, std::any val);

		void SetProjection(const glm::mat4 mat) noexcept;
		void PushModelView(const glm::mat4 mat) noexcept;
		void PopModelView() noexcept;
		void ClearModelView() noexcept;
		utils::Position2d ProjectScreen(utils::Position3d pos);

	private:
		const std::string _MvpUniformName = "MVP";

		std::map<std::string, std::any> _uniforms;
		glm::mat4 _projection;
		std::vector<glm::mat4> _modelView;
	};
}
