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
#include "primitiva.h"

GLFWwindow* Window = nullptr;
GLuint Shader_programm = 0;
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

void desenhaCasa(glm::vec3 posicao, float anguloRotacao = 0.0f, glm::vec3 escala= glm::vec3(1.0f)) {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");
    
    // Matriz base da casa (aplica a posição, rotação e escala global da casa)
    glm::mat4 matBase = glm::mat4(1.0f);
    matBase = glm::translate(matBase, posicao);
    matBase = glm::rotate(matBase, glm::radians(anguloRotacao), glm::vec3(0.0f, 1.0f, 0.0f));
    matBase = glm::scale(matBase, escala);
    
    glm::mat4 matCorpo = matBase;
    // Eleva 0.5 no eixo Y para a base do cubo encostar no chão (Y=0)
    matCorpo = glm::translate(matCorpo, glm::vec3(0.0f, 0.5f, 0.0f));
    matCorpo = glm::scale(matCorpo, glm::vec3(1.0f, 1.0f, 1.0f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matCorpo));
    glUniform3f(corLoc, 0.85f, 0.80f, 0.65f); // Cor bege/areia para as paredes
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glm::mat4 matTelhado = matBase;
    // Posiciona na altura do topo do cubo (Y=1.0)
    matTelhado = glm::translate(matTelhado, glm::vec3(0.0f, 1.0f, 0.0f));
    // Escala 0.8 para criar um beiral suave (0.8 * 1.5 = 1.2 de largura)
    matTelhado = glm::scale(matTelhado, glm::vec3(0.8f, 0.6f, 0.8f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTelhado));
    glUniform3f(corLoc, 0.80f, 0.25f, 0.15f); // Cor vermelho telha
    glBindVertexArray(VaoPiramide);
    glDrawArrays(GL_TRIANGLES, 0, 18);

    glm::mat4 matPorta = matBase;
    matPorta = glm::translate(matPorta, glm::vec3(0.0f, 0.25f, 0.51f));
    matPorta = glm::scale(matPorta, glm::vec3(0.25f, 0.5f, 0.05f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPorta));
    glUniform3f(corLoc, 0.35f, 0.18f, 0.05f); // Cor madeira escura
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void desenhaArvoreCubo(glm::vec3 posicao, float anguloRotacao = 0.0f, glm::vec3 escala= glm::vec3(1.0f)){
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");

    glm::mat4 matBase = glm::mat4(1.0f);
    matBase = glm::translate(matBase, posicao);
    matBase = glm::rotate(matBase, glm::radians(anguloRotacao), glm::vec3(0.0f,1.0f, 0.0f));
    matBase = glm::scale(matBase, escala);
    
    // tronco (Cilindro de Madeira)
    glm::mat4 matTronco = matBase;
    float alturaTronco = 1.2f;
    matTronco = glm::scale(matTronco, glm::vec3(0.25f, alturaTronco, 0.25f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTronco));
    glUniform3f(corLoc, 0.40f, 0.22f, 0.10f); // Marrom casca de árvore
    glBindVertexArray(VaoCilindro);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
    
        // topo(Cubo Verde de Folhas)
    glm::mat4 matCopa1 = matBase;
    matCopa1 = glm::translate(matCopa1, glm::vec3(0.0f, alturaTronco + 0.5f, 0.0f));
    matCopa1 = glm::scale(matCopa1, glm::vec3(1.4f, 1.5f, 1.4f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matCopa1));
    glUniform3f(corLoc, 0.15f, 0.55f, 0.15f); //verde escuro
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
void desenhaArvorePiramide(glm::vec3 posicao, float anguloRotacao = 0.0f, glm::vec3 escala= glm::vec3(1.0f)){
        GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");

    glm::mat4 matBase = glm::mat4(1.0f);
    matBase = glm::translate(matBase, posicao);
    matBase = glm::rotate(matBase, glm::radians(anguloRotacao), glm::vec3(0.0f,1.0f, 0.0f));
    matBase = glm::scale(matBase, escala);

    //tronco(Cilindro de Madeira)
    glm::mat4 matTronco = matBase;
    float alturaTronco = 1.5f;
    matTronco = glm::scale(matTronco, glm::vec3(0.25f, alturaTronco, 0.25f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTronco));
    glUniform3f(corLoc, 0.40f, 0.22f, 0.10f); // Marrom casca de árvore
    glBindVertexArray(VaoCilindro);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);

    //topo(piramide verde)
    glm::mat4 matFolhas1 = glm::translate(matBase, glm::vec3(0.0f, 0.6f, 0.0f));
    matFolhas1 = glm::scale(matFolhas1, glm::vec3(1.0f, 2.0f, 1.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matFolhas1));
    glUniform3f(corLoc, 0.10f, 0.45f, 0.15f);
    glBindVertexArray(VaoPiramide);
    glDrawArrays(GL_TRIANGLES, 0, 18);
}

void desenhaMoinho(glm::vec3 posicao, float anguloRotacao = 0.0f, glm::vec3 escala=glm::vec3(1.0f)) {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");
    
    // basw(Posiciona e orienta o moinho no mundo)
    glm::mat4 matBase = glm::mat4(1.0f);
    matBase = glm::translate(matBase, posicao);
    matBase = glm::rotate(matBase, glm::radians(anguloRotacao), glm::vec3(0.0f, 1.0f, 0.0f));
    matBase = glm::scale(matBase, escala);
    
    // torre (Cilindro Branco/Cinza Alto)
    float alturaTorre = 3.2f;
    glm::mat4 matTorre = matBase;
    // O cilindro original tem raio 0.75. Multiplicado por 1.0 fica com raio 0.75 e altura 3.2
    matTorre = glm::scale(matTorre, glm::vec3(1.0f, alturaTorre, 1.0f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTorre));
    glUniform3f(corLoc, 0.88f, 0.85f, 0.80f); // Cor pedra/reboco claro
    glBindVertexArray(VaoCilindro);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
    
    // teto (Pirâmide no Topo da Torre)
    glm::mat4 matTeto = matBase;
    matTeto = glm::translate(matTeto, glm::vec3(0.0f, alturaTorre, 0.0f));
    matTeto = glm::scale(matTeto, glm::vec3(1.2f, 1.0f, 1.2f)); // Beiral ligeiramente maior
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTeto));
    glUniform3f(corLoc, 0.55f, 0.25f, 0.15f); // Telha marrom/vermelha
    glBindVertexArray(VaoPiramide);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    
    // eixo (Ponto Central de Rotação das Pás)
    // Colocamos o eixo próximo ao topo da torre (Y = 2.6) e projetado para a frente (Z = 0.8)
    glm::mat4 matEixo = matBase;
    matEixo = glm::translate(matEixo, glm::vec3(0.0f, 2.6f, 0.8f));
    
    // Hub Central (Cilindro ou Cubo pequeno no miolo das pás)
    glm::mat4 matMiolo = glm::scale(matEixo, glm::vec3(0.2f, 0.2f, 0.2f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matMiolo));
    glUniform3f(corLoc, 0.25f, 0.15f, 0.05f); // Madeira escura
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    
    // 5. ANIMAÇÃO DAS PÁS (Giro contínuo no eixo Z)
    float anguloGiro = (float)glfwGetTime() * 50.0f;
    
    // Aplica a rotação contínua no eixo Z local do rotor
    glm::mat4 matRotorAnimado = glm::rotate(matEixo, glm::radians(anguloGiro), glm::vec3(0.0f,.0f, 1.0f));
    
    // Desenha as 4 pás espaçadas de 90 em 90 graus
    for (int i = 0; i < 4; i++) {
        glm::mat4 matPa = matRotorAnimado;
        // Gira cada uma das 4 pás na sua respectiva orientação (0°, 90°, 180°, 270°)
        matPa = glm::rotate(matPa, glm::radians(i * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        
        // Desloca o centro da pá para frente no eixo Y para ela se estender para fora a partirdo centro
        matPa = glm::translate(matPa, glm::vec3(0.0f, 0.9f, 0.0f));
            
        // Estica a pá: fina em X e Z, comprida em Y
        matPa = glm::scale(matPa, glm::vec3(0.18f, 1.5f, 0.03f));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPa));
            
        // Alterna tons sutis ou use uma cor de madeira/tecido nas pás
        glUniform3f(corLoc, 0.75f, 0.70f, 0.55f); // Tecido/Madeira clara
        glBindVertexArray(VaoCubo);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void desenhaFogueira(glm::vec3 posicao, float escalaGeral = 1.0f) {
        GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
        GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");
    
        // 1. Matriz Base da Fogueira
        glm::mat4 matBase = glm::mat4(1.0f);
        matBase = glm::translate(matBase, posicao);
        matBase = glm::scale(matBase, glm::vec3(escalaGeral));
    
        // ==========================================
        // 2. BRASAS / CINZAS (Base no Chão)
        // ==========================================
        glm::mat4 matBrasa = matBase;
        matBrasa = glm::scale(matBrasa, glm::vec3(0.6f, 0.05f, 0.6f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matBrasa));
        glUniform3f(corLoc, 0.12f, 0.10f, 0.10f); // Carvão quase preto
        glBindVertexArray(VaoCilindro);
        glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
    
        // ==========================================
        // 3. CÍRCULO DE PEDRAS AO REDOR
        // ==========================================
        int numPedras = 8;
        float raioCirculo = 0.55f;
        for (int i = 0; i < numPedras; i++) {
            float angulo = glm::radians((float)i * (360.0f / numPedras));
            float px = cos(angulo) * raioCirculo;
            float pz = sin(angulo) * raioCirculo;
    
            glm::mat4 matPedra = matBase;
            matPedra = glm::translate(matPedra, glm::vec3(px, 0.06f, pz));
            // Alterna tamanhos para parecer pedras naturais
            float varTamanho = (i % 2 == 0) ? 0.14f : 0.11f;
            matPedra = glm::scale(matPedra, glm::vec3(varTamanho, 0.12f, varTamanho));
    
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPedra));
            glUniform3f(corLoc, 0.45f, 0.45f, 0.45f); // Cinza pedra
            glBindVertexArray(VaoCubo);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    
        // ==========================================
        // 4. TORAS DE LENHA INCLINADAS
        // ==========================================
        int numToras = 4;
        for (int i = 0; i < numToras; i++) {
            glm::mat4 matTora = matBase;
            // Gira cada tora para uma direção
            matTora = glm::rotate(matTora, glm::radians((float)i * 90.0f + 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            // Inclina a tora em direção ao centro
            matTora = glm::rotate(matTora, glm::radians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            // Afina o cilindro e estica o comprimento
            matTora = glm::scale(matTora, glm::vec3(0.08f, 0.65f, 0.08f));
    
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTora));
            glUniform3f(corLoc, 0.30f, 0.15f, 0.05f); // Madeira escura
            glBindVertexArray(VaoCilindro);
            glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
        }
    
        // ==========================================
        // 5. CHAMAS DE FOGO (Animadas com Pulsação!)
        // ==========================================
        float tempo = (float)glfwGetTime();
        // Varia suavemente entre 0.85 e 1.15 usando a função seno
        float pulsacao1 = 0.95f + 0.15f * sin(tempo * 9.0f);
        float pulsacao2 = 0.90f + 0.15f * cos(tempo * 12.0f);
    
        // Chama Externa (Maior - Vermelho/Laranja)
        glm::mat4 matFogoExterno = matBase;
        matFogoExterno = glm::translate(matFogoExterno, glm::vec3(0.0f, 0.05f, 0.0f));
        matFogoExterno = glm::scale(matFogoExterno, glm::vec3(0.40f * pulsacao1, 0.70f * pulsacao1, 0.40f * pulsacao1));
    
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matFogoExterno));
        glUniform3f(corLoc, 0.95f, 0.30f, 0.05f); // Laranja avermelhado
        glBindVertexArray(VaoPiramide);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        // Chama Interna (Menor e mais rápida - Amarelo brilhante)
        glm::mat4 matFogoInterno = matBase;
        matFogoInterno = glm::translate(matFogoInterno, glm::vec3(0.0f, 0.08f, 0.0f));
        matFogoInterno = glm::rotate(matFogoInterno, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        matFogoInterno = glm::scale(matFogoInterno, glm::vec3(0.25f * pulsacao2, 0.50f * pulsacao2,0.25f * pulsacao2));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matFogoInterno));
        glUniform3f(corLoc, 1.0f, 0.85f, 0.10f); // Amarelo fogo
        glBindVertexArray(VaoPiramide);
        glDrawArrays(GL_TRIANGLES, 0, 18);
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

    // Chão
    glm::mat4 matPlano = glm::mat4(1.0f);
    matPlano = glm::scale(matPlano, glm::vec3(10.0f, 1.0f, 10.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPlano));
    glUniform3f(corLoc, 0.25f, 0.65f, 0.25f); // Verde grama uniforme
    glBindVertexArray(VaoPlano);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //Casas
    desenhaCasa(glm::vec3(0.0f, 0.0f, -2.5f), 0.0f, glm::vec3(2.0f));//1
    desenhaCasa(glm::vec3(-2.5f, 0.0f, -1.5f), 45.0f, glm::vec3(1.5f));//2
    desenhaCasa(glm::vec3(2.5f, 0.0f, -1.5f), -45.0f, glm::vec3(1.5f));//3
    desenhaCasa(glm::vec3(-3.5f, 0.0f, 0.5f), 90.0f, glm::vec3(1.3f));//4
    desenhaCasa(glm::vec3(3.5f, 0.0f, 0.5f), -90.0f, glm::vec3(1.3f));//5
    desenhaCasa(glm::vec3(-2.5f, 0.0f, 2.5f), 135.0f, glm::vec3(1.0f));//6
    desenhaCasa(glm::vec3(2.5f, 0.0f, 2.5f), -135.0f, glm::vec3(1.0f));//7

    //árvores piramide e cubo
    desenhaArvoreCubo(glm::vec3(-1.0f,0.0f,-5.5f), 0.0f, glm::vec3(1.0f));//8
    desenhaArvoreCubo(glm::vec3(1.0f,0.0f,-5.5f), 0.0f, glm::vec3(1.0f));//9
    desenhaArvorePiramide(glm::vec3(3.8f,0.0f, -4.2f), 45.0f, glm::vec3(1.5f));//10
    desenhaArvorePiramide(glm::vec3(-3.8f,0.0f, -4.2f), -45.0f, glm::vec3(1.5f));//11
    desenhaArvoreCubo(glm::vec3(5.0f,0.0f,-1.2f), 45.0f, glm::vec3(1.0f));//12
    desenhaArvoreCubo(glm::vec3(-5.0f,0.0f,-1.2f), -45.0f, glm::vec3(1.0f));//13
    desenhaArvorePiramide(glm::vec3(4.5f,0.0f, 3.2f), 75.0f, glm::vec3(1.5f));//14
    desenhaArvorePiramide(glm::vec3(-4.5f,0.0f, 3.2f), -75.0f, glm::vec3(1.5f));//15
    //Moinho cok animação
    desenhaMoinho(glm::vec3(6.0f, 0.0f, 6.0f), -90.0f, glm::vec3(1.2f));//16

    //fogueira com animação de fogo
    desenhaFogueira(glm::vec3(0.0f, 0.0f, 1.0f), 1.5f);//17

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