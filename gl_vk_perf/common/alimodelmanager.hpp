#ifndef __H_ALIMODELMGR__
#define __H_ALIMODELMGR__

#include <string>
#include <vector>

#include <glm/vec3.hpp>

class AliModelManager
{
	public:
		struct Mesh
		{
			std::string m_name;

			std::vector<glm::uvec3> m_faces;
			std::vector<glm::vec3> m_vertices;
		};

		std::vector<Mesh> m_meshes;

	public:
		AliModelManager(std::string);
		~AliModelManager();
};
#endif
