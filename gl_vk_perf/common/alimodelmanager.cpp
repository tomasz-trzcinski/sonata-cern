#include "alimodelmanager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <iostream>

glm::vec3 aiColorToVec3(aiColor4D &aiColor)
{
	return glm::vec3(aiColor.r, aiColor.g, aiColor.b);
}

AliModelManager::AliModelManager(std::string modelPath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath, 0);

	if (!scene)
	{
		std::runtime_error("Can't load model");
	}

	for (size_t m = 0; m < scene->mNumMeshes; ++m)
	{
		const aiMesh* mesh = scene->mMeshes[m];

		Mesh aliMesh;
		aliMesh.m_name = mesh->mName.C_Str();

		aliMesh.m_vertices.resize(mesh->mNumVertices);
		memcpy(&aliMesh.m_vertices[0], mesh->mVertices, mesh->mNumVertices * sizeof(aiVector3D));

		aliMesh.m_faces.resize(mesh->mNumFaces);

		for (unsigned int t = 0; t < mesh->mNumFaces; ++t)
		{
			const aiFace* face = &mesh->mFaces[t];
			memcpy(&aliMesh.m_faces[t], face->mIndices, face->mNumIndices * sizeof(unsigned int));
		}

		aiMaterial *mtl = scene->mMaterials[mesh->mMaterialIndex];

		aiColor4D diffuse;

		if(aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS)
		{
			aiColorToVec3(diffuse);
		}

		m_meshes.push_back(aliMesh);
	}
}

AliModelManager::~AliModelManager()
{

}
