#include "Angel.h"
#include "mat.h"
#include "vec.h"
#include "TriMesh.h"
#include "Camera.h"
#include <iostream>

#include <vector>
#include <string>
#include <algorithm>


using namespace std;
int WIDTH = 600;
int HEIGHT = 600;

int mainWindow;

struct openGLObject
{
	// 顶点数组对象
	GLuint vao;
	// 顶点缓存对象
	GLuint vbo;

	// 着色器程序
	GLuint program;
	// 着色器文件
	std::string vshader;
	std::string fshader;
	// 着色器变量
	GLuint pLocation;
	GLuint cLocation;
	GLuint nLocation;

	// 投影变换变量
	GLuint modelLocation;
	GLuint viewLocation;
	GLuint projectionLocation;

	// 阴影变量
	GLuint shadowLocation;
};


openGLObject mesh_object;

Light* light = new Light();

TriMesh* mesh = new TriMesh();

Camera* camera = new Camera();

void bindObjectAndData(TriMesh* mesh, openGLObject& object, const std::string &vshader, const std::string &fshader) {

	// 创建顶点数组对象
	#ifdef __APPLE__	// for MacOS
		glGenVertexArraysAPPLE(1, &object.vao);		// 分配1个顶点数组对象
		glBindVertexArrayAPPLE(object.vao);		// 绑定顶点数组对象
	#else				// for Windows
		glGenVertexArrays(1, &object.vao);  	// 分配1个顶点数组对象
		glBindVertexArray(object.vao);  	// 绑定顶点数组对象
	#endif

	// 创建并初始化顶点缓存对象
	glGenBuffers(1, &object.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glBufferData(GL_ARRAY_BUFFER, 
		( mesh->getPoints().size() + mesh->getColors().size() + mesh->getNormals().size() ) * sizeof(vec3),
		NULL, 
		GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->getPoints().size() * sizeof(vec3), &mesh->getPoints()[0]);
	glBufferSubData(GL_ARRAY_BUFFER, mesh->getPoints().size() * sizeof(vec3), mesh->getColors().size() * sizeof(vec3), &mesh->getColors()[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (mesh->getPoints().size() + mesh->getColors().size()) * sizeof(vec3), mesh->getNormals().size() * sizeof(vec3), &mesh->getNormals()[0]);

	object.vshader = vshader;
	object.fshader = fshader;
	object.program = InitShader(object.vshader.c_str(), object.fshader.c_str());

	// 从顶点着色器中初始化顶点的坐标
	object.pLocation = glGetAttribLocation(object.program, "vPosition");
	glEnableVertexAttribArray(object.pLocation);
	glVertexAttribPointer(object.pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 从顶点着色器中初始化顶点的颜色
	object.cLocation = glGetAttribLocation(object.program, "vColor");
	glEnableVertexAttribArray(object.cLocation);
	glVertexAttribPointer(object.cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(mesh->getPoints().size() * sizeof(vec3)));

	// 从顶点着色器中初始化顶点的法向量
	object.nLocation = glGetAttribLocation(object.program, "vNormal");
	glEnableVertexAttribArray(object.nLocation);
	glVertexAttribPointer(object.nLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((mesh->getPoints().size() + mesh->getColors().size()) * sizeof(vec3)));
	
	// 获得矩阵位置
	object.modelLocation = glGetUniformLocation(object.program, "model");
	object.viewLocation = glGetUniformLocation(object.program, "view");
	object.projectionLocation = glGetUniformLocation(object.program, "projection");
	object.shadowLocation = glGetUniformLocation(object.program, "isShadow");
}

void bindLightAndMaterial(TriMesh* mesh, openGLObject& object, Light* light, Camera* camera) {

	// 传递相机的位置
	glUniform3fv( glGetUniformLocation(object.program, "eye_position"), 1, vec3(camera->eye[0], camera->eye[1], camera->eye[2]) );

	// 传递物体的材质
	vec4 meshAmbient = mesh->getAmbient();
	vec4 meshDiffuse = mesh->getDiffuse();
	vec4 meshSpecular = mesh->getSpecular();
	float meshShininess = mesh->getShininess();
	glUniform4fv(glGetUniformLocation(object.program, "material.ambient"), 1, meshAmbient);
	glUniform4fv(glGetUniformLocation(object.program, "material.diffuse"), 1, meshDiffuse);
	glUniform4fv(glGetUniformLocation(object.program, "material.specular"), 1, meshSpecular);
	glUniform1f(glGetUniformLocation(object.program, "material.shininess"), meshShininess);

	// 传递光源信息
	vec4 lightAmbient = light->getAmbient();
	vec4 lightDiffuse = light->getDiffuse();
	vec4 lightSpecular = light->getSpecular();
	vec3 lightPosition = light->getTranslation();

	glUniform4fv(glGetUniformLocation(object.program, "light.ambient"), 1, lightAmbient);
	glUniform4fv(glGetUniformLocation(object.program, "light.diffuse"), 1, lightDiffuse);
	glUniform4fv(glGetUniformLocation(object.program, "light.specular"), 1, lightSpecular);
	glUniform3fv(glGetUniformLocation(object.program, "light.position"), 1, lightPosition);

}


void init()
{
	std::string vshader, fshader;
	// 读取着色器并使用
	#ifdef __APPLE__	// for MacOS
		vshader = "shaders/vshader_mac.glsl";
		fshader = "shaders/fshader_mac.glsl";
	#else				// for Windows
		vshader = "shaders/vshader_win.glsl";
		fshader = "shaders/fshader_win.glsl";
	#endif

	// 设置光源位置
	light->setTranslation(vec3(0.0, 0.0, 2.0));
	light->setAmbient(vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射

	// 设置物体的旋转位移
	mesh->setTranslation(vec3(0.0, 0.0, 0.0));
	mesh->setRotation(vec3(0.0, 0.0, 0.0));
	mesh->setScale(vec3(1.0, 1.0, 1.0));

	// 设置材质
	mesh->setAmbient(vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	mesh->setDiffuse(vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	mesh->setSpecular(vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	mesh->setShininess(1.0); //高光系数
	
	// 将物体的顶点数据传递
	bindObjectAndData(mesh, mesh_object, vshader, fshader);

	glClearColor(0.0, 0.0, 0.0, 0.0);
}



void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 相机矩阵计算
	camera->updateCamera();
	camera->viewMatrix = camera->getViewMatrix();
	camera->projMatrix = camera->getProjectionMatrix(false);

	#ifdef __APPLE__	// for MacOS
		glBindVertexArrayAPPLE(mesh_object.vao);
	#else
		glBindVertexArray(mesh_object.vao);
	#endif
	glUseProgram(mesh_object.program);

	// 物体的变换矩阵
	mat4 modelMatrix = mesh->getModelMatrix();

	// 传递矩阵
	glUniformMatrix4fv(mesh_object.modelLocation, 1, GL_TRUE, &modelMatrix[0][0]);
	glUniformMatrix4fv(mesh_object.viewLocation, 1, GL_TRUE, &camera->viewMatrix[0][0]);
	glUniformMatrix4fv(mesh_object.projectionLocation, 1, GL_TRUE, &camera->projMatrix[0][0]);
	// 将着色器变量 isShadow 设置为0，表示正常绘制的颜色，如果是1着表示阴影
	glUniform1i(mesh_object.shadowLocation, 0);

	// 将材质和光源数据传递给着色器
	bindLightAndMaterial(mesh, mesh_object, light, camera);
	// 绘制
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

	glutSwapBuffers();
}


void printHelp()
{
	std::cout << "================================================" << std::endl;
	std::cout << "Use mouse to controll the light position (drag)." << std::endl;
	std::cout << "================================================" << std::endl << std::endl;

	std::cout << "Keyboard Usage" << std::endl;
	std::cout <<
		"[Window]" << std::endl <<
		"ESC:		Exit" << std::endl <<
		"h:		Print help message" << std::endl <<
		std::endl <<
		"[Model]" << std::endl <<
		"-:		Reset material parameters" << std::endl <<
		"1/2/3/!/@/#:	Change ambient parameters" << std::endl <<
		"4/5/6/$/%/^:	(TODO) Change diffuse parameters" << std::endl <<
		"7/8/9/&/*/(:	(TODO) Change specular parameters" << std::endl <<
		"0/):		(TODO) Change shininess parameters" << std::endl <<
		std::endl <<
		"q:		Load sphere model" << std::endl << 
		"a:		Load Pikachu model" << std::endl << 
		"w:		Load Squirtle model" << std::endl << 
		"s:		Load sphere_coarse model" << std::endl << 
		std::endl <<
		"[Camera]" << std::endl <<
		"SPACE:		Reset camera parameters" << std::endl <<
		"u/U:		Increase/Decrease the rotate angle" << std::endl <<
		"i/I:		Increase/Decrease the up angle" << std::endl <<
		"o/O:		Increase/Decrease the camera radius" << std::endl << std::endl;

}

// 鼠标响应函数
void mouse(int x, int y)
{
	// 用鼠标控制光源的位置lightPos，以实时更新光照效果
	// 窗口坐标系与图像坐标系转换
	float half_winx = WIDTH / 2.0;
	float half_winy = HEIGHT / 2.0;
	float lx = float(x - half_winx) / half_winx;
	float ly = float(HEIGHT - y - half_winy) / half_winy;
	
	vec3 pos = light->getTranslation();
	
	pos.x = lx;
	pos.y = ly;
	light->setTranslation(pos);
}

void mouseWheel(int button, int dir, int x, int y)
{
	vec3 pos = light->getTranslation();

	if (dir > 0)
	{
		pos.z += 0.1;
		// std::cout << "far from to light\n";
	}
	else
	{
		pos.z -= 0.1;
		// std::cout << "close to light\n";
	}
	light->setTranslation(pos);
}

// 键盘响应函数
void keyboard(unsigned char key, int x, int y)
{
	float tmp;
	vec4 ambient;
	switch (key)
	{
		// ESC键退出游戏
	case 033: exit(EXIT_SUCCESS); break;
	case 'h': printHelp(); break;

	case 'q':
		std::cout << "read sphere.off" << std::endl;
		mesh = new TriMesh();
		mesh->readOff("./assert/sphere.off");
		init();
		break;
	case 'a':
		std::cout << "read Pikachu.off" << std::endl;
		mesh = new TriMesh();
		mesh->readOff("./assert/Pikachu.off");
		init();
		break;
	case 'w':
		std::cout << "read Squirtle.off" << std::endl;
		mesh = new TriMesh();
		mesh->readOff("./assert/Squirtle.off");
		init();
		break;
	case 's':
		std::cout << "read sphere_coarse.off" << std::endl;
		mesh = new TriMesh();
		mesh->readOff("./assert/sphere_coarse.off");
		init();
		break;
	case '1': 
		ambient = mesh->getAmbient();
		tmp = ambient.x;
		ambient.x = std::min(tmp + 0.1, 1.0); 
		mesh->setAmbient(ambient);
		break;
	case '2':
		ambient = mesh->getAmbient();
		tmp = ambient.y;
		ambient.y = std::min(tmp + 0.1, 1.0);
		mesh->setAmbient(ambient);
		break;
	case '3':
		ambient = mesh->getAmbient();
		tmp = ambient.z;
		ambient.z = std::min(tmp + 0.1, 1.0);
		mesh->setAmbient(ambient);
		break;
		
	case '!':
		ambient = mesh->getAmbient();
		tmp = ambient.x;
		ambient.x = std::max(tmp - 0.1, 0.0);
		mesh->setAmbient(ambient);
		break;
	case '@':
		ambient = mesh->getAmbient();
		tmp = ambient.y;
		ambient.y = std::max(tmp - 0.1, 0.0);
		mesh->setAmbient(ambient);
		break; 
	case '#':
		ambient = mesh->getAmbient();
		tmp = ambient.z;
		ambient.z = std::max(tmp - 0.1, 0.0);
		mesh->setAmbient(ambient);
		break;

	// 控制调整材质，以实时更新光照效果

	case '-':
		mesh->setAmbient(vec4(0.2, 0.2, 0.2, 1.0));
		mesh->setDiffuse(vec4(0.7, 0.7, 0.7, 1.0));
		mesh->setSpecular(vec4(0.2, 0.2, 0.2, 1.0));
		mesh->setShininess(1.0);
		break;
	case '=':
		std::cout << "ambient: " << mesh->getAmbient() << std::endl;
		std::cout << "diffuse: " << mesh->getDiffuse() << std::endl;
		std::cout << "specular: " << mesh->getSpecular() << std::endl;
		std::cout << "shininess: " << mesh->getShininess() << std::endl;
		break;
	case ' ':
		cin >> a >> b >> c;
		break;

	// 通过按键改变相机和投影的参数
	default:
		camera->keyboard(key, x, y);
		break;

	}
	glutPostRedisplay();

}


void idle(void)
{
	glutPostRedisplay();
}

// 重新设置窗口
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

void cleanData() {
	mesh->cleanData();
	
	delete camera;
	camera = NULL;

	// 释放内存
	delete mesh;
	mesh = NULL;

	// 删除绑定的对象
	#ifdef __APPLE__
		glDeleteVertexArraysAPPLE(1, &mesh_object.vao);
	#else
		glDeleteVertexArrays(1, &mesh_object.vao);
	#endif
	glDeleteBuffers(1, &mesh_object.vbo);
	glDeleteProgram(mesh_object.program);

}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	// 窗口支持双重缓冲、深度测试、超采样
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	mainWindow = glutCreateWindow("Phong-Vertex");
	 
	#ifdef __APPLE__
	#else
		glewExperimental = GL_TRUE;
		glewInit();
	#endif

	mesh->readOff("./assert/sphere.off");
	// Init mesh, shaders, buffer
	init();

	// bind callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouse);
	// glutMouseWheelFunc(mouseWheel);
	glutIdleFunc(idle);

	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();


	cleanData();


	return 0;
}

