#ifndef __H_LINEUTIL__
#define __H_LINEUTIL__

#include <glm/glm.hpp>
#include <vector>
#include <tuple>

#include "enums.hpp"

class LineUtil
{
	public:
		static std::tuple<glm::vec3, glm::vec3> calcGeomControlPoints(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t = 0.25f);
		static std::tuple<glm::vec3, glm::vec3> calcHobbyControlPoints(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

		static void fillBuffers(Render::CalcType cType, Render::LineType lType, const std::vector<glm::vec3> &pointList, std::vector<glm::vec3> &vertexData, std::vector<glm::u16> &indexData);
};

#endif