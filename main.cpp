#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <highgui.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <stb_image.h>

#include "Shader.h"
#include "LaneDetection/LaneDetectionModule.h"
#include "LaneDetection/LaneMark.h"


void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

unsigned int loadTexture(const char *path);

//void laneDetectionThread(cv::Mat frame) {
//    LaneDetectionModule lm;
//    lm.detectLane(frame);
//    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
//            std::chrono::system_clock::now());
//    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
//    std::time_t timestamp = tmp.count();
//    std::thread::id tid = std::this_thread::get_id();
//    std::cout << "f id=" << tid << ": " << timestamp << std::endl;
//}
cv::Mat camera_matrix, dist_coeffs;

void readCameraPara() {
    cv::FileStorage fs("../res/camera.yml", cv::FileStorage::READ);

    fs["camera_matrix"] >> camera_matrix;
    fs["distortion_coefficients"] >> dist_coeffs;

    std::cout << "camera_matrix\n"
              << camera_matrix << std::endl;
    std::cout << "\ndist coeffs\n"
              << dist_coeffs << std::endl;
}

int main() {
    cv::VideoCapture cap("../../videos/test1.mp4");
    if (!cap.isOpened()) {
        std::cout << "Failed to load camera" << std::endl;
        return -1;
    } else {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);//宽度
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);//宽度
    }
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "AR Lane Marker", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader cameraShader("../shaders/cameraVertex.glsl", "../shaders/cameraFragment.glsl");
    Shader arcShader("../shaders/arcVertex.glsl", "../shaders/arcFragment.glsl");
    Shader turnShader("../shaders/turnVertex.glsl", "../shaders/turnFragment.glsl");
    Shader panelSpeedShader("../shaders/panelVertex.glsl", "../shaders/panelFragment.glsl");
    Shader panelDegreeShader("../shaders/panelVertex.glsl", "../shaders/panelFragment.glsl");
    Shader pointerSpeedShader("../shaders/panelVertex.glsl", "../shaders/panelFragment.glsl");
    Shader pointerDegreeShader("../shaders/panelVertex.glsl", "../shaders/panelFragment.glsl");


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float camera_vertices[] = {
            // positions          // colors           // texture coords
            1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
            1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int camera_indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };
    unsigned int cameraVBO, cameraVAO, cameraEBO;
    glGenVertexArrays(1, &cameraVAO);
    glGenBuffers(1, &cameraVBO);
    glGenBuffers(1, &cameraEBO);

    glBindVertexArray(cameraVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cameraVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(camera_vertices), camera_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cameraEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(camera_indices), camera_indices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(camera_indices), camera_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    //enable depth test
    glEnable(GL_DEPTH_TEST);

    // set up arc vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float arc_vertices[] = {
            // positions
            0.0f, 0.0f, 0.0f,
            0.0f, 0.1f, 0.0f,
            0.5f, 0.0f, 0.0f,
            -0.5f, 0.0f, 0.0f,
            -0.5f, -0.1f, 0.0f,
            0.5f, -0.1f, 0.0f
    };
    unsigned int arc_indices[] = {
            0, 1, 3,
            0, 1, 2,
            0, 2, 5,
            0, 3, 4
    };
    unsigned int arc_VBO, arc_VAO, arc_EBO;
    glGenVertexArrays(1, &arc_VAO);
    glGenBuffers(1, &arc_VBO);
    glGenBuffers(1, &arc_EBO);

    glBindVertexArray(arc_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, arc_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arc_vertices), arc_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arc_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arc_indices), arc_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // set up turn mark vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float turn_vertices[] = {
            // positions
            0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f,
            1.f, -0.5f, 0.0f,
            -1.f, -0.5f, 0.0f,
    };
    unsigned int turn_indices[] = {
            0, 1, 3,
            0, 1, 2,
    };

    unsigned int turn_VBO, turn_VAO, turn_EBO;
    glGenVertexArrays(1, &turn_VAO);
    glGenBuffers(1, &turn_VBO);
    glGenBuffers(1, &turn_EBO);

    glBindVertexArray(turn_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, turn_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(turn_vertices), turn_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, turn_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(turn_indices), turn_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // set up panel vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float panel_vertices[] = {
            // positions
            1.f, 1.f, 0.0f, 1.0f, 1.0f, // top right
            1.f, -1.f, 0.0f, 1.0f, 0.0f, // bottom right
            -1.f, -1.f, 0.0f, 0.0f, 0.0f, // bottom left
            -1.f, 1.f, 0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int panel_indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };

    unsigned int panel_VBO, panel_VAO, panel_EBO;
    glGenVertexArrays(1, &panel_VAO);
    glGenBuffers(1, &panel_VBO);
    glGenBuffers(1, &panel_EBO);

    glBindVertexArray(panel_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, panel_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(panel_vertices), panel_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, panel_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(panel_indices), panel_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int panelSpeedTexture = loadTexture("../res/panel.png");
    panelSpeedShader.use();
    panelSpeedShader.setInt("texture1", 0);

    unsigned int pointerSpeedTexture = loadTexture("../res/pointer.png");
    pointerSpeedShader.use();
    pointerSpeedShader.setInt("texture1", 0);

    unsigned int panelDegreeTexture = loadTexture("../res/panel2.png");
    panelDegreeShader.use();
    panelDegreeShader.setInt("texture1", 0);

    unsigned int pointerDegreeTexture = loadTexture("../res/pointer2.png");
    pointerDegreeShader.use();
    pointerDegreeShader.setInt("texture1", 0);

    //Lane detection parameters
    int lanePointNum = 10;
    std::vector<int> medianPoints;
    LaneMark laneMark(lanePointNum);
    readCameraPara();
    laneMark.setCamMatrix(camera_matrix);
    laneMark.setDistCoeff(dist_coeffs);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D,
                      texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // load image, create texture and generate mipmaps
        cv::Mat frame;
        cap >> frame;
        cv::flip(frame, frame, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // render container
        cameraShader.use();

        glBindVertexArray(cameraVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glClear(GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        //Lane Detection
        //std::thread ldThread(laneDetectionThread, frame);
        //ldThread.join();
        LaneDetectionModule lm;
        lm.detectLane(frame, lanePointNum);
        // create transformations
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        // retrieve the matrix uniform locations

        if (lm.pointsLeftX.size() > 0 && lm.pointsRightX.size() > 0) {
            // lane arc render
            // activate shader
            arcShader.use();
            glBindVertexArray(arc_VAO);
            arcShader.setMat4("view", view);
            arcShader.setMat4("projection", projection);

            // lane arc
            medianPoints = lm.getMedianPoints(lm.pointsLeftX, lm.pointsRightX, lanePointNum);
            double headingDegree = lm.getHeadingDegree();
            for (unsigned int i = 0; i < lanePointNum; i++) {
                glm::vec3 transPos = glm::vec3(
                        0.0f + (2.0f * (float) medianPoints[i] / (float) SCR_WIDTH - 1.0f) * 2.f,
                        -1.0f,
                        -1.0f - 0.3f * i);
                glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
                model = glm::translate(model, transPos);
                model = glm::scale(model,
                                   glm::vec3(1.5f * ((float) (lanePointNum - i) / (float) lanePointNum), 1.f, 0.6f));
                model = glm::rotate(model, glm::radians(-80.0f), glm::vec3(1.0f, 0.0f, 0.0f));

                arcShader.setMat4("model", model);
                arcShader.setFloat("fragIter", (float) i);
                glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
            }

            //projection and view matrix
            std::vector<cv::Point2f> newPoints = {};
            newPoints.emplace_back(cv::Point2f(lm.pointsLeftX[lanePointNum - 2], lm.pointsLeftX[lanePointNum - 1]));
            newPoints.emplace_back(cv::Point2f(lm.pointsRightX[lanePointNum - 2], lm.pointsRightX[lanePointNum - 1]));
            newPoints.emplace_back(cv::Point2f(lm.pointsRightX[lanePointNum - 6], lm.pointsRightX[lanePointNum - 5]));
            newPoints.emplace_back(cv::Point2f(lm.pointsLeftX[lanePointNum - 6], lm.pointsLeftX[lanePointNum - 5]));
            laneMark.update(newPoints);
            cv::Mat viewM = laneMark.getViewMatrix();
            cv::Mat projM = laneMark.getProjectMatrix();
            memcpy(glm::value_ptr(view), viewM.data, 16 * sizeof(float));
            memcpy(glm::value_ptr(projection), projM.data, 16 * sizeof(float));

            // turn mark render
            // activate shader
            turnShader.use();
            glBindVertexArray(turn_VAO);
            turnShader.setMat4("view", view);
            turnShader.setMat4("projection", projection);
            // render container
            for (unsigned int j = 0; j < 5; j++) {
                glm::vec3 transPos = glm::vec3(0.0f + 2.0f * j,
                                               3.0f,
                                               -25.0f - 5.f * j);
                glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
                //model = glm::scale(model, glm::vec3(2.f , 1.f, 1.f));
                model = glm::translate(model, transPos);
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                //model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

                turnShader.setMat4("model", model);
                turnShader.setFloat("fragIter", (float) j);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }

            // speed render
            // bind Texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, panelSpeedTexture);
            panelSpeedShader.use();
            glBindVertexArray(panel_VAO);
            panelSpeedShader.setMat4("view", view);
            panelSpeedShader.setMat4("projection", projection);
            glm::vec3 transPos = glm::vec3(-1.5f,
                                           0.1f,
                                           -2.f);
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::scale(model, glm::vec3(3.f, 1.5f, 3.f));
            model = glm::translate(model, transPos);
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            panelSpeedShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // speed panel pointer render
            // bind Texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pointerSpeedTexture);
            pointerSpeedShader.use();
            glBindVertexArray(panel_VAO);
            pointerSpeedShader.setMat4("view", view);
            pointerSpeedShader.setMat4("projection", projection);
            transPos = glm::vec3(-1.49f,
                                 0.1f,
                                 -2.f);
            model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::scale(model, glm::vec3(3.f, 1.5f, 3.f));
            model = glm::translate(model, transPos);
            model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            pointerSpeedShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // degree panel render
            // bind Texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, panelDegreeTexture);
            panelDegreeShader.use();
            glBindVertexArray(panel_VAO);
            panelDegreeShader.setMat4("view", view);
            panelDegreeShader.setMat4("projection", projection);
            transPos = glm::vec3(1.5f,
                                 0.1f,
                                 -2.f);
            model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::scale(model, glm::vec3(3.f, 1.5f, 3.f));
            model = glm::translate(model, transPos);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            panelDegreeShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // degree panel pointer render
            // bind Texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pointerDegreeTexture);
            pointerDegreeShader.use();
            glBindVertexArray(panel_VAO);
            pointerDegreeShader.setMat4("view", view);
            pointerDegreeShader.setMat4("projection", projection);
            transPos = glm::vec3(1.49f,
                                 0.1f,
                                 -2.f);
            model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::scale(model, glm::vec3(3.f, 1.5f, 3.f));
            model = glm::translate(model, transPos);
            float turningDegree=(float) headingDegree * 90.0f / 40.0f;
            if(turningDegree>90.0f  ){
                turningDegree=90.0f;
            }else if(turningDegree<-90.0f){
                turningDegree=-90.0f;
            }
            model = glm::rotate(model, glm::radians(turningDegree),
                                glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            pointerDegreeShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)W
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cameraVAO);
    glDeleteBuffers(1, &cameraVBO);
    glDeleteBuffers(1, &cameraEBO);
    glDeleteVertexArrays(1, &arc_VAO);
    glDeleteBuffers(1, &arc_VBO);
    glDeleteBuffers(1, &arc_EBO);
    glDeleteVertexArrays(1, &turn_VAO);
    glDeleteBuffers(1, &turn_VBO);
    glDeleteBuffers(1, &turn_EBO);
    glDeleteVertexArrays(1, &panel_VAO);
    glDeleteBuffers(1, &panel_VBO);
    glDeleteBuffers(1, &panel_EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE
                                                                            : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}