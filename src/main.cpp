#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
#include "shaders.hpp"
#include "stb/stb_image.h"
#include "camera.hpp"
#include "bezierCurve.hpp"
#include "bezierSurface.hpp"
#include "skinning.hpp"


using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const * path);
void create_pingpong(unsigned int pingpongFBO[], unsigned int pingpongBuffer[]);
void blur_lights_texture(Shader gaussianShader, unsigned int pingpongFBO[], unsigned int pingpongBuffer[],
                  unsigned int light_texture, unsigned int quadVAO);
void render_lights(Shader lightCubeShader, glm::mat4 projection, glm::mat4 view,
                   unsigned int lightVAO, glm::vec3 pointLightPositions[]);
void display_bloom_texture(Shader bloomShader, unsigned int quadVAO,
                           unsigned int hdrBuffer, unsigned int blurredLights);
void setup_quad_main_texture(unsigned int &quadVAO, unsigned int &quadVBO);
void setup_lights(unsigned int &lightVAO, unsigned int &VBO);
void setup_hdrFBO(unsigned int &hdrFBO, unsigned int colorBuffers[]);
GLFWwindow* init_glfw_glad();
BezierSurface init_BezierSurface();
void setup_lightingShader(Shader lightingShader, glm::vec3 pointLightPositions[], glm::mat4 &projection,
                    glm::mat4 &view, glm::mat4 &model);
vector<unsigned int> load_pbr_textures(filesystem::path path);
void drawPolygon();
void drawNormals();
void drawCurves();
void drawSurfaces();

// settings
unsigned int SCR_WIDTH = 1400;
unsigned int SCR_HEIGHT = 1000;
bool currentDrawingMode = true;
bool drawingModeKeyPressed = true;
bool drawNormalsState = false;
bool normalKeyPressed = false;
bool drawCurvesState = false;
bool updateSkinning = false;


BezierCurve bezierCurve1;//50 segments pas défaut
BezierCurve bezierCurve2;//idem


// camera
Camera camera(glm::vec3(3.0f, 3.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


int main() {

    GLFWwindow *window = init_glfw_glad();

    glEnable(GL_DEPTH_TEST);

    filesystem::path path = filesystem::current_path();
    path = path.parent_path();

    Shader lightingShader((path / "src/shaders/materialShader.vs").c_str(), (path / "src/shaders/PBR_texture_maps.fs").c_str());
//    Shader lightingShader((path / "src/shaders/materialShader.vs").c_str(), (path / "src/shaders/PBR.fs").c_str());
    Shader lightCubeShader((path / "src/shaders/lightShader.vs").c_str(), (path / "src/shaders/lightShader.fs").c_str());
    Shader screenShader((path / "src/shaders/screenShader.vs").c_str(), (path / "src/shaders/screenShader.fs").c_str());
    Shader normalShader((path / "src/shaders/normal.vs").c_str(), (path / "src/shaders/normal.fs").c_str(), (path / "src/shaders/normal.gs").c_str());
    Shader gaussianShader((path / "src/shaders/filtreGaussien.vs").c_str(), (path / "src/shaders/filtreGaussien.fs").c_str());
    Shader bloomShader((path / "src/shaders/bloom.vs").c_str(), (path / "src/shaders/bloom.fs").c_str());

    glm::vec3 pointLightPositions[] = {
            glm::vec3(0.f),
            glm::vec3(8.0f,6.0f,-30.0f),
            glm::vec3(24.0f, 8.0f, -12.0f)
    };

    BezierSurface bezierSurface = init_BezierSurface();

    Skinning anim(bezierSurface);

    vector<unsigned int> textures_pbr;
    textures_pbr = load_pbr_textures(path);

    // VAO et VBO pour les cubes de lumière
    unsigned int lightVAO;
    unsigned int VBO;
    setup_lights(lightVAO, VBO);


    // Le quad qui va couvrir l'écran pour la texture du framebuffer
    unsigned int quadVAO, quadVBO;
    setup_quad_main_texture(quadVAO, quadVBO);

    lightingShader.use();
//    lightingShader.setVec3("albedo", 0.f, 0.8f, 0.f);
//    lightingShader.setFloat("ao", 1.0f);
//    lightingShader.setFloat("metallic", 1.f);
//    lightingShader.setFloat("roughness", 0.4f);
    lightingShader.setInt("albedoMap", 0);
    lightingShader.setInt("normalMap", 1);
    lightingShader.setInt("metallicMap", 2);
    lightingShader.setInt("roughnessMap", 3);
    lightingShader.setInt("aoMap", 4);


    gaussianShader.use();
    gaussianShader.setInt("image", 0);
    bloomShader.use();
    bloomShader.setInt("scene", 0);
    bloomShader.setInt("bloomBlur", 1);

    /// frame buffer
    // set up floating point framebuffer to render scene to
    unsigned int hdrFBO, colorBuffers[2];
    setup_hdrFBO(hdrFBO, colorBuffers);

    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    create_pingpong(pingpongFBO, pingpongBuffer);


    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();

        glm::mat4 projection, view, model;

        setup_lightingShader(lightingShader, pointLightPositions, projection, view, model);

//        bezierSurface.Draw(textures_pbr);

//        if(updateSkinning){
//            anim.updateBones(0.001);
//            anim.Draw(textures_pbr);
//        }

        anim.Draw(textures_pbr);
        anim.updateBones(0.2);



        if(drawNormalsState){
            normalShader.use();
            normalShader.setMat4("projection", projection);
            normalShader.setMat4("view", view);
            normalShader.setMat4("model", model);
//            bezierSurface.Draw(textures_pbr);
            anim.Draw(textures_pbr);
        }

        render_lights(lightCubeShader, projection, view, lightVAO, pointLightPositions);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        blur_lights_texture(gaussianShader, pingpongFBO, pingpongBuffer,colorBuffers[1], quadVAO);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        display_bloom_texture(bloomShader, quadVAO, colorBuffers[0], pingpongBuffer[true]);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteFramebuffers(1, &hdrFBO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &quadVBO);

    bezierSurface.del();
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !drawingModeKeyPressed) {
        currentDrawingMode = !currentDrawingMode;
        drawingModeKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
        drawingModeKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !normalKeyPressed) {
        drawNormalsState = !drawNormalsState;
        normalKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE)
        normalKeyPressed = false;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        drawCurvesState = !drawCurvesState;
//    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        glfwSetCursorPosCallback(window, NULL);
        glfwSetScrollCallback(window, NULL);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    //Mettre le contrôle via la souris en appuyant sur p
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void drawPolygon(){
    if(currentDrawingMode){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }else{
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void create_pingpong(unsigned int pingpongFBO[], unsigned int pingpongBuffer[]){
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }
}

void blur_lights_texture(Shader gaussianShader, unsigned int pingpongFBO[], unsigned int pingpongBuffer[],
                  unsigned int light_texture, unsigned int quadVAO){
    bool horizontal = true, first_iteration = true;
    int amount = 20;
    gaussianShader.use();
    for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        gaussianShader.setInt("horizontal", horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? light_texture : pingpongBuffer[!horizontal]);

        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
}

void render_lights(Shader lightCubeShader, glm::mat4 projection, glm::mat4 view,
                   unsigned int lightVAO, glm::vec3 pointLightPositions[]){
    lightCubeShader.use();
    lightCubeShader.setMat4("projection", projection);
    lightCubeShader.setMat4("view", view);
    glBindVertexArray(lightVAO);

    for(unsigned int i = 0; i < 3; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        if(i==0){
            model = glm::translate(model,camera.Position - camera.Front);
        }else{
            model = glm::translate(model,pointLightPositions[i-1]);
        }
//            model = glm::scale(model, glm::vec3(0.5f)); // plus petit cube
        lightCubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void display_bloom_texture(Shader bloomShader, unsigned int quadVAO, unsigned int hdrBuffer, unsigned int blurredLights){


    glDisable(GL_DEPTH_TEST);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    bloomShader.use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrBuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, blurredLights);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void setup_quad_main_texture(unsigned int &quadVAO, unsigned int &quadVBO){
    float quadVertices[] = {
            // positions            // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void setup_lights(unsigned int &lightVAO, unsigned int &VBO){
    //cube pour les lumières
    float vertices[] = {
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f
    };

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void setup_hdrFBO(unsigned int &hdrFBO, unsigned int colorBuffers[]){
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

//    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLFWwindow* init_glfw_glad(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Mac OS uniquement
    #endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bezier Surfaces", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    //chargement des pointeurs de fonctions openGL avec glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        exit(-1);
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return window;
}

BezierSurface init_BezierSurface(){
    vector<vector<glm::vec3>> controlPoints;
    /// Courbes de bézier pour la création de la surface de bézier
    vector<glm::vec3> curve1 = {
            glm::vec3(1.0f,  3.0f, 1.0f),
            glm::vec3(1.0f, -1.8f, -2.2f),
            glm::vec3(1.0f, 1.6f, -5.8f),
            glm::vec3(1.0f,  2.6f, -9.3f)
    };
    vector<glm::vec3> curve2 = {
            glm::vec3(2.2f,  1.2f, 1.0f),
            glm::vec3(2.2f, 1.7f, -2.2f),
            glm::vec3(2.2f, -2.6f, -5.8f),
            glm::vec3(2.2f,  2.0f, -9.3f)
    };
    vector<glm::vec3> curve3 = {
            glm::vec3(5.6f,  2.0f, 1.0f),
            glm::vec3(5.6f, -1.8f, -2.2f),
            glm::vec3(5.6f, 1.6f, -5.8f),
            glm::vec3(5.6f,  1.2f, -9.3f)
    };
    vector<glm::vec3> curve4 = {
            glm::vec3(8.4f,  3.4f, 1.0f),
            glm::vec3(8.4f, 1.7f, -2.2f),
            glm::vec3(8.4f, -2.6f, -5.8f),
            glm::vec3(8.4f,  0.6f, -9.3f)
    };

    controlPoints.push_back(curve1);
    controlPoints.push_back(curve2);
    controlPoints.push_back(curve3);
    controlPoints.push_back(curve4);

    BezierSurface bezierSurface(controlPoints);

    return bezierSurface;
}

vector<unsigned int> load_pbr_textures(filesystem::path path){
    /// Textures pour pbr/brdf
    unsigned int albedo    = loadTexture((path / "src/textures/streaky-metal1-ue/streaky-metal1_albedo.png").c_str());
    unsigned int normal    = loadTexture((path / "src/textures/streaky-metal1-ue/streaky-metal1_normal-dx.png").c_str());
    unsigned int metallic  = loadTexture((path / "src/textures/streaky-metal1-ue/streaky-metal1_metallic.png").c_str());
    unsigned int roughness = loadTexture((path / "src/textures/streaky-metal1-ue/streaky-metal1_roughness.png").c_str());
    unsigned int ao        = loadTexture((path / "src/textures/streaky-metal1-ue/streaky-metal1_ao.png").c_str());

    vector<unsigned int> textures_pbr;
    textures_pbr.push_back(albedo);
    textures_pbr.push_back(normal);
    textures_pbr.push_back(metallic);
    textures_pbr.push_back(roughness);
    textures_pbr.push_back(ao);
    return textures_pbr;
}

void setup_lightingShader(Shader lightingShader, glm::vec3 pointLightPositions[], glm::mat4 &projection,
                    glm::mat4 &view, glm::mat4 &model){

    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = camera.GetViewMatrix();
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);


    lightingShader.setVec3("viewPos", camera.Position);

    lightingShader.setFloat("material.shininess", 32.0f);

    for(int i=0; i<3; i++){
        string pl = "pointLights[" + string(to_string(i)) + string("].");
        if(i==0){
            lightingShader.setVec3(pl + string("position"), camera.Position - camera.Front);
        }else{
            lightingShader.setVec3(pl + string("position"), pointLightPositions[i]);
        }
        lightingShader.setFloat(string(pl) + string("constant"), 1.0f);
        lightingShader.setFloat(string(pl) + string("linear"), 0.09f);
        lightingShader.setFloat(string(pl) + string("quadratic"), 0.032f);
        lightingShader.setVec3(pl + string("lightColor"), glm::vec3(50.0f, 50.0f, 50.0f));
    }

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(2.0f));
    model = glm::translate(model, glm::vec3(0.f, -5.f, 2.f));
    lightingShader.setMat4("model", model);
}