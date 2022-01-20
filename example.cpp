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

// ������Ƭ�еĶ�������
typedef struct vIndex
{
    unsigned int a, b, c;
    vIndex(int ia = 0, int ib = 0, int ic = 0) : a(ia), b(ib), c(ic) {}
} vec3i;

std::string fileName = "Models/cow.off";
std::vector<vec3> vertices; //���ļ��ж�ȡ�Ķ�������
std::vector<vec3i> faces;   //���ļ��ж�ȡ���涥������

int nVertices = 0;
int nFaces = 0;
int nEdges = 0;

std::vector<vec3> points; //������ɫ���Ļ��Ƶ�
std::vector<vec3> colors; //������ɫ������ɫ

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
    // @TODO���޸Ĵ˺�����ȡOFF�ļ�����άģ�͵���Ϣ

    //���������ַ���"OFF"

    //����㡢�桢����Ŀ
    vec3 point;
    // ��ȡÿ�����������
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

    // ��ȡ��Ķ�������

    fin.close();
}

void storeFacesPoints()
{
    points.clear();
    colors.clear();

    // @TODO�� �޸Ĵ˺�����points��colors�����д洢ÿ��������Ƭ�ĸ��������ɫ��Ϣ
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

    // ���������������
    GLuint vao[1];
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    // ��������ʼ�����㻺�����
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3) + colors.size() * sizeof(vec3), NULL, GL_STATIC_DRAW);

    // @TODO���޸���ɺ��ٴ�����ע�ͣ��������ᱨ��
    // �ֱ��ȡ����
    glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec3), &points[0]);
    glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), colors.size() * sizeof(vec3), &colors[0]);

    // ��������ʼ�����������������
    // @TODO: �޸����generateCube�������ٴ�����ע�ͣ��������ᱨ��
    GLuint vertexIndexBuffer;
    glGenBuffers(1, &vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(vec3i), faces.data(), GL_STATIC_DRAW);

    // ��ȡ��ɫ����ʹ��
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // �Ӷ�����ɫ���г�ʼ�������λ��
    GLuint pLocation = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(pLocation);
    glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    // ��ƬԪ��ɫ���г�ʼ���������ɫ
    GLuint cLocation = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(cLocation);
    glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec3)));

    // ��þ���洢λ��
    matrixLocation = glGetUniformLocation(program, "matrix");
    // ��ɫ����
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display()
{
    // ������
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ���ɱ任����
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
    // @TODO: �ڴ˴��޸ĺ���������任����

    // ��ָ��λ���д���任����
    glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, m);
    //glDrawArrays(GL_TRIANGLES, 0, points.size());
    //ע�������ǿ�ʼʹ��glDrawElements������
    glDrawArrays(GL_TRIANGLES, 0, points.size());

    glutSwapBuffers();
}

// ͨ��Deltaֵ����Theta
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

// ��ԭTheta��Delta
void resetTheta()
{
    scaleTheta = vec3(1.0, 1.0, 1.0);
    rotateTheta = vec3(0.0, 0.0, 0.0);
    translateTheta = vec3(0.0, 0.0, 0.0);
    scaleDelta = DEFAULT_DELTA;
    rotateDelta = DEFAULT_DELTA;
    translateDelta = DEFAULT_DELTA;
}

// ���±仯Deltaֵ
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
        // Esc����
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
    glutAddMenuEntry("x", TRANSFORM_X); //�����µĲ˵���xyz
    glutAddMenuEntry("y", TRANSFORM_Y);
    glutAddMenuEntry("z", TRANSFORM_Z);
    glutAttachMenu(GLUT_MIDDLE_BUTTON); //������Ҽ��ֱ�ٿ���ת��ʼ������м��ǵ����˵���
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

// ���ص�����
void windowMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // ��������������ʼ
        glutIdleFunc(idleFunction);
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        // �������ɿ�������ֹͣ
        glutIdleFunc(NULL);
    } //�����elseif�����������elseifע�͵����Ϳ��Բ���������һ�ξͿ���һֱ��ת��Ч��
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // ����Ҽ���������ʼ
        glutIdleFunc(idleFunction2);
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        // ����Ҽ��ɿ�������ֹͣ
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
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // ����֧��˫�ػ������Ȳ���
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
    // ���������Ϣ
    printHelp();
    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}