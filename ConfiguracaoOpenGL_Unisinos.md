# Configuração do Ambiente de C++ com <img src="https://github.com/user-attachments/assets/db96047c-5e1a-4656-a3af-ee3432f9f35a" alt="OpenGL Logo" height="32" style="vertical-align: middle;" /> sem instalações

Para usar nas máquinas dos laboratórios da Unisinos.

## Passo 1 - Compilador C/C++

Verificar se existe este diretório no computador da sala:
- C:\msys64
  - C:\msys64\ucrt64\bin

Se NÃO houver, precisa instalar o msys2:

Link: https://github.com/msys2/msys2-installer/releases/download/2025-12-13/msys2-x86_64-20251213.exe 

Mesmo se diretório existir, abra o msys2 (Windows+R, digitar C:\msys64\ucrt64.exe e pressionar ENTER) e digitar no terminal:
```sh
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
```

- Primeira opção só dar ENTER
- Depois responder "yes" para tudo.

---

## Passo 2 - Baixar o CMAKE Portable no link abaixo:

Link: https://github.com/Kitware/CMake/releases/download/v4.0.2/cmake-4.0.2-windows-x86_64.zip

Descompactar (Sugere-se colocar em Documentos)

Ex:  C:\Users\NOME_DO_USUARIO\Documents\

---

## Passo 3 - Baixar Git portable no link abaixo:

Link: https://github.com/git-for-windows/git/releases/download/v2.49.0.windows.1/PortableGit-2.49.0-64-bit.7z.exe

Descompactar (clicar 2x)

---

## Passo 4 - Abrir o VSCode e instalar as seguintes extensões:

- C/C++
- C/C++ Extension Pack
- CMake Tools

---

## Passo 5 - Clonando o Repositório de Exemplo

1️⃣ **Clone o repositório** no diretório de sua escolha: https://github.com/guilhermechagaskurtz/OpenGLProject

2️⃣ Abra o VS Code e vá em `Arquivo -> Abrir Diretório` (ou `File -> Open Folder`). Selecione o diretório onde você clonou o repositório de exemplo.

<img src="https://github.com/user-attachments/assets/501ad8f1-30cc-44b0-bec0-8ef650a79998" alt="" style="width:30%; height:auto;" align="middle"/>

⚠️ É muito importante entrar no diretório do projeto pelo comando `Open Folder`. Caso contrário, o VS Code não reconhece o diretório `${workspaceFolder}`, o que poderá causar problemas no passo de compilação.

⚠️⚠️ EVITE SALVAR EM DIRETÓRIOS COM ACENTUAÇÃO, ESPAÇO OU CARACTERES ESPECIAIS. Muitos erros acontecem devido a isso.

---

## Passo 6 - Configurar o PATH do Windows

Com o projeto aberto no Visual Studio Code, crie (caso não exista) a pasta .vscode dentro dele.

Depois, crie o arquivo settings.json, e dentro dele adicione:
```
{
    "cmake.cmakePath": "C:/Users/NOME_DO_USUARIO/Documents/cmake-4.0.2-windows-x86_64/bin/cmake.exe",
    "cmake.environment": {
        "PATH": "C:\\msys64\\ucrt64\\bin;C:\\Users\\NOME_DO_USUARIO\\Documents\\cmake-4.0.2-windows-x86_64\\bin;C:\\Users\\NOME_DO_USUARIO\\Documents\\PortableGit\\bin;${env:PATH}"
    },
    "cmake.buildEnvironment": {
        "PATH": "C:\\msys64\\ucrt64\\bin;C:\\Users\\NOME_DO_USUARIO\\Documents\\cmake-4.0.2-windows-x86_64\\bin;C:\\Users\\NOME_DO_USUARIO\\Documents\\PortableGit\\bin;${env:PATH}"
    }
}
```

Para garantir que deu tudo certo, aperte F1 e execute o comando **CMake: Delete Cache and Reconfigure**

⚠️⚠️Obs: modifique o caminho para o SEU USUÁRIO!!!⚠️⚠️

---

## Passo 7 - Configurando o CMake no VS Code

1️⃣ No VS Code, pressione `Ctrl + Shift + P` e procure por `CMake: Scan for kit`

<img src="https://github.com/user-attachments/assets/93417f3a-6fc3-448c-bb73-1cc3a33ab199" alt="" style="width:40%; height:auto;" align="middle"/>

2️⃣ Logo após, pressione novamente `Ctrl + Shift + P` e procure por `CMake: Select a Kit`.  

3️⃣ **Selecione o compilador correto**:
   - Escolha `GCC for MSYS2 UCRT64` (ou similar, conforme sua instalação)
     
     <img src="https://github.com/user-attachments/assets/618655d3-253a-4d99-a6c0-e241c45a5c0c" alt="" style="width:40%; height:auto;" align="middle"/>

4️⃣ Este passo costuma acontecer automaticamente. Mas caso não ocorra nada após selecionar o kit, pressione `Ctrl + Shift + P` e execute `CMake: Configure`
   - Isso fará o **CMake detectar o compilador correto e preparar o projeto**. Espera-se que, após esta etapa, tenha-se uma saída similar a esta no terminal do VS Studio Code:
  
   <img src="https://github.com/user-attachments/assets/ca3bc7f1-5115-48d2-9f39-6a236e18f9b3" alt="" style="width:50%; height:auto;" align="middle"/>

---

## Passo 8 - Compilando e Executando o Projeto

Da mesma forma que o **CMake: Configure** pode executar automaticamente, a compilação pode ocorrer em sequência também de forma automática.
Caso não ocorra ou você pretenda compilar novamente:

1️⃣ Pressione `Ctrl + Shift + P` e execute `CMake: Build`
   - Ou rode manualmente no terminal:

   ```sh
   cd build
   cmake --build .
   ```

2️⃣ **Execute o programa**:
   ```sh
   ./Exemplo.exe
   ```

Se tudo estiver correto, o projeto será compilado e executado com sucesso! 🚀

⚠️ Erros nessa etapa:
- Evite salvar o projeto em uma pasta com nome muito grande, espaços, acentuação ou caracteres especiais.
- Apague a pasta build, tente compilar novamente.
- Tente rodar o "scan for kits" novamente.
- Tente tudo relacionado ao cmake novamente.
- Uma hora vai...
---

## Passo 9 - Adicionar DLLs no projeto (ANTES DE COMPILAR E EXECUTAR)

Caso os exemplos não rode (nada acontece), vá até a pasta **bin** de onde você descompactou o **msys2** (ou onde ele está instalado, por ex, C:\msys64\ucrt64\bin) e então copie as seguintes dlls para a pasta **build** do seu projeto:
- libstdc++-6.dll
- libgcc_s_seh-1.dll
- libwinpthread-1.dll

## 📌 Próximos Passos

Agora que seu ambiente está configurado, você pode:

✅ Adicionar os exemplos e exercícios dentro da pasta `src/`
```
📁 src/
│── Exemplo1.cpp
│── Exemplo2.cpp
│── Exemplo3.cpp
```

✅ Para cada exemplo/exercício novo, atualizar o arquivo `CMakelists.txt`
```cmake
set(EXERCISES 
    Exemplo1
    Exemplo2
    Exemplo3
)
```
✅ Atualmente, o `CMakelists.txt` já está configurado para compilar e gerar o excutável de cada código acrescentado no set EXERCISES. Se necessário, adicionar novas dependências
```cmake
foreach(EXERCISE ${EXERCISES})
    add_executable(${EXERCISE} src/${EXERCISE}.cpp Common/glad.c)
    target_link_libraries(${EXERCISE} glfw opengl32)
endforeach()
```
✅ Isso faz com que cada exercício gere seu próprio executável dentro da pasta build/.

✅ Portanto, se adicionar mais arquivos .cpp, basta incluir o nome na lista EXERCISES e rodar o CMake novamente.
