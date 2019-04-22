#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <any>
#include <glm/glm.hpp>
#include <vector>
#include "DrawContext.h"

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

		void PushMvp(const glm::mat4 mat) noexcept;
		void PopMvp() noexcept;
		void ClearMvp() noexcept;

	private:
		const std::string _MvpUniformName = "MVP";

		std::map<std::string, std::any> _uniforms;
		std::vector<glm::mat4> _mvp;
	};
}
