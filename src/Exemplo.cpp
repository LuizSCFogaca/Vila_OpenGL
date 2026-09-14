// Câmera

// Neste exemplo, especificamos uma câmera virtual através da aplicação de transformações de projeção e lookAt
#include <iostream>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLFWwindow* Window = nullptr;
GLuint Shader_programm = 0;
GLuint VaoPlano = 0;
GLuint VaoCubo = 0;
GLuint VaoPiramide = 0;
GLuint VaoCilindro = 0;
int NumVerticesCilindro = 0;
GLuint Vao = 0;

int WIDTH = 800;
int HEIGHT = 600;

float Tempo_entre_frames = 0.0f; // variavel utilizada para movimentar a camera

// Variáveis referentes a câmera virtual e sua projeção
float Cam_speed = 5.0f; // velocidade da camera aumentada um pouco para navegação livre
glm::vec3 Cam_pos = glm::vec3(0.0f, 0.0f, 2.0f); // posicao inicial da câmera
glm::vec3 Cam_front = glm::vec3(0.0f, 0.0f, -1.0f); // vetor para onde a câmera está olhando
glm::vec3 Cam_up = glm::vec3(0.0f, 1.0f, 0.0f); // vetor "para cima" global

float Cam_yaw = 0.0f; // ângulo de rotação da câmera (esquerda/direita)
float Cam_pitch = 0.0f; // ângulo de inclinação da câmera (cima/baixo)
float Cam_fov = 67.0f;

// Variáveis de controle do mouse
double lastX = WIDTH / 2.0;
double lastY = HEIGHT / 2.0;
bool primeiro_mouse = true;

void redimensionaCallback(GLFWwindow* window, int w, int h) {
    WIDTH = w;
    HEIGHT = h;
    glViewport(0, 0, WIDTH, HEIGHT);
}

// Callback responsável por ler a posição do mouse e girar a câmera
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (primeiro_mouse) {
        lastX = xpos;
        lastY = ypos;
        primeiro_mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Invertido, pois as coordenadas Y vão de cima para baixo
    
    lastX = xpos;
    lastY = ypos;

    float sensibilidade = 0.1f;
    xoffset *= sensibilidade;
    yoffset *= sensibilidade;

    Cam_yaw -= xoffset;
    Cam_pitch += yoffset;

    // Trava do pitch para evitar que a câmera dê uma cambalhota
    if (Cam_pitch > 89.0f) Cam_pitch = 89.0f;
    if (Cam_pitch < -89.0f) Cam_pitch = -89.0f;
}

void inicializaOpenGL() {
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar o GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    Window = glfwCreateWindow(WIDTH, HEIGHT, "Exemplo - Camera Livre com Mouse", NULL, NULL);
    if (!Window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowSizeCallback(Window, redimensionaCallback);
    
    // Registra a função do mouse e oculta o cursor na tela
    glfwSetCursorPosCallback(Window, mouse_callback);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwMakeContextCurrent(Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar o GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void inicializaPlano() {
    glGenVertexArrays(1, &VaoPlano);
    glBindVertexArray(VaoPlano);

    // VBO dos vértices do cubo
    float points[] = {
        //chão triangulo 1
        0.75f, 0.0f, 0.75f,
        -0.75f, 0.0f, 0.75f,
        -0.75f, 0.0f, -0.75f,
        //chão triangulo 2
         -0.75f, 0.0f, -0.75f,
         0.75f, 0.0f,-0.75f,
         0.75f, 0.0f, 0.75f,
    };
    
    GLuint pvbo;
    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // VBO das cores
    float cores[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };
    
    GLuint cvbo;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cores), cores, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void inicializaCubo(){
    glGenVertexArrays(1, &VaoCubo);
    glBindVertexArray(VaoCubo);

    // VBO dos vértices do cubo
    float points[] = {
        // face frontal
        0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        // face traseira
        0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
       -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        // face esquerda
       -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f,
        // face direita
        0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
        // face baixo
       -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f,
        // face cima
       -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f,
    };
    
    GLuint pvbo;
    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // VBO das cores
    float cores[] = {
        // face frontal - vermelha
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // face traseira - verde
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // face esquerda - azul
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        // face direita - ciano
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        // face baixo - magenta
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        // face cima - amarelo
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    };
    
    GLuint cvbo;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cores), cores, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

}

void inicializaPiramide(){
    glGenVertexArrays(1, &VaoPiramide);
    glBindVertexArray(VaoPiramide);

    // 18 vértices (Base + 4 paredes triangulares)
    float points[] = {
        // Base quadrada (2 triângulos, Y = 0.0f)
         0.75f, 0.0f,  0.75f,
        -0.75f, 0.0f,  0.75f,
        -0.75f, 0.0f, -0.75f,

        -0.75f, 0.0f, -0.75f,
         0.75f, 0.0f, -0.75f,
         0.75f, 0.0f,  0.75f,

        // Parede frontal (Z = +0.75)
        -0.75f, 0.0f,  0.75f,
         0.75f, 0.0f,  0.75f,
         0.0f,  1.0f,  0.0f,

        // Parede direita (X = +0.75)
         0.75f, 0.0f,  0.75f,
         0.75f, 0.0f, -0.75f,
         0.0f,  1.0f,  0.0f,

        // Parede traseira (Z = -0.75)
         0.75f, 0.0f, -0.75f,
        -0.75f, 0.0f, -0.75f,
         0.0f,  1.0f,  0.0f,

        // Parede esquerda (X = -0.75)
        -0.75f, 0.0f, -0.75f,
        -0.75f, 0.0f,  0.75f,
         0.0f,  1.0f,  0.0f
    };
    
    GLuint pvbo;
    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // VBO das cores (18 vértices com cores distintas por face)
    float cores[] = {
        // Base (cinza escuro)
        0.3f, 0.3f, 0.3f,  0.3f, 0.3f, 0.3f,  0.3f, 0.3f, 0.3f,
        0.3f, 0.3f, 0.3f,  0.3f, 0.3f, 0.3f,  0.3f, 0.3f, 0.3f,
        // Parede frontal (vermelho telha)
        0.9f, 0.2f, 0.2f,  0.9f, 0.2f, 0.2f,  0.9f, 0.2f, 0.2f,
        // Parede direita (laranja telha)
        0.9f, 0.5f, 0.2f,  0.9f, 0.5f, 0.2f,  0.9f, 0.5f, 0.2f,
        // Parede traseira (vermelho escuro)
        0.7f, 0.1f, 0.1f,  0.7f, 0.1f, 0.1f,  0.7f, 0.1f, 0.1f,
        // Parede esquerda (marrom claro)
        0.8f, 0.3f, 0.2f,  0.8f, 0.3f, 0.2f,  0.8f, 0.3f, 0.2f
    };
    
    GLuint cvbo;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cores), cores, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void inicializaCilindro() {
    glGenVertexArrays(1, &VaoCilindro);
    glBindVertexArray(VaoCilindro);

    const int segmentos = 20;
    const float raio = 0.75f;
    const float altura = 1.0f;
    const float pi = 3.14159265359f;
    const float passo = 2.0f * pi / (float)segmentos;

    std::vector<float> points;
    std::vector<float> cores;

    for (int i = 0; i < segmentos; i++) {
        float ang1 = (float)i * passo;
        float ang2 = (float)(i + 1) * passo;

        float x1 = raio * cos(ang1);
        float z1 = raio * sin(ang1);
        float x2 = raio * cos(ang2);
        float z2 = raio * sin(ang2);

        // 1. Tampa superior (Y = altura)
        points.push_back(0.0f); points.push_back(altura); points.push_back(0.0f);
        points.push_back(x1);   points.push_back(altura); points.push_back(z1);
        points.push_back(x2);   points.push_back(altura); points.push_back(z2);

        for (int k = 0; k < 3; k++) {
            cores.push_back(0.7f); cores.push_back(0.7f); cores.push_back(0.7f);
        }

        // 2. Tampa inferior (Y = 0.0f)
        points.push_back(0.0f); points.push_back(0.0f);   points.push_back(0.0f);
        points.push_back(x2);   points.push_back(0.0f);   points.push_back(z2);
        points.push_back(x1);   points.push_back(0.0f);   points.push_back(z1);

        for (int k = 0; k < 3; k++) {
            cores.push_back(0.4f); cores.push_back(0.4f); cores.push_back(0.4f);
        }

        // 3. Lateral do cilindro (2 triângulos)
        // Triângulo 1
        points.push_back(x1); points.push_back(0.0f);   points.push_back(z1);
        points.push_back(x2); points.push_back(0.0f);   points.push_back(z2);
        points.push_back(x1); points.push_back(altura); points.push_back(z1);

        // Triângulo 2
        points.push_back(x2); points.push_back(0.0f);   points.push_back(z2);
        points.push_back(x2); points.push_back(altura); points.push_back(z2);
        points.push_back(x1); points.push_back(altura); points.push_back(z1);

        // Cor lateral
        for (int k = 0; k < 6; k++) {
            cores.push_back(0.2f); cores.push_back(0.5f); cores.push_back(0.8f);
        }
    }

    NumVerticesCilindro = (int)(points.size() / 3);

    GLuint pvbo;
    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint cvbo;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, cores.size() * sizeof(float), cores.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void inicializaShaders() {
    const char* vertex_shader = 
        "#version 400\n"
        "layout(location = 0) in vec3 vertex_posicao;\n"
        "uniform mat4 matriz, view, proj;\n"
        "void main () {\n"
        "    gl_Position = proj * view * matriz * vec4(vertex_posicao, 1.0);\n"
        "}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    
    GLint success;
    char infoLog[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vs, 512, NULL, infoLog);
        std::cerr << "Erro no vertex shader:\n" << infoLog << std::endl;
    }

    const char* fragment_shader = 
        "#version 400\n"
        "uniform vec3 corObjeto;\n"
        "out vec4 frag_colour;\n"
        "void main () {\n"
        "    frag_colour = vec4(corObjeto, 1.0);\n"
        "}\n";

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fs, 512, NULL, infoLog);
        std::cerr << "Erro no fragment shader:\n" << infoLog << std::endl;
    }

    Shader_programm = glCreateProgram();
    glAttachShader(Shader_programm, vs);
    glAttachShader(Shader_programm, fs);
    glLinkProgram(Shader_programm);
    
    glGetProgramiv(Shader_programm, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(Shader_programm, 512, NULL, infoLog);
        std::cerr << "Erro na linkagem do shader:\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void atualizaDirecaoCamera() {
    // Recalcula o vetor de direção considerando tanto o Yaw quanto o Pitch
    glm::vec3 front;
    front.x = sin(glm::radians(-Cam_yaw)) * cos(glm::radians(Cam_pitch));
    front.y = sin(glm::radians(Cam_pitch)); 
    front.z = -cos(glm::radians(-Cam_yaw)) * cos(glm::radians(Cam_pitch));
    Cam_front = glm::normalize(front);
}

void trataTeclado() {
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(Window, true);
    }

    // Calcula o vetor "Direita" da câmera usando o Cross product
    glm::vec3 Cam_right = glm::normalize(glm::cross(Cam_front, Cam_up));

    // A/D - Movimento lateral (Strafe)
    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS) {
        Cam_pos -= Cam_right * Cam_speed * Tempo_entre_frames;
    }
    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS) {
        Cam_pos += Cam_right * Cam_speed * Tempo_entre_frames;
    }

    // W/S - Movimento para frente e para trás
    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS) {
        Cam_pos += Cam_front * Cam_speed * Tempo_entre_frames;
    }
    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS) {
        Cam_pos -= Cam_front * Cam_speed * Tempo_entre_frames;
    }

    // Subir / Descer estritamente no eixo Y global (Q e E agora fazem isso, já que o mouse cuida da rotação)
    if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS) {
        Cam_pos.y += Cam_speed * Tempo_entre_frames;
    }
    if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS) {
        Cam_pos.y -= Cam_speed * Tempo_entre_frames;
    }

    if (glfwGetKey(Window, GLFW_KEY_Z) == GLFW_PRESS || glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        Cam_fov = 10.0f;
    } else {
        Cam_fov = 67.0f;
    }
}

void desenhaCena() {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");

    // --- 1. PLANO (Chão verde) ---
    glm::mat4 matPlano = glm::mat4(1.0f);
    matPlano = glm::scale(matPlano, glm::vec3(8.0f, 1.0f, 8.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPlano));
    glUniform3f(corLoc, 0.25f, 0.65f, 0.25f); // Verde grama uniforme
    glBindVertexArray(VaoPlano);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // --- 2. CUBO (Marrom) ---
    glm::mat4 matCubo = glm::mat4(1.0f);
    matCubo = glm::translate(matCubo, glm::vec3(-2.5f, 0.5f, 0.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matCubo));
    glUniform3f(corLoc, 0.60f, 0.35f, 0.15f); // Marrom uniforme
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // --- 3. PIRÂMIDE (Vermelho telha) ---
    glm::mat4 matPiramide = glm::mat4(1.0f);
    matPiramide = glm::translate(matPiramide, glm::vec3(0.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPiramide));
    glUniform3f(corLoc, 0.85f, 0.30f, 0.15f); // Laranja/Vermelho telha uniforme
    glBindVertexArray(VaoPiramide);
    glDrawArrays(GL_TRIANGLES, 0, 18);

    // --- 4. CILINDRO (Azul) ---
    glm::mat4 matCilindro = glm::mat4(1.0f);
    matCilindro = glm::translate(matCilindro, glm::vec3(2.5f, 0.0f, 0.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matCilindro));
    glUniform3f(corLoc, 0.20f, 0.55f, 0.85f); // Azul uniforme
    glBindVertexArray(VaoCilindro);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
}

void inicializaRenderizacao() {
    double tempo_anterior = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(Window)) {
        double tempo_frame_atual = glfwGetTime();
        Tempo_entre_frames = (float)(tempo_frame_atual - tempo_anterior);
        tempo_anterior = tempo_frame_atual;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(Shader_programm);
        
        trataTeclado();
        atualizaDirecaoCamera();

        GLint viewLoc = glGetUniformLocation(Shader_programm, "view");
        GLint projLoc = glGetUniformLocation(Shader_programm, "proj");

        glViewport(0, 0, WIDTH, HEIGHT);

        glm::mat4 view_fps = glm::lookAt(Cam_pos, Cam_pos + Cam_front, Cam_up);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_fps));

        float aspecto_tela = (float)WIDTH / (float)HEIGHT;
        glm::mat4 proj_persp = glm::perspective(glm::radians(Cam_fov), aspecto_tela, 0.1f, 100.0f);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj_persp));

        desenhaCena();

        glClear(GL_DEPTH_BUFFER_BIT);

        int miniW = 200;
        int miniH = 200;
        int miniX = WIDTH - miniW - 20;
        int miniY = HEIGHT - miniH - 20;
        glViewport(miniX, miniY, miniW, miniH);

        glm::vec3 topo_pos = glm::vec3(0.0f, 5.0f, 0.0f);
        glm::vec3 topo_alvo = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 topo_up = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::mat4 view_mini = glm::lookAt(topo_pos, topo_alvo, topo_up);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_mini));

        float ortho_size = 2.0f;
        glm::mat4 proj_ortho = glm::ortho(-ortho_size, ortho_size, -ortho_size, ortho_size, 0.1f, 20.0f);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj_ortho));

        desenhaCena();

        glfwPollEvents();
        glfwSwapBuffers(Window);
    }
    
    glfwTerminate();
}

int main() {
    inicializaOpenGL();
    inicializaPlano();
    inicializaCubo();
    inicializaPiramide();
    inicializaCilindro();
    inicializaShaders();
    inicializaRenderizacao();

    return 0;
}