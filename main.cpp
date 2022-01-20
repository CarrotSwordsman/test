#include "Angel.h"
#include "TriMesh.h"

#include <vector>
#include <string>

const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;

const int TRANSFORM_SCALE = 0;
const int TRANSFORM_ROTATE = 1;
const int TRANSFORM_TRANSLATE = 2;

const double DELTA_DELTA = 0.3;   // Delta�ı仯��
const double DEFAULT_DELTA = 0.5; // Ĭ�ϵ�Deltaֵ

double scaleDelta = DEFAULT_DELTA;
double rotateDelta = DEFAULT_DELTA;
double translateDelta = DEFAULT_DELTA;

glm::vec3 scaleTheta(1.0, 1.0, 1.0);     // ���ſ��Ʊ���
glm::vec3 rotateTheta(0.0, 0.0, 0.0);    // ��ת���Ʊ���
glm::vec3 translateTheta(0.0, 0.0, 0.0); // ƽ�ƿ��Ʊ���

int currentTransform = TRANSFORM_ROTATE; // ���õ�ǰ�任
int mainWindow;

struct openGLObject
{
    // �����������
    GLuint vao;
    // ���㻺�����
    GLuint vbo;

    // ��ɫ������
    GLuint program;
    // ��ɫ���ļ�
    std::string vshader;
    std::string fshader;
    // ��ɫ������
    GLuint pLocation;
    GLuint cLocation;
    GLuint matrixLocation;
    GLuint darkLocation;
};

openGLObject cube_object;

TriMesh *cube = new TriMesh();

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void bindObjectAndData(TriMesh *mesh, openGLObject &object, const std::string &vshader, const std::string &fshader)
{

    // ���������������
    glGenVertexArrays(1, &object.vao); // ����1�������������
    glBindVertexArray(object.vao);     // �󶨶����������

    // ��������ʼ�����㻺�����
    glGenBuffers(1, &object.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh->getPoints().size() * sizeof(glm::vec3) + mesh->getColors().size() * sizeof(glm::vec3),
                 NULL,
                 GL_STATIC_DRAW);

    // @TODO: Task3-�޸���TriMesh.cpp�Ĵ���ɺ��ٴ�����ע�ͣ��������ᱨ��
    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->getPoints().size() * sizeof(glm::vec3), &mesh->getPoints()[0]);
    glBufferSubData(GL_ARRAY_BUFFER, mesh->getPoints().size() * sizeof(glm::vec3), mesh->getColors().size() * sizeof(glm::vec3), &mesh->getColors()[0]);

    object.vshader = vshader;
    object.fshader = fshader;
    object.program = InitShader(object.vshader.c_str(), object.fshader.c_str());

    // �Ӷ�����ɫ���г�ʼ�������λ��
    object.pLocation = glGetAttribLocation(object.program, "vPosition");
    glEnableVertexAttribArray(object.pLocation);
    glVertexAttribPointer(object.pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // �Ӷ�����ɫ���г�ʼ���������ɫ
    object.cLocation = glGetAttribLocation(object.program, "vColor");
    glEnableVertexAttribArray(object.cLocation);
    glVertexAttribPointer(object.cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(mesh->getPoints().size() * sizeof(glm::vec3)));

    // ��þ���洢λ��
    object.matrixLocation = glGetUniformLocation(object.program, "matrix");
}

void init()
{
    std::string vshader, fshader;
    // ��ȡ��ɫ����ʹ��
    vshader = "shaders/vshader.glsl";
    fshader = "shaders/fshader.glsl";

    cube->generateCube();
    bindObjectAndData(cube, cube_object, vshader, fshader);

    // ��ɫ����
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display()
{
    // ������
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(cube_object.program);

    glBindVertexArray(cube_object.vao);

    // ��ʼ���任����
    glm::mat4 m(1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0);

    // @TODO: Task4-�ڴ˴��޸ĺ������������յı任����
    // ���ú����������ֱ仯�ı仯�����ۼӵõ��仯����
    // ע�����ֱ仯�ۼӵ�˳��
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, translateTheta);
    m = trans * m;
    /*glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::scale(trans, scaleTheta);
	m = trans * m;*/

    // ��ָ��λ��matrixLocation�д���任����m
    glUniformMatrix4fv(cube_object.matrixLocation, 1, GL_FALSE, glm::value_ptr(m));

    // �����������еĸ���������
    glDrawArrays(GL_TRIANGLES, 0, cube->getPoints().size());
}

// ͨ��Deltaֵ����Theta
void updateTheta(int axis, int sign)
{
    switch (currentTransform)
    {
        // ���ݱ任���ͣ����ӻ����ĳ�ֱ任�ı仯��
    case TRANSFORM_SCALE:
        scaleTheta[axis] += sign * scaleDelta;
        break;
    case TRANSFORM_ROTATE:
        rotateTheta[axis] += sign * rotateDelta;
        break;
    case TRANSFORM_TRANSLATE:
        translateTheta[axis] += sign * translateDelta;
        break;
    }
}

// ��ԭTheta��Delta
void resetTheta()
{
    scaleTheta = glm::vec3(1.0, 1.0, 1.0);
    rotateTheta = glm::vec3(0.0, 0.0, 0.0);
    translateTheta = glm::vec3(0.0, 0.0, 0.0);
    scaleDelta = DEFAULT_DELTA;
    rotateDelta = DEFAULT_DELTA;
    translateDelta = DEFAULT_DELTA;
}

// ���±仯Deltaֵ
void updateDelta(int sign)
{
    switch (currentTransform)
    {
        // ���ݱ仯�������ӻ����ÿһ�α仯�ĵ�λ�仯��
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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    switch (key)
    {
    // �˳���
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    // 1�����š�
    case GLFW_KEY_1:
        if (action == GLFW_PRESS)
            currentTransform = TRANSFORM_SCALE;
        break;
    // 2: ��ת��
    case GLFW_KEY_2:
        if (action == GLFW_PRESS)
            currentTransform = TRANSFORM_ROTATE;
        break;
    // 3: �ƶ���
    case GLFW_KEY_3:
        if (action == GLFW_PRESS)
            currentTransform = TRANSFORM_TRANSLATE;
        break;
    // 4: �����ߡ�
    case GLFW_KEY_4:
        if (action == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    // 5: �����档
    case GLFW_KEY_5:
        if (action == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        ;
        break;
    // Q: ���� x��
    case GLFW_KEY_Q:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            updateTheta(X_AXIS, 1);
        break;
    // A: ���� x��
    case GLFW_KEY_A:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            updateTheta(X_AXIS, -1);
        break;
    // W: ���� y��
    case GLFW_KEY_W:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            updateTheta(Y_AXIS, 1);
        break;
    // S: ���� x��
    case GLFW_KEY_S:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            updateTheta(Y_AXIS, -1);
        break;
    // E: ���� z��
    case GLFW_KEY_E:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            updateTheta(Z_AXIS, 1);
        break;
    // D: ���� z��
    case GLFW_KEY_D:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            updateTheta(Z_AXIS, -1);
        break;
    // R: ���ӱ仯����
    case GLFW_KEY_R:
        if (action == GLFW_PRESS)
            updateDelta(1);
        break;
    // F: ���ٱ仯����
    case GLFW_KEY_F:
        if (action == GLFW_PRESS)
            updateDelta(-1);
        break;
    // T: ����ֵ���á�
    case GLFW_KEY_T:
        if (action == GLFW_PRESS)
            resetTheta();
        break;
    }
}

void printHelp()
{
    printf("%s\n\n", "3D Transfomations");
    printf("Keyboard options:\n");
    printf("1: Transform Scale\n");
    printf("2: Transform Rotate\n");
    printf("3: Transform Translate\n");
    printf("q: Increase x\n");
    printf("a: Decrease x\n");
    printf("w: Increase y\n");
    printf("s: Decrease y\n");
    printf("e: Increase z\n");
    printf("d: Decrease z\n");
    printf("r: Increase delta of currently selected transform\n");
    printf("f: Decrease delta of currently selected transform\n");
    printf("t: Reset all transformations and deltas\n");
}

void cleanData()
{
    cube->cleanData();

    // �ͷ��ڴ�
    delete cube;
    cube = NULL;

    // ɾ���󶨵Ķ���
    glDeleteVertexArrays(1, &cube_object.vao);

    glDeleteBuffers(1, &cube_object.vbo);
    glDeleteProgram(cube_object.program);
}

int main(int argc, char **argv)
{
    // ��ʼ��GLFW�⣬������Ӧ�ó�����õĵ�һ��GLFW����
    glfwInit();

    // ����GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // ���ô�������
    GLFWwindow *window = glfwCreateWindow(600, 600, "3D Transfomations", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // �����κ�OpenGL�ĺ���֮ǰ��ʼ��GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    init();
    // ���������Ϣ
    printHelp();
    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        display();

        // ������ɫ���� �Լ� �����û�д���ʲô�¼�������������롢����ƶ��ȣ�
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    cleanData();

    return 0;
}