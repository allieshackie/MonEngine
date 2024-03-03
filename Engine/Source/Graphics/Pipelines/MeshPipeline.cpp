#include "LLGL/Utils/Utility.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <sstream>
#include <filesystem>

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"

#include "MeshPipeline.h"

void MeshPipeline::Render(LLGL::CommandBuffer& commands) const
{
	// Set resources
	if (model != nullptr)
	{
		commands.SetPipelineState(*mPipeline);
		commands.SetVertexBuffer(*mVertexBuffer);
		commands.SetResourceHeap(*mVolumeResourceHeap);

		commands.Draw(model->numVertices, model->firstVertex);
	}
}

void MeshPipeline::Init(LLGL::RenderSystemPtr& renderSystem, const std::string& shaderPath)
{
	mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(VolumeSettings)),
	                                             &volumeSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"normal", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});
	vertexFormat.SetStride(sizeof(TexturedVertex));

	std::string vertPath = shaderPath;
	vertPath.append("volume.vert");

	std::string fragPath = shaderPath;
	fragPath.append("volume.frag");

	mShader = std::make_unique<Shader>(*renderSystem, vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());

	// All layout bindings that will be used by graphics and compute pipelines
	LLGL::PipelineLayoutDescriptor layoutDesc;
	{
		layoutDesc.bindings =
		{
			LLGL::BindingDescriptor{
				LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer, LLGL::StageFlags::AllStages, 0
			}
		};
	}
	// Create pipeline layout
	const auto pipelineLayout = renderSystem->CreatePipelineLayout(layoutDesc);

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.pipelineLayout = pipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
	}
	mPipeline = renderSystem->CreatePipelineState(pipelineDesc);

	std::vector<LLGL::ResourceViewDescriptor> resourceViews = {mConstantBuffer};
	mVolumeResourceHeap = renderSystem->CreateResourceHeap(pipelineLayout, resourceViews);
}

void MeshPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, glm::mat4 model,
                                                    glm::mat4 projection,
                                                    glm::mat4 view)
{
	// TODO: Apply rotation in ZXY order
	volumeSettings.pvmMat = projection * view * model;
	commands.UpdateBuffer(*mConstantBuffer, 0, &volumeSettings, sizeof(VolumeSettings));
}

// TODO: Implement actual model class as RenderObject
void MeshPipeline::AddRenderObjectVBuffer(const LLGL::RenderSystemPtr& renderSystem, RenderObject& obj)
{
	std::vector<TexturedVertex> vertices;
	_LoadObjModel(vertices, "../Data/Models/Pyramid.obj");
	model = std::make_unique<Model>();
	model->numVertices = static_cast<std::uint32_t>(vertices.size());

	mVertexBuffer = renderSystem->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(vertices.size() * sizeof(Vertex)), mShader->GetVertexFormat()),
		vertices.data()
	);
}

TriangleMesh MeshPipeline::_LoadObjModel(std::vector<TexturedVertex>& vertices, const std::string& filename) const
{
	// Read obj file
	std::ifstream file(filename);
	if (!file.good())
		throw std::runtime_error("failed to load model from file: \"" + filename + "\"");

	// Initialize triangle mesh
	TriangleMesh mesh;
	mesh.firstVertex = static_cast<std::uint32_t>(vertices.size());

	std::vector<glm::vec3> coords, normals;
	std::vector<glm::vec2> texCoords;

	std::string line;
	// Read each line
	while (std::getline(file, line))
	{
		std::stringstream s;
		s << line;

		// Parse line
		std::string mode;
		s >> mode;

		if (mode == "v")
		{
			glm::vec3 v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			coords.push_back(v);
		}
		else if (mode == "vt")
		{
			glm::vec2 t;
			s >> t.x;
			s >> t.y;
			texCoords.push_back(t);
		}
		else if (mode == "vn")
		{
			glm::vec3 n;
			s >> n.x;
			s >> n.y;
			s >> n.z;
			normals.push_back(n);
		}
		else if (mode == "f")
		{
			unsigned int v = 0, vt = 0, vn = 0;

			for (int i = 0; i < 3; ++i)
			{
				// Read vertex index
				s >> v;

				// Read texture-coordinate index
				if (texCoords.empty())
					s.ignore(2);
				else
				{
					s.ignore(1);
					s >> vt;
					s.ignore(1);
				}

				// Read normal index
				s >> vn;

				// Add vertex to mesh
				vertices.push_back(
					{
						coords[v - 1],
						(vn - 1 < normals.size() ? normals[vn - 1] : glm::vec3{}),
						(vt - 1 < texCoords.size() ? texCoords[vt - 1] : glm::vec2{})
					}
				);
				mesh.numVertices++;
			}
		}
	}

	file.close();

	return mesh;
}
