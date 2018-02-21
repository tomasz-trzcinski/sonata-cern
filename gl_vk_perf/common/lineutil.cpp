#include "lineutil.hpp"

//http://scaledinnovation.com/analytics/splines/aboutSplines.html
std::tuple<glm::vec3, glm::vec3> LineUtil::calcGeomControlPoints(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t)
{
	float d01 = glm::distance(p0, p1);
	float d12 = glm::distance(p1, p2);

	float fa = t * d01 / (d01 + d12);
	float fb = t * d12 / (d01 + d12);

	glm::vec3 c1 = p1 + fa*(p0 - p2);
	glm::vec3 c2 = p1 - fb*(p0 - p2);

	return std::make_tuple(c1, c2);
}

std::tuple<glm::vec3, glm::vec3> LineUtil::calcHobbyControlPoints(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	glm::vec3 W1 = glm::normalize(p2 - p0);
	glm::vec3 W2 = glm::normalize(p3 - p1);

	glm::vec3 delta = p2 - p1;

	float atan_delta = glm::atan(delta.y, delta.x);

	float theta = glm::atan(W1.y, W1.x) - atan_delta;

	float phi = atan_delta - glm::atan(W2.y, W2.x);

	float a = 1.597f, b = 0.07f, c = 0.37f;

	float st = glm::sin(theta);
	float ct = glm::cos(theta);
	float sp = glm::sin(phi);
	float cp = glm::cos(phi);

	float d = glm::distance(p2, p1);

	float alpha = a*(st - b*sp)*(sp - b*st)*(ct - cp);
	float rho = (2 + alpha) / (1 + (1 - c)*ct + c*cp);
	float sigma = (2 - alpha) / (1 + (1 - c)*cp + c*ct);

	float tau = 1;

	glm::vec3 c1 = p1 + (rho / (3 * tau)) * d * W1;
	glm::vec3 c2 = p2 - (sigma / (3 * tau)) * d * W2;

	return std::make_tuple(c1, c2);
}

void LineUtil::fillBuffers(Render::CalcType cType, Render::LineType lType, const std::vector<glm::vec3> &pointList, std::vector<glm::vec3> &vertexData, std::vector<glm::u16> &indexData)
{
	if (cType == Render::CalcType::CPU)
	{
		size_t numLines = pointList.size() - 1;
		glm::u16 index = 0;

		glm::vec3 c1, c2, dummy;
		for (glm::u16 i = 0; i < numLines; ++i)
		{
			glm::vec3 p0, p1, p2, p3;
			//first line is p(0) -> p(1), but p(-1) needed doesn't exist! Special case with duplicated first vertex
			if (i == 0)
			{
				p0 = pointList[i];
				p1 = pointList[i];
				p2 = pointList[i + 1];
				p3 = pointList[i + 2];
			}
			//last line is p(end-1) -> p(end), but p(end+1) needed doesn't exist! Special case with duplicated last vertex
			else if (i == numLines - 1)
			{
				p0 = pointList[i - 1];
				p1 = pointList[i];
				p2 = pointList[i + 1];
				p3 = pointList[i + 1];
			}
			else
			{
				p0 = pointList[i - 1];
				p1 = pointList[i];
				p2 = pointList[i + 1];
				p3 = pointList[i + 2];
			}

			if (lType == Render::LineType::GEOM)
			{
				std::tie(dummy, c1) = LineUtil::calcGeomControlPoints(p0, p1, p2);
				std::tie(c2, dummy) = LineUtil::calcGeomControlPoints(p1, p2, p3);
			}
			else
			{
				std::tie(c1, c2) = LineUtil::calcHobbyControlPoints(p0, p1, p2, p3);
			}

			vertexData.push_back(p1);
			vertexData.push_back(c1);
			vertexData.push_back(c2);
			if (i == (numLines - 1))
				vertexData.push_back(p2);

			indexData.push_back(index);
			indexData.push_back(index + 1);
			indexData.push_back(index + 2);
			indexData.push_back(index + 3);
			index += 3;
		}
	}
	else
	{
		vertexData.insert(vertexData.end(), pointList.begin(), pointList.end());
		size_t numLines = pointList.size() - 1;

		for (glm::u16 i = 0; i < numLines; ++i)
		{
			//first line is p(0) -> p(1), but p(-1) needed doesn't exist! Special case with duplicated first vertex
			if (i == 0)
			{
				indexData.push_back(i);
				indexData.push_back(i);
				indexData.push_back(i + 1);
				indexData.push_back(i + 2);
			}
			//last line is p(end-1) -> p(end), but p(end+1) needed doesn't exist! Special case with duplicated last vertex
			else if (i == numLines - 1)
			{
				indexData.push_back(i - 1);
				indexData.push_back(i);
				indexData.push_back(i + 1);
				indexData.push_back(i + 1);
			}
			else
			{
				indexData.push_back(i - 1);
				indexData.push_back(i);
				indexData.push_back(i + 1);
				indexData.push_back(i + 2);
			}
		}
	}
}
