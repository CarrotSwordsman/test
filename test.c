#include "TriMesh.h"


// 一些基础颜色
const vec3 basic_colors[8] = {
	vec3(1.0, 1.0, 1.0),	// White
	vec3(1.0, 1.0, 0.0),	// Yellow
	vec3(0.0, 1.0, 0.0),	// Green
	vec3(0.0, 1.0, 1.0),	// Cyan
	vec3(1.0, 0.0, 1.0),	// Magenta
	vec3(1.0, 0.0, 0.0),	// Red
	vec3(0.0, 0.0, 0.0),	// Black
	vec3(0.0, 0.0, 1.0)		// Blue
};

// 立方体的各个点
const vec3 cube_vertices[8] = {
	vec3(-0.5, -0.5, -0.5),
	vec3(0.5, -0.5, -0.5),
	vec3(-0.5,  0.5, -0.5),
	vec3(0.5,  0.5, -0.5),
	vec3(-0.5, -0.5,  0.5),
	vec3(0.5, -0.5,  0.5),
	vec3(-0.5,  0.5,  0.5),
	vec3(0.5,  0.5,  0.5)
};

// 三角形的点
const vec3 triangle_vertices[3] = {
	vec3(-0.5, -0.5, 0.0),
	vec3(0.5, -0.5, 0.0),
	vec3(0.0, 0.5, 0.0)
};

// 正方形平面
const vec3 square_vertices[4] = {
	vec3(-0.5, -0.5, 0.0),
	vec3(0.5, -0.5, 0.0),
	vec3(0.5, 0.5, 0.0),
	vec3(-0.5, 0.5, 0.0),
};


TriMesh::TriMesh()
{
}

TriMesh::~TriMesh()
{
}

std::vector<vec3> TriMesh::getVertexPositions()
{
	return vertex_positions;
}

std::vector<vec3> TriMesh::getVertexColors()
{
	return vertex_colors;
}

std::vector<vec3> TriMesh::getVertexNormals()
{
	return vertex_normals;
}

std::vector<vec3i> TriMesh::getFaces()
{
	return faces;
}


std::vector<vec3> TriMesh::getPoints()
{
	return points;
}

std::vector<vec3> TriMesh::getColors()
{
	return colors;
}

std::vector<vec3> TriMesh::getNormals()
{
	return normals;
}


void TriMesh::computeTriangleNormals()
{
	// 这里的resize函数会给face_normals分配一个和faces一样大的空间
	face_normals.resize(faces.size());
	for (size_t i = 0; i < faces.size(); i++) {
		auto& face = faces[i];
		// 计算每个面片的法向量并归一化
		vec3 norm;		
		face_normals[i] = cross((vertex_positions[face.y]- vertex_positions[face.x]), (vertex_positions[face.z] - vertex_positions[face.x]));
		face_normals[i] = normalize(face_normals[i]);
		// face_normals[i] = norm;
	}
}

void TriMesh::computeVertexNormals()
{
	// 计算面片的法向量
	if (face_normals.size() == 0 && faces.size() > 0) {
		computeTriangleNormals();
	}
	
	// 这里的resize函数会给vertex_normals分配一个和vertex_positions一样大的空间
	// 并初始化法向量为0
	vertex_normals.resize(vertex_positions.size(), vec3(0, 0, 0));
	// 求法向量均值
	for (size_t i = 0; i < faces.size(); i++) {
		auto& face = faces[i];
		vertex_normals[face.x] += face_normals[i];
		vertex_normals[face.y] += face_normals[i];
		vertex_normals[face.z] += face_normals[i];
	}
	// 对累加的法向量并归一化
	for (size_t i = 0; i < vertex_normals.size(); i++) {
		vertex_normals[i] = normalize(vertex_normals[i] / length(vertex_normals[i]));
	}
}


vec3 TriMesh::getTranslation()
{
	return translation;
}

vec3 TriMesh::getRotation()
{
	return rotation;
}

vec3 TriMesh::getScale()
{
	return scale;
}

mat4 TriMesh::getModelMatrix()
{
	return Translate( getTranslation())*
		RotateZ( getRotation()[2])*
		RotateY( getRotation()[1])*
		RotateX( getRotation()[0])*
		Scale( getScale() );
}

void TriMesh::setTranslation(vec3 translation)
{
	this->translation = translation;
}

void TriMesh::setRotation(vec3 rotation)
{
	this->rotation= rotation;
}

void TriMesh::setScale(vec3 scale)
{
	this->scale = scale;
}

vec4 TriMesh::getAmbient() { return ambient; };
vec4 TriMesh::getDiffuse() { return diffuse; };
vec4 TriMesh::getSpecular() { return specular; };
float TriMesh::getShininess() { return shininess; };

void TriMesh::setAmbient(vec4 _ambient) { ambient = _ambient; };
void TriMesh::setDiffuse(vec4 _diffuse) { diffuse = _diffuse; };
void TriMesh::setSpecular(vec4 _specular) { specular = _specular; };
void TriMesh::setShininess(float _shininess) { shininess = _shininess; };

void TriMesh::cleanData() {
	vertex_positions.clear();
	vertex_colors.clear();
	vertex_normals.clear();
	
	faces.clear();
	face_normals.clear();

	points.clear();
	colors.clear();
	normals.clear();
}

void TriMesh::storeFacesPoints() {
	// 计算法向量
	if (vertex_normals.size() == 0)
		computeVertexNormals();
	
	// 根据每个三角面片的顶点下标存储要传入GPU的数据
	for (int i = 0; i < faces.size(); i++)
	{
		// 坐标
		points.push_back(vertex_positions[faces[i].x]);
		points.push_back(vertex_positions[faces[i].y]);
		points.push_back(vertex_positions[faces[i].z]);
		// 颜色
		colors.push_back(vertex_colors[faces[i].x]);
		colors.push_back(vertex_colors[faces[i].y]);
		colors.push_back(vertex_colors[faces[i].z]);
		// 法向量
		if (vertex_normals.size() != 0) {
			normals.push_back(vertex_normals[faces[i].x]);
			normals.push_back(vertex_normals[faces[i].y]);
			normals.push_back(vertex_normals[faces[i].z]);
		}
	}
}

// 立方体生成12个三角形的顶点索引
void TriMesh::generateCube() {
	// 创建顶点前要先把那些vector清空
	cleanData();

	// 存储立方体的各个面信息
	for (int i = 0; i < 8; i++)
	{
		vertex_positions.push_back(cube_vertices[i]);
		vertex_colors.push_back(basic_colors[i]);
	}

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 1, 3));
	faces.push_back(vec3i(0, 3, 2));
	faces.push_back(vec3i(1, 4, 5));
	faces.push_back(vec3i(1, 0, 4));
	faces.push_back(vec3i(4, 0, 2));
	faces.push_back(vec3i(4, 2, 6));
	faces.push_back(vec3i(5, 6, 4));
	faces.push_back(vec3i(5, 7, 6));
	faces.push_back(vec3i(2, 6, 7));
	faces.push_back(vec3i(2, 7, 3));
	faces.push_back(vec3i(1, 5, 7));
	faces.push_back(vec3i(1, 7, 3));

	storeFacesPoints();
}

void TriMesh::generateTriangle(vec3 color)
{
	// 创建顶点前要先把那些vector清空
	cleanData();

	for (int i = 0; i < 3; i++)
	{
		vertex_positions.push_back(triangle_vertices[i]);
		vertex_colors.push_back(color);
	}

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 1, 2));

	storeFacesPoints();
}


void TriMesh::generateSquare(vec3 color)
{
	// 创建顶点前要先把那些vector清空
	cleanData();

	for (int i = 0; i < 4; i++)
	{
		vertex_positions.push_back(square_vertices[i]);
		vertex_colors.push_back(color);
	}

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 1, 2));
	faces.push_back(vec3i(0, 2, 3));
	storeFacesPoints();
}

void TriMesh::readOff(const std::string& filename)
{
    // fin打开文件读取文件信息
    if (filename.empty())
    {
        return;
    }
    std::ifstream fin;
    fin.open(filename);
    if (!fin)
    {
        printf("File on error\n");
        return;
    }
    else
    {
        printf("File open success\n");

		cleanData();

		int nVertices, nFaces, nEdges;

        // 读取OFF字符串
        std::string str;
        fin >> str;
        // 读取文件中顶点数、面片数、边数
        fin >> nVertices >> nFaces >> nEdges;
        // 根据顶点数，循环读取每个顶点坐标
        for (int i = 0; i < nVertices; i++)
        {
            vec3 tmp_node;
            fin >> tmp_node.x >> tmp_node.y >> tmp_node.z;
            vertex_positions.push_back(tmp_node);
			vertex_colors.push_back(tmp_node);
        }
        // 根据面片数，循环读取每个面片信息，并用构建的vec3i结构体保存
        for (int i = 0; i < nFaces; i++)
        {
            int num, a, b, c;
            // num记录此面片由几个顶点构成，a、b、c为构成该面片顶点序号
            fin >> num >> a >> b >> c;
            faces.push_back(vec3i(a, b, c));
        }
    }
    fin.close();

    storeFacesPoints();
};


// Light
mat4 Light::getShadowProjectionMatrix() {
	// 这里只实现了Y=0平面上的阴影投影矩阵
	float lx, ly, lz;

	mat4 modelMatrix = this->getModelMatrix();
	vec4 light_position = modelMatrix * this->translation;
	
	lx = light_position[0];
	ly = light_position[1];
	lz = light_position[2];
	
	return mat4(
		-ly, 0.0, 0.0, 0.0,
		lx, 0.0, lz, 1.0,
		0.0, 0.0, -ly, 0.0,
		0.0, 0.0, 0.0, -ly
	);
}
