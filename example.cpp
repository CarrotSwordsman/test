#include "Angel.h"
#include "mat.h"
#include "vec.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#pragma comment(lib, "glew32.lib")

const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;

const int TRANSFORM_SCALE = 0;
const int TRANSFORM_ROTATE = 1;
const int TRANSFORM_TRANSLATE = 2;
const int TRANSFORM_X = 3;
const int TRANSFORM_Y = 4;
const int TRANSFORM_Z = 5;

const double DELTA_DELTA = 0.1;
const double DEFAULT_DELTA = 0.3;

double scaleDelta = DEFAULT_DELTA;
double rotateDelta = DEFAULT_DELTA;
double translateDelta = DEFAULT_DELTA;

vec3 scaleTheta(1.0, 1.0, 1.0);
vec3 rotateTheta(0.0, 0.0, 0.0);
vec3 translateTheta(0.0, 0.0, 0.0);

GLint matrixLocation;
int currentTransform = TRANSFORM_TRANSLATE;
int mainWindow;

// 三角面片中的顶点序列
typedef struct vIndex
{
    unsigned int a, b, c;
    vIndex(int ia = 0, int ib = 0, int ic = 0) : a(ia), b(ib), c(ic) {}
} vec3i;

std::string fileName = "Models/cow.off";
std::vector<vec3> vertices; //从文件中读取的顶点坐标
std::vector<vec3i> faces;   //从文件中读取的面顶点索引

int nVertices = 0;
int nFaces = 0;
int nEdges = 0;

std::vector<vec3> points; //传入着色器的绘制点
std::vector<vec3> colors; //传入着色器的颜色

void read_off(const std::string filename)
{
    if (filename.empty())
    {
        return;
    }

    std::ifstream fin;
    std::string a;
    fin.open(filename);
    fin >> a;
    fin >> nVertices >> nFaces >> nEdges;
    // @TODO：修改此函数读取OFF文件中三维模型的信息

    //读入首行字符串"OFF"

    //读入点、面、边数目
    vec3 point;
    // 读取每个顶点的坐标
    for (int i = 0; i < nVertices; i++)
    {
        fin >> point.x >> point.y >> point.z;
        vertices.push_back(point);
    }
    vec3i point2;
    for (int i = 0; i < nFaces; i++)
    {
        int num;
        fin >> num;
        fin >> point2.a >> point2.b >> point2.c;
        faces.push_back(point2);
    }

    // 读取面的顶点序列

    fin.close();
}

void storeFacesPoints()
{
    points.clear();
    colors.clear();

    // @TODO： 修改此函数在points和colors容器中存储每个三角面片的各个点和颜色信息
    for (int i = 0; i < nFaces; i++)
    {
        points.push_back(vertices[faces[i].a]);
        points.push_back(vertices[faces[i].b]);
        points.push_back(vertices[faces[i].c]);
    }
}

void init()
{
    storeFacesPoints();

    // 创建顶点数组对象
    GLuint vao[1];
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    // 创建并初始化顶点缓存对象
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3) + colors.size() * sizeof(vec3), NULL, GL_STATIC_DRAW);

    // @TODO：修改完成后再打开下面注释，否则程序会报错
    // 分别读取数据
    glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec3), &points[0]);
    glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), colors.size() * sizeof(vec3), &colors[0]);

    // 创建并初始化顶点索引缓存对象
    // @TODO: 修改完成generateCube函数后再打开下面注释，否则程序会报错
    GLuint vertexIndexBuffer;
    glGenBuffers(1, &vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(vec3i), faces.data(), GL_STATIC_DRAW);

    // 读取着色器并使用
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // 从顶点着色器中初始化顶点的位置
    GLuint pLocation = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(pLocation);
    glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    // 从片元着色器中初始化顶点的颜色
    GLuint cLocation = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(cLocation);
    glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec3)));

    // 获得矩阵存储位置
    matrixLocation = glGetUniformLocation(program, "matrix");
    // 黑色背景
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display()
{
    // 清理窗口
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 生成变换矩阵
    mat4 m(1.0, 0.0, 0.0, 0.0,
           0.0, 1.0, 0.0, 0.0,
           0.0, 0.0, 1.0, 0.0,
           0.0, 0.0, 0.0, 1.0);

    mat4 T = Translate(translateTheta);
    mat4 X = RotateX(rotateTheta.x);
    mat4 Y = RotateY(rotateTheta.y);
    mat4 Z = RotateZ(rotateTheta.z);
    mat4 S = Scale(scaleTheta);

    m = T * X * Y * Z * S * m;
    // @TODO: 在此处修改函数，计算变换矩阵

    // 从指定位置中传入变换矩阵
    glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, m);
    //glDrawArrays(GL_TRIANGLES, 0, points.size());
    //注意这里是开始使用glDrawElements函数了
    glDrawArrays(GL_TRIANGLES, 0, points.size());

    glutSwapBuffers();
}

// 通过Delta值更新Theta
void updateTheta(int axis, int sign)
{
    switch (currentTransform)
    {
    case TRANSFORM_SCALE:
        scaleTheta[axis] += sign * scaleDelta;
        break;
    case TRANSFORM_ROTATE:
        rotateTheta[axis] += sign * rotateDelta;
        break;
    case TRANSFORM_TRANSLATE:
        translateTheta[axis] += sign * translateDelta;
        break;
    case TRANSFORM_X:

        break;
    case TRANSFORM_Y:

        break;
    case TRANSFORM_Z:

        break;
    }
}

// 复原Theta和Delta
void resetTheta()
{
    scaleTheta = vec3(1.0, 1.0, 1.0);
    rotateTheta = vec3(0.0, 0.0, 0.0);
    translateTheta = vec3(0.0, 0.0, 0.0);
    scaleDelta = DEFAULT_DELTA;
    rotateDelta = DEFAULT_DELTA;
    translateDelta = DEFAULT_DELTA;
}

// 更新变化Delta值
void updateDelta(int sign)
{
    switch (currentTransform)
    {
    case TRANSFORM_SCALE:
        scaleDelta += sign * DELTA_DELTA;
        break;
    case TRANSFORM_ROTATE:
        rotateDelta += sign * DELTA_DELTA;
        break;
    case TRANSFORM_TRANSLATE:
        translateDelta += sign * DELTA_DELTA;
        break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
        updateTheta(X_AXIS, 1);
        break;
    case 'a':
        updateTheta(X_AXIS, -1);
        break;
    case 'w':
        updateTheta(Y_AXIS, 1);
        break;
    case 's':
        updateTheta(Y_AXIS, -1);
        break;
    case 'e':
        updateTheta(Z_AXIS, 1);
        break;
    case 'd':
        updateTheta(Z_AXIS, -1);
        break;
    case 'r':
        updateDelta(1);
        break;
    case 'f':
        updateDelta(-1);
        break;
    case 't':
        resetTheta();
        break;
    case 033:
        // Esc按键
        exit(EXIT_SUCCESS);
        break;
    }
    glutPostWindowRedisplay(mainWindow);
}

void menuEvents(int menuChoice)
{
    currentTransform = menuChoice;
}

void setupMenu()
{
    glutCreateMenu(menuEvents);
    glutAddMenuEntry("Scale", TRANSFORM_SCALE);
    glutAddMenuEntry("Rotate", TRANSFORM_ROTATE);
    glutAddMenuEntry("Translate", TRANSFORM_TRANSLATE);
    glutAddMenuEntry("x", TRANSFORM_X); //加入新的菜单栏xyz
    glutAddMenuEntry("y", TRANSFORM_Y);
    glutAddMenuEntry("z", TRANSFORM_Z);
    glutAttachMenu(GLUT_MIDDLE_BUTTON); //左键和右键分别操控旋转开始，因此中键是调出菜单栏
}

void idleFunction()
{
    switch (currentTransform)
    {
    case TRANSFORM_X:
        rotateTheta.x -= 0.5 * rotateDelta;
        break;
    case TRANSFORM_Y:
        rotateTheta.y -= 0.5 * rotateDelta;
        break;
    case TRANSFORM_Z:
        rotateTheta.z -= 0.5 * rotateDelta;
        break;
    }
    glutPostWindowRedisplay(mainWindow);
}

void idleFunction2()
{
    switch (currentTransform)
    {
    case TRANSFORM_X:
        rotateTheta.x += 0.5 * rotateDelta;
        break;
    case TRANSFORM_Y:
        rotateTheta.y += 0.5 * rotateDelta;
        break;
    case TRANSFORM_Z:
        rotateTheta.z += 0.5 * rotateDelta;
        break;
    }
    glutPostWindowRedisplay(mainWindow);
}

// 鼠标回调函数
void windowMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // 鼠标左键，动画开始
        glutIdleFunc(idleFunction);
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        // 鼠标左键松开，动画停止
        glutIdleFunc(NULL);
    } //将这个elseif和最下面这个elseif注释掉，就可以产生按下了一次就可以一直旋转的效果
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // 鼠标右键，动画开始
        glutIdleFunc(idleFunction2);
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        // 鼠标右键松开，动画停止
        glutIdleFunc(NULL);
    }
}

void printHelp()
{
    printf("%s\n\n", "3D Transfomations");
    printf("Keyboard options:\n");
    printf("q: Increase x\n");
    printf("a: Decrease x\n");
    printf("w: Increase y\n");
    printf("s: Decrease y\n");
    printf("e: Increase z\n");
    printf("d: Decrease z\n");
    printf("r: Increase delta of currently selected transform\n");
    printf("f: Decrease delta of currently selected transform\n");
    printf("t: Reset all transformations and deltas\n");
    printf("\n \n \nif you select x and push left-button of the mouse,it will rotate arount x axis clockwise.\n");
    printf("if you push right-button of the mouse,it will rotate arount x axis  counterclockwise.\n");
    printf("And if you release the button,it will stop rotating.");
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // 窗口支持双重缓冲和深度测试
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    mainWindow = glutCreateWindow("3D Transfomations");

    glewExperimental = GL_TRUE;
    glewInit();

    read_off(fileName);

    init();
    setupMenu();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(windowMouse);
    // 输出帮助信息
    printHelp();
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}