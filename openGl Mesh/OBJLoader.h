#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <glm.hpp>

namespace objl
{
	// Structure: Vertex
	//
	// Description: Model Vertex object that holds
	//	a Position, Normal, and Texture Coordinate
	struct Vertex
	{
		// Position Vector
		glm::vec3 Position;

		// Normal Vector
		glm::vec3 Normal;

		// Texture Coordinate Vector
		glm::vec2 TextureCoordinate;
	};

	struct Material
	{
		Material();

		// Material Name
		std::string name;
		// Ambient Color
		glm::vec3 Ka;
		// Diffuse Color
		glm::vec3 Kd;
		// Specular Color
		glm::vec3 Ks;
		// Specular Exponent
		float Ns;
		// Optical Density
		float Ni;
		// Dissolve
		float d;
		// Illumination
		int illum;
		// Ambient Texture Map
		std::string map_Ka;
		// Diffuse Texture Map
		std::string map_Kd;
		// Specular Texture Map
		std::string map_Ks;
		// Specular Hightlight Map
		std::string map_Ns;
		// Alpha Texture Map
		std::string map_d;
		// Bump Map
		std::string map_bump;
	};

	// Structure: Mesh
	//
	// Description: A Simple Mesh Object that holds
	//	a name, a vertex list, and an index list
	struct Mesh
	{
		// Default Constructor
		Mesh();
		// Variable Set Constructor
		Mesh(std::vector<Vertex>& _Vertices, std::vector<unsigned int>& _Indices);
		// Mesh Name
		std::string MeshName;
		// Vertex List
		std::vector<Vertex> Vertices;
		// Index List
		std::vector<unsigned int> Indices;

		// Material
		Material MeshMaterial;
	};

	// Namespace: Algorithm
	//
	// Description: The namespace that holds all of the
	// Algorithms needed for OBJL
	namespace algorithm
	{
		// Angle between 2 Vector3 Objects
		float AngleBetweenV3(const glm::vec3 a, const glm::vec3 b);
		// Projection Calculation of a onto b
		glm::vec3 ProjV3(const glm::vec3 a, const glm::vec3 b);
		// A test to see if P1 is on the same side as P2 of a line segment ab
		bool SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b);

		// Generate a cross produect normal for a triangle
		glm::vec3 GenTriNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3);

		// Check to see if a glm::vec3 Point is within a 3 glm::vec3 Triangle
		bool inTriangle(glm::vec3 point, glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3);

		// Split a String into a string array at a given token
		void split(const std::string& in,
			std::vector<std::string>& out,
			std::string token);

		// Get tail of string after first token and possibly following spaces
		std::string tail(const std::string& in);

		// Get first token of string
		std::string firstToken(const std::string& in);

		// Get element at given index position
		template <class T>
		const T& getElement(const std::vector<T>& elements, std::string& index);
	}

	// Class: Loader
	//
	// Description: The OBJ Model Loader
	class Loader
	{
	public:
		// Default Constructor
		Loader();
		~Loader();

		// Load a file into the loader
		//
		// If file is loaded return true
		//
		// If the file is unable to be found
		// or unable to be loaded return false
		bool LoadFile(std::string Path);

		// Loaded Mesh Objects
		std::vector<Mesh> LoadedMeshes;
		// Loaded Vertex Objects
		std::vector<Vertex> LoadedVertices;
		// Loaded Index Positions
		std::vector<unsigned int> LoadedIndices;
		// Loaded Material Objects
		std::vector<Material> LoadedMaterials;

	private:
		// Generate vertices from a list of positions, 
		//	tcoords, normals and a face line
		void GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
			const std::vector<glm::vec3>& iPositions,
			const std::vector<glm::vec2>& iTCoords,
			const std::vector<glm::vec3>& iNormals,
			std::string icurline);

		// Triangulate a list of vertices into a face by printing
		//	inducies corresponding with triangles within it
		void VertexTriangluation(std::vector<unsigned int>& oIndices,
			const std::vector<Vertex>& iVerts);

		// Load Materials from .mtl file
		bool LoadMaterials(std::string path);
	};
}