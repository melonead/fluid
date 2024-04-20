#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <Sph.h>
#include <chrono>
#include <vector>
#include <WinBase.h>
#include "Settings.h"
#include "Hashtable.h"
#include "shader.h"
// #include <SOIL2/SOIL2.h>


#define numVAOs 1
#define numVBOs 5

std::vector<Particle*> particleTable[NUMCELLS]{};

const int NUMSEGMENTS = 180;
float CircleVertices[NUMSEGMENTS * 3 * 3];
const float PI = 3.142857f;
const float DEG2RAD = PI / 180.0f;

Particle SimParticles[NUMPARTICLES] = {};

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
float pyrLocX, pyrLocY, pyrLocZ;
GLuint brickTexture;
GLuint texture;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint mvLoc, projLoc;

float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, tMat, rMat;

glm::vec2 translations[NUMPARTICLES];

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void display(GLFWwindow* window,
    Shader shaderProgram,
    Shader lineShaderProgram,
    Shader circleShaderProgram,
    Shader rectShaderProgram,
    double deltaTime);
void init(GLFWwindow* window);
void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight);
GLuint load_stb_image(const char* image_path);
void setUpCircleVertices(float radius, float cx, float cy);
float randomFloat(float Min, float Max);
void initParticles();
void drawLine(float start[], float end[]);
void drawCircle(float center[], float radius);
void drawRect(double topLeft[], double height, double width);
glm::vec4 screenToWorldSpace(double mousePos[], glm::mat4 projection, glm::mat4 viewMatrix);
void interactWithMouse(Particle& p, glm::vec4 mousePos, bool mouseClick[]);

const char* vPath = "shader/vertex_shader.glsl";
const char* fPath = "shader/fragment_shader.glsl";

const char* linevPath = "shader/lineVertex.glsl";
const char* linefPath = "shader/lineFragment.glsl";

const char* circlevPath = "shader/circleVertex.glsl";
const char* circlefPath = "shader/circleFragment.glsl";

const char* rectvPath = "shader/rectVertex.glsl";
const char* rectfPath = "shader/rectFragment.glsl";

int SCR_WIDTH =  1200;
int SCR_HEIGHT = 600;
double gravityAccel = -9.8;


// mouse stuff
bool mouseKeyPressed[] = { false, false};
glm::vec4 mousePosition;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
        
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window; 
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fluid Simulation", NULL, NULL);
    //glfwSetWindowSizeLimits(window, 800, 600, 1200, 1200);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // time
    double lastTime = glfwGetTime();
    double deltaTime = 0.0;

    // initialize

    init(window);
    glfwSetWindowSizeCallback(window, window_reshape_callback);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    initParticles();
    
    // render loop
    // -----------
    Shader shaderProgram(vPath, fPath);
    Shader lineShaderProgram(linevPath, linefPath);
    Shader circleShaderProgram(circlevPath, circlefPath);
    Shader rectShaderProgram(rectvPath, rectfPath);
    while (!glfwWindowShouldClose(window))
    {
        
        deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
        processInput(window);
        /*if (mouseKeyPressed[0])
        {
            cameraZ += 0.1f;
        }
        else if (mouseKeyPressed[1])
        {
            cameraZ -= 0.1f;
        }*/

      
        vMat = glm::translate(glm::mat4(1.0), glm::vec3(-cameraX, -cameraY, -cameraZ));
        deltaTime = (1.0 / 120.0);
        /*if (glfwGetTime() > 20)
            gravityAccel = -9.8;*/
        display(window, shaderProgram, lineShaderProgram, circleShaderProgram, rectShaderProgram, deltaTime);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        mouseKeyPressed[0] = true;
    }
    else {
        mouseKeyPressed[0] = false;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    {
        mouseKeyPressed[1] = true;
    }
    else {
        mouseKeyPressed[1] = false;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void init(GLFWwindow* window)
{
    // generate and bind voa buffer
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);

    // generate and bind vbo buffer
    glGenBuffers(numVBOs, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // vertex positions

    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 24.2001f;
    
    setUpCircleVertices(0.15f, 0.0f, 0.0f);

    // projection matrix
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    pMat = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    // load image
    texture = load_stb_image("resources/textures/earthmap1k.jpg");
    
    vMat = {
        1.0f,     0.0f,     0.0f,     0.0f,
        0.0f,     1.0f,     0.0f,     0.0f,
        0.0f,     0.0f,     1.0f,     0.0f,
        -cameraX, -cameraY, -cameraZ, 1.0f
    };

    mMat = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

}


void display(GLFWwindow* window,
    Shader shaderProgram,
    Shader lineShaderProgram,
    Shader circleShaderProgram,
    Shader rectShaderProgram,
    double deltaTime)
{
    // glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    mvMat = mMat * vMat;

    circleShaderProgram.use();
    circleShaderProgram.setMatrix4fv("mv_matrix", mvMat);
    circleShaderProgram.setMatrix4fv("proj_matrix", pMat);
    for (int i = 0; i < NUMPARTICLES; i++)
    {
        glm::vec2 trans;
        trans.x = SimParticles[i].pos[0];
        trans.y = SimParticles[i].pos[1];

        std::vector<Particle*> neighbors;

        getNeighbors(SimParticles[i].pos, neighbors, particleTable);

        translations[i] = trans;
        computeDensity(SimParticles[i], neighbors, neighbors.size());
        computeForces(SimParticles[i], neighbors, neighbors.size());
        LeapFrogIntegration(SimParticles[i], deltaTime, gravityAccel);

        interactWithMouse(SimParticles[i], mousePosition, mouseKeyPressed);
        //std::cout << particleTable[SimParticles[i].key].size() << std::endl;
       
        int k = computeKey(SimParticles[i].pos);
        //std::cout << "22222222222222222222222222222222222222222222222222222222222222222222222222222222222222" << std::endl;
        //std::cout << " FOR PARTICLE ID = " << SimParticles[i].id << std::endl;
        //std::cout << "new key = " << k << " older key = " << SimParticles[i].key << std::endl;
        //std::cout << "==================================================" << std::endl;
        //std::cout << " FOR PARTICLE ID = " << particleTable[k][SimParticles[i].index]->id << std::endl;
        //std::cout << "new key = " << k << " older key = " << particleTable[k][SimParticles[i].index]->key << std::endl;
        //std::cout << "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" << std::endl;
        //if ((k != SimParticles[i].key) && !particleTable[SimParticles[i].key].empty())
        //{
        //    SimParticles[i].prevKey = SimParticles[i].key;
        //    SimParticles[i].prevIndex = SimParticles[i].index;
        //    
        //    // remove it from its current cell
        //    particleTable[SimParticles[i].prevKey].erase(particleTable[SimParticles[i].prevKey].begin() + SimParticles[i].prevIndex);
        //    // put it in the cell it belongs to now
        //    insertInCell(SimParticles[i], particleTable);


        //    //std::vector<Particle>::iterator pIter;
        //    //for (pIter = particleTable[SimParticles[i].key].begin(); pIter != particleTable[SimParticles[i].key].end(); pIter++)
        //    //{
        //    //    if ((*pIter).id == SimParticles[i].id)
        //    //    {
        //    //        /*std::cout << "found match" << std::endl;
        //    //        std::cout << "Size is = " << particleTable[SimParticles[i].key].size() << std::endl;
        //    //        std::cout << "Particle index is " << SimParticles[i].index << std::endl;*/
        //    //        particleTable[SimParticles[i].key].erase(pIter);
        //    //        insertInCell(SimParticles[i], particleTable);
        //    //        break;
        //    //    }
        //    //}


        //    
        //}

        neighbors.clear();
    }
    // clear the table
    clearTable(particleTable);
    // repopulate the table with updated positions
    for (int i = 0; i < NUMPARTICLES; i++)
    {
        insertInCell(&SimParticles[i], particleTable);
    }

    shaderProgram.use();
    shaderProgram.setMatrix4fv("mv_matrix", mvMat);
    shaderProgram.setMatrix4fv("proj_matrix", pMat);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(translations), translations, GL_STATIC_DRAW);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(vao[0]);
    glDrawArraysInstanced(GL_TRIANGLES, 0, NUMSEGMENTS * 3, NUMPARTICLES);
    

    lineShaderProgram.use();
    lineShaderProgram.setMatrix4fv("mv_matrix", mvMat);
    lineShaderProgram.setMatrix4fv("proj_matrix", pMat);
    //float cellSize = IRADIUS;
    //int yRange = (Y_MAX_BOUND - Y_MIN_BOUND) / cellSize;
    //int xRange = (X_MAX_BOUND - X_MIN_BOUND) / cellSize;

    //// draw horizontal lines
    //for (int y = 0; y < yRange + 1; y++)
    //{
    //    float start[] = {X_MIN_BOUND, Y_MAX_BOUND - cellSize * y};
    //    float end[] = {X_MAX_BOUND, Y_MAX_BOUND - cellSize * y};
    //    drawLine(start, end);
    //}
    //// draw vertical lines
    //for (int x = 0; x < xRange + 1; x++)
    //{
    //    float start[] = { X_MIN_BOUND + cellSize * x, Y_MAX_BOUND};
    //    float end[] = { X_MIN_BOUND + cellSize * x, Y_MIN_BOUND};
    //    drawLine(start, end);
    //}

    //rectShaderProgram.use();
    //rectShaderProgram.setMatrix4fv("mv_matrix", mvMat);
    //rectShaderProgram.setMatrix4fv("proj_matrix", pMat);
    double mousePosX;
    double mousePosY;
    glfwGetCursorPos(window, &mousePosX, &mousePosY);
    
    //std::cout << SCR_WIDTH / xRange << std::endl;
    //double cellPos[] = { floorl((mousePosX / IRADIUS) / 30), floorl((mousePosY / IRADIUS) / 30) };
    double mPos[] = { mousePosX, mousePosY };
    
    glm::vec4 p = screenToWorldSpace(mPos, pMat, vMat);
    mPos[0] = p.x * cameraZ;
    mPos[1] = p.y * cameraZ;
    mousePosition.x = mPos[0];
    mousePosition.y = mPos[1];

    /*glm::vec2 mCellPos = getCellPosition(mPos);

    std::cout << tableHash(mCellPos) << std::endl;*/

}

/* setUpCircleVertices: creates and sets up vertices for rendering circle*/
void setUpCircleVertices(float radius, float cx, float cy) // OPTIMIZE: duplicate vertices used to create the circle's tri
{   
    float angle = 360 / NUMSEGMENTS;
    float temp[NUMSEGMENTS * 3];
    int jump1 = 0;
    for (int i = 0; i < NUMSEGMENTS; i++)
    {   
        // get the x and y of the position
        float x = radius * cosf(angle * i * PI / 180);
        float y = radius * sinf(angle * i * PI / 180);
        
        temp[i * 3] = x;
        temp[i * 3 + 1] = y;
        temp[i * 3 + 2] = 0;

    }

    int index = 0;
    int temp_index = 0;
    for (int j = 0; j < NUMSEGMENTS; j++)
    {
  
        // put the first 3 x, y, 0 components
        CircleVertices[index] = temp[temp_index];
        CircleVertices[index + 1] = temp[temp_index + 1];
        CircleVertices[index + 2] = temp[temp_index + 2];

        // skip 3 places
        index += 3;
        if (j != NUMSEGMENTS - 1)
        {
            CircleVertices[index] = temp[temp_index + 3];
            CircleVertices[index + 1] = temp[temp_index + 4];
            CircleVertices[index + 2] = temp[temp_index + 5];
        }
        else
        {
            CircleVertices[index] = temp[0];
            CircleVertices[index + 1] = temp[1];
            CircleVertices[index + 2] = temp[2];
        }

        index += 3;
        CircleVertices[index] = cx;
        CircleVertices[index + 1] = cy;
        CircleVertices[index + 2] = 0;

        temp_index += 3;
        index += 3;

    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(CircleVertices), CircleVertices, GL_STATIC_DRAW);
}


float randomFloat(float Min, float Max)
{
    return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}


void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight) {
    aspect = (float)newWidth / (float)newHeight; // new width&height provided by the callback
    glViewport(0, 0, newWidth, newHeight); // sets screen region associated with framebuffer 
    pMat = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void mouse_call_back()
{

}

GLuint load_stb_image(const char* image_path)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    // set the texture filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(image_path, & width, &height, &nrChannels, 0);
    
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return tex;
}

// initParticles: initialize the particles in a grid shape
void initParticles()
{
    float range = 200.0f;
    int size = std::sqrt(NUMPARTICLES);
    float startPos[2] = {-2.5f, -2.1f};
    float spacing = 3.0f;
    int c = 0;
    //std::cout << "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" << std::endl;
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {   
            int index = x + y * size;
            SimParticles[index].mass = 1;
            SimParticles[index].pressure = 0;
            SimParticles[index].pos[0] = startPos[0] + x / (spacing * 0.5);
            SimParticles[index].pos[1] = startPos[1] + y / spacing;
            SimParticles[index].id = index;
            insertInCell(&SimParticles[index], particleTable);
            Particle* ptr = &SimParticles[index];
            /*std::cout << "------------------------------------------------------ excepting only one initialization ---------------------------------" << std::endl;*/
           /* std::cout << "key Sim = " << SimParticles[index].key << std::endl;
            std::cout << "key Table = " << ptr->key << std::endl;*/

        }
    }
}

void drawLine(float start[], float end[])
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);

    float verts[] =
    {
        end[0],   end[1],    0.0,
        start[0], start[1],  0.0
    };


    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(vao[0]);
    glDrawArrays(GL_LINES, 0, 2);
}

void drawRect(double topLeft[], double height, double width)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);

    float verts[] =
    {
        topLeft[0], topLeft[1] + height, 0.0,// bottom left
        topLeft[0] + width, topLeft[1] + height, 0.0, // bottom right
        
        topLeft[0] + width, topLeft[1], 0.0f, // top right
        topLeft[0], topLeft[1], 0.0 // top left
    };


    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(vao[0]);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void drawCircle(float center[], float radius)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    int const numVertices = 20;
    float vertices[numVertices * 3];
    float angle = (360.0f / numVertices) * DEG2RAD;
    for (int i = 0; i < numVertices; i++)
    {
        vertices[i * 3] = center[0] + cosf(angle * i) * radius;
        vertices[i * 3 + 1] = center[1] + sinf(angle * i) * radius;
        vertices[i * 3 + 2] = 0;
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(vao[0]);
    glDrawArrays(GL_LINE_LOOP, 0, numVertices);
}

glm::vec4 screenToWorldSpace(double mousePos[], glm::mat4 projection, glm::mat4 viewMatrix)
{
    // from mouse to NDC
    double x = (2.0 * mousePos[0]) / SCR_WIDTH - 1.0;
    double y = 1.0 - (2.0 * mousePos[1]) / SCR_HEIGHT;
    // homogenouse clip coordinates
    glm::vec4 pos = glm::vec4(x, y, -1.0, 1.0);
    // from NDC to camera coordinates
    pos = glm::inverse(projection) * pos;
    // from camera to world coordinates
    pos = glm::inverse(viewMatrix) * pos;
    //glm::normalize(pos);
    return pos;
}

void interactWithMouse(Particle& p, glm::vec4 mousePos, bool mouseClick[])
{
    // get distance from mouse position
    double xDist = mousePos.x - p.pos[0];
    double yDist = mousePos.y - p.pos[1];
    double dist = sqrt(xDist * xDist + yDist * yDist);
    double dir[] = { xDist / dist, yDist / dist };
    double speed = 0.5;

    //std::cout << dist << std::endl;
    double infDist = 4.0;
    // repel 
    if (dist < infDist && mouseClick[0])
    {
        p.velocity[0] += (-dir[0] * speed);
        p.velocity[1] += (-dir[1] * speed);
    }

    // attract
    if (dist < infDist && mouseClick[1])
    {
        p.velocity[0] += (dir[0] * speed);
        p.velocity[1] += (dir[1] * speed);
    }
}

/*
1. mouse callback
2. transform mouse callback x and y to NDC
3. transform NDC to homogenouse clip coordinates
4. transform homogenous clip coordinates to eye coordinates
5. 
*/
