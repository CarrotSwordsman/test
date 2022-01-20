#include "TriMesh.h"


// һЩ������ɫ
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

// ������ĸ�����
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

// �����εĵ�
const vec3 triangle_vertices[3] = {
	vec3(-0.5, -0.5, 0.0),
	vec3(0.5, -0.5, 0.0),
	vec3(0.0, 0.5, 0.0)
};

// ������ƽ��
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
	// �����resize�������face_normals����һ����facesһ����Ŀռ�
	face_normals.resize(faces.size());
	for (size_t i = 0; i < faces.size(); i++) {
		auto& face = faces[i];
		// ����ÿ����Ƭ�ķ���������һ��
		vec3 norm;		
		face_normals[i] = cross((vertex_positions[face.y]- vertex_positions[face.x]), (vertex_positions[face.z] - vertex_positions[face.x]));
		face_normals[i] = normalize(face_normals[i]);
		// face_normals[i] = norm;
	}
}

void TriMesh::computeVertexNormals()
{
	// ������Ƭ�ķ�����
	if (face_normals.size() == 0 && faces.size() > 0) {
		computeTriangleNormals();
	}
	
	// �����resize�������vertex_normals����һ����vertex_positionsһ����Ŀռ�
	// ����ʼ��������Ϊ0
	vertex_normals.resize(vertex_positions.size(), vec3(0, 0, 0));
	// ��������ֵ
	for (size_t i = 0; i < faces.size(); i++) {
		auto& face = faces[i];
		vertex_normals[face.x] += face_normals[i];
		vertex_normals[face.y] += face_normals[i];
		vertex_normals[face.z] += face_normals[i];
	}
	// ���ۼӵķ���������һ��
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
	// ���㷨����
	if (vertex_normals.size() == 0)
		computeVertexNormals();
	
	// ����ÿ��������Ƭ�Ķ����±�洢Ҫ����GPU������
	for (int i = 0; i < faces.size(); i++)
	{
		// ����
		points.push_back(vertex_positions[faces[i].x]);
		points.push_back(vertex_positions[faces[i].y]);
		points.push_back(vertex_positions[faces[i].z]);
		// ��ɫ
		colors.push_back(vertex_colors[faces[i].x]);
		colors.push_back(vertex_colors[faces[i].y]);
		colors.push_back(vertex_colors[faces[i].z]);
		// ������
		if (vertex_normals.size() != 0) {
			normals.push_back(vertex_normals[faces[i].x]);
			normals.push_back(vertex_normals[faces[i].y]);
			normals.push_back(vertex_normals[faces[i].z]);
		}
	}
}

// ����������12�������εĶ�������
void TriMesh::generateCube() {
	// ��������ǰҪ�Ȱ���Щvector���
	cleanData();

	// �洢������ĸ�������Ϣ
	for (int i = 0; i < 8; i++)
	{
		vertex_positions.push_back(cube_vertices[i]);
		vertex_colors.push_back(basic_colors[i]);
	}

	// ÿ��������Ƭ�Ķ����±�
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
	// ��������ǰҪ�Ȱ���Щvector���
	cleanData();

	for (int i = 0; i < 3; i++)
	{
		vertex_positions.push_back(triangle_vertices[i]);
		vertex_colors.push_back(color);
	}

	// ÿ��������Ƭ�Ķ����±�
	faces.push_back(vec3i(0, 1, 2));

	storeFacesPoints();
}


void TriMesh::generateSquare(vec3 color)
{
	// ��������ǰҪ�Ȱ���Щvector���
	cleanData();

	for (int i = 0; i < 4; i++)
	{
		vertex_positions.push_back(square_vertices[i]);
		vertex_colors.push_back(color);
	}

	// ÿ��������Ƭ�Ķ����±�
	faces.push_back(vec3i(0, 1, 2));
	faces.push_back(vec3i(0, 2, 3));
	storeFacesPoints();
}

void TriMesh::readOff(const std::string& filename)
{
    // fin���ļ���ȡ�ļ���Ϣ
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

        // ��ȡOFF�ַ���
        std::string str;
        fin >> str;
        // ��ȡ�ļ��ж���������Ƭ��������
        fin >> nVertices >> nFaces >> nEdges;
        // ���ݶ�������ѭ����ȡÿ����������
        for (int i = 0; i < nVertices; i++)
        {
            vec3 tmp_node;
            fin >> tmp_node.x >> tmp_node.y >> tmp_node.z;
            vertex_positions.push_back(tmp_node);
			vertex_colors.push_back(tmp_node);
        }
        // ������Ƭ����ѭ����ȡÿ����Ƭ��Ϣ�����ù�����vec3i�ṹ�屣��
        for (int i = 0; i < nFaces; i++)
        {
            int num, a, b, c;
            // num��¼����Ƭ�ɼ������㹹�ɣ�a��b��cΪ���ɸ���Ƭ�������
            fin >> num >> a >> b >> c;
            faces.push_back(vec3i(a, b, c));
        }
    }
    fin.close();

    storeFacesPoints();
};


// Light
mat4 Light::getShadowProjectionMatrix() {
	// ����ֻʵ����Y=0ƽ���ϵ���ӰͶӰ����
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
