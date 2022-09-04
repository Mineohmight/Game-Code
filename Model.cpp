#include "Model.h"

Model::Model(const char* file, std::vector<float> scale, std::vector<float> translation, std::vector<float> color) {

	trimesh::TriMesh* m = trimesh::TriMesh::read(file);
	if (!m) {
		exit(69);
	}

	m->need_normals();
	m->need_faces();

	// add vertices and each vertice's respective normal
	for (int i = 0; i < m->vertices.size(); i++)
	{
		for (int j = 0; j < 11; j++)
		vertices.emplace_back();

			//position
		vertices.at(i * 11 + 0) = m->vertices.at(i).at(0) * scale.at(0) + translation.at(0);
		vertices.at(i * 11 + 1) = m->vertices.at(i).at(1) * scale.at(1) + translation.at(1);
		vertices.at(i * 11 + 2) = m->vertices.at(i).at(2) * scale.at(2) + translation.at(2);
		
			//colors 
		vertices.at(i * 11 + 3) = color.at(0);
		vertices.at(i * 11 + 4) = color.at(1);
		vertices.at(i * 11 + 5) = color.at(2);

		//set all the texture coordinates for each vertex
		vertices.at(i * 11 + 6) = m->vertices.at(i).at(0);
		vertices.at(i * 11 + 7) = m->vertices.at(i).at(1);

		//normals (using vertex normals) (I am pretty sure these vertex normals are converted into face normals in the graphics engine though)
		vertices.at(i * 11 + 8) = m->normals.at(i).at(0) * scale.at(0) + translation.at(0);
		vertices.at(i * 11 + 9) = m->normals.at(i).at(1) * scale.at(1) + translation.at(1);
		vertices.at(i * 11 + 10) = m->normals.at(i).at(2) * scale.at(2) + translation.at(2);
		//vertices.at(i * 11 + 8) = m->trinorm(i).at(0);
		//vertices.at(i * 11 + 9) = m->trinorm(i).at(1);
		//vertices.at(i * 11 + 10) = m->trinorm(i).at(2);
			
	}

	// add indices that instruct how the triangles are to be drawn (order of the vertices)
	for (int i = 0; i < m->faces.size(); i++)
	{
		indices.emplace_back(m->faces.at(i).at(0));
		indices.emplace_back(m->faces.at(i).at(1));
		indices.emplace_back(m->faces.at(i).at(2));
	}
}