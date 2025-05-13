#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iomanip>
#include <list>
#include <vector>
#include <map>
#include <utility>
#include <fstream>     // Para ofstream y ifstream
#include <sstream>     // Para stringstream

using namespace std;

// Estructura para almacenar información del usuario
struct Usuario {
    string username;
    string password;
    list<string> games;  // Biblioteca de juegos del usuario
    
    // Constructor
    Usuario(string user = "", string pass = "") : username(user), password(pass) {}
};

//Nombre del jugador
string playerName;

// Vector para almacenar múltiples usuarios
vector<Usuario> usuarios;
int usuarioActualIndex = -1;  // -1 significa que no hay usuario logueado

// Códigos de colores ANSI
#define RESET "\033[0m"       // Resetear color
#define RED "\033[31m"        // Rojo
#define GREEN "\033[32m"      // Verde
#define YELLOW "\033[33m"     // Amarillo
#define BLUE "\033[34m"       // Azul

// Función para mostrar el menú con colores
void mostrarMenu() {
    cout << BLUE << "==========================" << RESET << endl;
    cout << YELLOW << "       MENU DEL JUEGO     " << RESET << endl;
    cout << BLUE << "==========================" << RESET << endl;
    cout << GREEN << "1. Registrarse" << RESET << endl;
    cout << YELLOW << "2. Loguearse" << RESET << endl;
    cout << BLUE << "3. Tienda de Juegos" << RESET << endl;
    cout << GREEN << "4. Biblioteca de Juegos" << RESET << endl;
    cout << YELLOW << "5. Ver Usuarios Registrados" << RESET << endl;
    cout << RED << "6. Cerrar Sesión" << RESET << endl;
    cout << RED << "7. Eliminar mi cuenta" << RESET << endl;
    cout << RED << "8. Salir" << RESET << endl;
    cout << BLUE << "==========================" << RESET << endl;
    
    // Muestra el usuario actual si está logueado
    if (usuarioActualIndex != -1) {
        cout << GREEN << "Usuario actual: " << usuarios[usuarioActualIndex].username << RESET << endl;
    } else {
        cout << YELLOW << "No hay usuario logueado" << RESET << endl;
    }
    
    cout << "Seleccione una opción: ";
}

//Función para guardar usuarios en archivo de texto
void guardarUsuariosEnArchivo() {
    ofstream archivo("usuarios.txt");

    if (!archivo) {
        cerr << "Error al abrir el archivo para guardar usuarios." << endl;
        return;
    }

    for (const auto& user : usuarios) {
        archivo << user.username << ";" << user.password << ";";

        // Guardar juegos separados por comas
        for (auto it = user.games.begin(); it != user.games.end(); ++it) {
            archivo << *it;
            if (next(it) != user.games.end()) archivo << ",";
        }

        archivo << endl;
    }

    archivo.close();
}

//Función para cargar usuarios en archivo de texto
void cargarUsuariosDesdeArchivo() {
    ifstream archivo("usuarios.txt");

    if (!archivo) {
        // El archivo aún no existe, no hay usuarios
        return;
    }

    usuarios.clear();
    string linea;

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string username, password, juegosStr;

        getline(ss, username, ';');
        getline(ss, password, ';');
        getline(ss, juegosStr);

        Usuario user(username, password);

        stringstream juegosStream(juegosStr);
        string juego;

        while (getline(juegosStream, juego, ',')) {
            if (!juego.empty()) {
                user.games.push_back(juego);
            }
        }

        usuarios.push_back(user);
    }

    archivo.close();
}

//Función para eliminar la cuenta actual
void eliminarCuentaActual() {
    system("cls");

    if (usuarioActualIndex == -1) {
        cout << YELLOW << "No hay ninguna sesión activa." << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        system("cls");
        return;
    }

    string nombre = usuarios[usuarioActualIndex].username;

    cout << RED << "Estás a punto de eliminar tu cuenta: " << nombre << RESET << endl;
    cout << "Esta acción no se puede deshacer." << endl;
    cout << "¿Estás seguro de que deseas continuar? (s/n): ";

    char confirmacion;
    cin >> confirmacion;

    if (confirmacion == 's' || confirmacion == 'S') {
        usuarios.erase(usuarios.begin() + usuarioActualIndex);
        usuarioActualIndex = -1;
        guardarUsuariosEnArchivo();

        cout << GREEN << "La cuenta ha sido eliminada exitosamente." << RESET << endl;
    } else {
        cout << YELLOW << "Operación cancelada. Tu cuenta no fue eliminada." << RESET << endl;
    }

    cout << "Presione cualquier tecla para continuar...";
    getch();
    system("cls");
}


// Función para registrar un nuevo usuario
void PageRegister() {
    system("cls");

    // Si ya hay un usuario logueado, no se permite registrar otro
    if (usuarioActualIndex != -1) {
        cout << YELLOW << "Ya hay un usuario logueado: " << usuarios[usuarioActualIndex].username << RESET << endl;
        cout << "Debe cerrar sesión antes de registrar un nuevo usuario." << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        system("cls");
        return;
    }

    string nuevoUsername, nuevoPassword;

    cout << BLUE << "Registro de Usuario" << RESET << endl;
    cout << BLUE << "====================" << RESET << endl;
    cout << "Ingrese un nombre de usuario: ";
    cin >> nuevoUsername;

    // Verificar si el usuario ya existe
    for (const auto& user : usuarios) {
        if (user.username == nuevoUsername) {
            cout << RED << "\nEl nombre de usuario ya está en uso. Intente con otro." << RESET << endl;
            cout << "Presione cualquier tecla para continuar...";
            getch();
            system("cls");
            return;
        }
    }

    cout << "Ingrese una contraseña: ";
    cin >> nuevoPassword;

    // Crear y añadir el nuevo usuario
    Usuario nuevoUsuario(nuevoUsername, nuevoPassword);
    usuarios.push_back(nuevoUsuario);
    usuarioActualIndex = -1;

    cout << GREEN << "\nRegistro exitoso. ¡Bienvenido, " << nuevoUsername << "!" << RESET << endl;
    cout << "Ahora puedes iniciar sesión." << endl;
    cout << BLUE << "====================" << RESET << endl;
    cout << "Presione cualquier tecla para continuar...";
    getch();
    system("cls");
}


// Función para ver la lista de usuarios registrados
void verUsuariosRegistrados() {
    system("cls");
    cout << BLUE << "Usuarios Registrados" << RESET << endl;
    cout << BLUE << "===================" << RESET << endl;
    
    if (usuarios.empty()) {
        cout << YELLOW << "No hay usuarios registrados." << RESET << endl;
    } else {
        cout << "Total de usuarios: " << usuarios.size() << endl << endl;
        for (size_t i = 0; i < usuarios.size(); i++) {
            cout << i + 1 << ". " << usuarios[i].username;
            if (static_cast<int>(i) == usuarioActualIndex) {
                cout << " " << GREEN << "(Actual)" << RESET;
            }
            cout << endl;
        }
    }
    
    cout << "\nPresione cualquier tecla para continuar...";
    getch();
    system("cls");
}

void PageLogin() {
    system("cls");

    // Si ya hay un usuario logueado, no se permite iniciar sesión de nuevo
    if (usuarioActualIndex != -1) {
        cout << YELLOW << "Ya hay un usuario logueado: " << usuarios[usuarioActualIndex].username << RESET << endl;
        cout << "Debe cerrar sesión antes de iniciar sesión con otra cuenta." << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        system("cls");
        return;
    }

    string userlog, passlog;

    cout << BLUE << "Inicio de Sesión" << RESET << endl;
    cout << BLUE << "=================" << RESET << endl;

    if (usuarios.empty()) {
        cout << RED << "No hay usuarios registrados. Por favor, regístrese primero." << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        system("cls");
        return;
    }

    cout << "Ingrese su nombre de usuario: ";
    cin >> userlog;

    bool usuarioEncontrado = false;
    for (size_t i = 0; i < usuarios.size(); i++) {
        if (usuarios[i].username == userlog) {
            usuarioEncontrado = true;
            cout << "Ingrese su contraseña: ";
            cin >> passlog;

            if (usuarios[i].password == passlog) {
                usuarioActualIndex = static_cast<int>(i); // El usuario está logueado
                cout << GREEN << "\nInicio de sesión exitoso. ¡Bienvenido, " << usuarios[i].username << "!" << RESET << endl;
            } else {
                cout << RED << "\nContraseña incorrecta. Intente nuevamente." << RESET << endl;
            }
            break;
        }
    }

    if (!usuarioEncontrado) {
        cout << RED << "\nUsuario no encontrado. Por favor, regístrese primero." << RESET << endl;
    }

    cout << "Presione cualquier tecla para continuar...";
    getch();
    system("cls");
}

void cerrarSesion() {
    system("cls");
    
    if (usuarioActualIndex == -1) {
        cout << YELLOW << "No hay usuario logueado actualmente." << RESET << endl;
    } else {
        string nombreUsuario = usuarios[usuarioActualIndex].username;
        usuarioActualIndex = -1;
        cout << GREEN << "Sesión de " << nombreUsuario << " cerrada exitosamente." << RESET << endl;
    }
    
    cout << "Presione cualquier tecla para continuar...";
    getch();
    system("cls");
}
/*
    Juego de Cuatro en Raya
*/
// Definición de constantes
const int FILAS = 6;
const int COLUMNAS = 7;
char tablero[FILAS][COLUMNAS];

// Función para inicializar el tablero
void inicializarTablero4EnRaya() {
    for (int i = 0; i < FILAS; ++i)
        for (int j = 0; j < COLUMNAS; ++j)
            tablero[i][j] = ' ';
}

// Función para imprimir el tablero
void imprimirTablero4enRaya() {
    system("cls"); // En Windows usa "cls"
    for (int i = 0; i < FILAS; ++i) {
        cout << "|";
        for (int j = 0; j < COLUMNAS; ++j)
            cout << tablero[i][j] << "|";
        cout << "\n";
    }
    cout << " ";
    for (int j = 0; j < COLUMNAS; ++j)
        cout << j << " ";
    cout << endl;
}

// Función para insertar una ficha en el tablero
bool insertarFicha(int columna, char ficha) {
    if (columna < 0 || columna >= COLUMNAS)
        return false;
    for (int i = FILAS - 1; i >= 0; --i) {
        if (tablero[i][columna] == ' ') {
            tablero[i][columna] = ficha;
            return true;
        }
    }
    return false;
}

// Función para verificar si hay un ganador
bool hayGanador(char ficha) {
    // Horizontal
    for (int i = 0; i < FILAS; ++i)
        for (int j = 0; j <= COLUMNAS - 4; ++j)
            if (tablero[i][j] == ficha && tablero[i][j+1] == ficha &&
                tablero[i][j+2] == ficha && tablero[i][j+3] == ficha)
                return true;

    // Vertical
    for (int j = 0; j < COLUMNAS; ++j)
        for (int i = 0; i <= FILAS - 4; ++i)
            if (tablero[i][j] == ficha && tablero[i+1][j] == ficha &&
                tablero[i+2][j] == ficha && tablero[i+3][j] == ficha)
                return true;

    // Diagonal ?
    for (int i = 0; i <= FILAS - 4; ++i)
        for (int j = 0; j <= COLUMNAS - 4; ++j)
            if (tablero[i][j] == ficha && tablero[i+1][j+1] == ficha &&
                tablero[i+2][j+2] == ficha && tablero[i+3][j+3] == ficha)
                return true;

    // Diagonal ?
    for (int i = 3; i < FILAS; ++i)
        for (int j = 0; j <= COLUMNAS - 4; ++j)
            if (tablero[i][j] == ficha && tablero[i-1][j+1] == ficha &&
                tablero[i-2][j+2] == ficha && tablero[i-3][j+3] == ficha)
                return true;

    return false;
}

bool tableroLleno() {
    for (int j = 0; j < COLUMNAS; ++j)
        if (tablero[0][j] == ' ')
            return false;
    return true;
}

// Función para la computadora, SOLO elige una columna aleatoria
int elegirColumnaComputadora() {
    srand(time(0));  // Inicializa la semilla aleatoria
    int columna;
    do {
        columna = rand() % COLUMNAS;  // Elige una columna aleatoria
        // Verifica si la columna está disponible
        bool columnaValida = false;
        for (int i = 0; i < FILAS; ++i) {
            if (tablero[i][columna] == ' ') {
                columnaValida = true;
                break;
            }
        }
        if (columnaValida) break;
    } while (true);
    return columna;  // Solo devuelve la columna, NO inserta la ficha
}

// Función para jugar Cuatro en Raya
void playCuatroEnRaya() {
    system("cls");
    inicializarTablero4EnRaya();
    char jugador = 'X';
    bool juegoTerminado = false;
    playerName = usuarios[usuarioActualIndex].username;

    while (!juegoTerminado) {
        imprimirTablero4enRaya();
        int columna;
        if (jugador == 'X') {
            cout << "Turno del Jugador (" << playerName <<"). Elija una columna (0-6): ";
            cin >> columna;
        } else {
            columna = elegirColumnaComputadora();
            cout << "Turno de la Computadora (O). Columna elegida: " << columna << endl;
        }

        if (insertarFicha(columna, jugador)) {
            if (hayGanador(jugador)) {
                imprimirTablero4enRaya();
                cout << "¡El jugador " << jugador << " ha ganado!" << endl;
                juegoTerminado = true;
            } else if (tableroLleno()) {
                imprimirTablero4enRaya();
                cout << "¡Es un empate!" << endl;
                juegoTerminado = true;
            }
            jugador = (jugador == 'X') ? 'O' : 'X';  // Cambia de turno
        } else {
            cout << "Columna llena. Intente nuevamente." << endl;
        }
    }
    cout << "Presione cualquier tecla para continuar...";
    getch();
    system("cls");
}
/*
    Juego de Snake
*/

// Dimensiones del tablero
const int WIDTH = 20;
const int HEIGHT = 20;

// Posición de la fruta
int fruitX, fruitY;
//Record de puntaje en snake
int recordsnake;

// Función para generar una nueva fruta en una posición aleatoria
void GenerateFruit() {
    fruitX = rand() % (WIDTH - 2) + 1;
    fruitY = rand() % (HEIGHT - 2) + 1;
}

// Función para dibujar la serpiente y la fruta
void DrawBoard(const vector<pair<int, int>>& snake, int score) {
    system("cls");

    // Dibuja el borde superior
    for (int i = 0; i < WIDTH; i++) cout << "#";
    cout << endl;

    // Dibuja los lados y el contenido
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH - 1) {
                cout << "#"; // Paredes
            } else if (x == fruitX && y == fruitY) {
                cout << "F"; // Fruta
            } else {
                bool isBody = false;
                // Dibuja la serpiente
                for (auto segment : snake) {
                    if (segment.first == x && segment.second == y) {
                        // La cabeza es "O" y el cuerpo "o"
                        cout << (segment == snake[0] ? "O" : "o");
                        isBody = true;
                        break;
                    }
                }
                if (!isBody) cout << " "; // Espacio vacío
            }
        }
        cout << endl;
    }

    // Dibuja el borde inferior
    for (int i = 0; i < WIDTH; i++) cout << "#";
    cout << endl;

    // Muestra la información del jugador
    cout << "Jugador: " << playerName << " | Puntaje: " << score << endl;
}

// Función para mostrar el menú principal
void ShowMenuSnake() {
    cout << "\n\n";
    cout << "======================================\n";
    cout << "           JUEGO DE SNAKE             \n";
    cout << "======================================\n\n";
    
    cout << "Bienvenido al juego de Snake!\n\n";
    
    // Si hay un usuario logueado, usamos su nombre
    if (usuarioActualIndex != -1) {
        playerName = usuarios[usuarioActualIndex].username;
        cout << "Jugador: " << playerName << "\n\n";
    } else {
        cout << "Por favor, ingresa tu nombre: ";
        cin >> playerName;
        cout << "\n";
    }
    
    cout << "Hola " << playerName << "! Estas son las reglas del juego:\n\n";
    cout << "1. Controlarás una serpiente que se mueve por el tablero.\n";
    cout << "2. Usa las teclas W, A, S, D para moverte (arriba, izquierda, abajo, derecha).\n";
    cout << "3. Debes comer las frutas (representadas por 'F') para crecer.\n";
    cout << "4. Si chocas contra los bordes o contra ti mismo, pierdes.\n";
    cout << "5. El objetivo es hacer que la serpiente crezca lo más posible.\n\n";
    cout<< "\n"<<endl;
    cout<< "\nResumen de estadisticas de: "<<playerName<<" en Snake"<<endl;
    cout<<"Record actual obtenido : "<<recordsnake<<endl;
    cout << "Presiona cualquier tecla para comenzar...";
    getch(); 
}

// Función principal del juego
void game() {
    const int DELAY = 150;
    int dx = 0, dy = 0;  // Dirección inicial: detenida
    int score = 0;
    bool paused = false; 

    // Inicializa la serpiente en el centro del tablero
    vector<pair<int, int>> snake = {{WIDTH / 2, HEIGHT / 2}};
    GenerateFruit();  // Genera la primera fruta

    bool gameRunning = true;

    while (gameRunning) {
        // Si el juego está pausado, solo revisamos si el usuario quiere despausar
        if (paused) {
            if (_kbhit()) {
                char key = _getch();
                if (key == 'p') {
                    paused = false;
                    DrawBoard(snake, score); // Redibuja el tablero al despausar
                } else if (key == 'q') {
                    gameRunning = false;
                }
            }
            Sleep(100); // Reducir el uso de CPU mientras está pausado
            continue;   // Salta el resto del ciclo mientras está pausado
        }
        
        // Entrada de teclado
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'w': dy = -1; dx = 0; break;
                case 's': dy = 1; dx = 0; break;
                case 'a': dx = -1; dy = 0; break;
                case 'd': dx = 1; dy = 0; break;
                case 'q': gameRunning = false; break;
                case 'p':  // Pausar el juego
                    paused = true;
                    system("cls");
                    cout << "Juego Pausado! Presiona 'P' para continuar o 'Q' para salir..." << endl;
                    continue; // Salta al siguiente ciclo para procesar la pausa
            }
        }

        // Calcula la nueva posición de la cabeza
        int newHeadX = snake[0].first + dx;
        int newHeadY = snake[0].second + dy;

        // Verifica colisión los bordes
        if (newHeadX <= 0 || newHeadX >= WIDTH - 1 || newHeadY <= 0 || newHeadY >= HEIGHT - 1) {
            break; // Fin del juego
        }

        // Verifica colisión consigo misma
        if (dx != 0 || dy != 0) {
            for (size_t i = 1; i < snake.size(); i++) {
                if (snake[i].first == newHeadX && snake[i].second == newHeadY) {
                    gameRunning = false;
                    break;
                }
            }
        }

        // Verifica si comió la fruta
        bool grow = false;
        if (newHeadX == fruitX && newHeadY == fruitY) {
            score += 10;
            
            GenerateFruit();  // Nueva fruta
            grow = true;
        }

        // Mueve la serpiente
        snake.insert(snake.begin(), {newHeadX, newHeadY}); // Añade la nueva cabeza
        if (!grow) {
            snake.pop_back();  // Si no comió, elimina la cola
        }

        // Dibuja el tablero actualizado
        DrawBoard(snake, score);

        // Control de velocidad del juego
        Sleep(DELAY);
    }

    cout << "\n¡Juego terminado! Puntaje final: " << score << endl;
    if (score>recordsnake)
    {
        recordsnake=score;
        cout<<"\n¡¡¡Enhorabuena!!!, nuevo record obtenido..."<<endl;
            cout<<"\nRecord actual : "<<recordsnake<<endl;
    } else {
        cout<<"\nRecord actual : "<<recordsnake<<endl;
    }
    getch();
}

void playSnake() {
    system("cls");
    ShowMenuSnake();
    game();
    cout << "Gracias por jugar, " << playerName << "!" << endl;
    cout << "Presiona cualquier tecla para salir...";
    getch();
    system("cls");
}

/*
    Juego de Sudoku
*/

void imprimirTablero(const int tablero[9][9]) {
    for (int i = 0; i < 9; i++) {
        if (i % 3 == 0 && i != 0) {
            cout << "---------------------" << endl;
        }
        for (int j = 0; j < 9; j++) {
            if (j % 3 == 0 && j != 0) {
                cout << "| ";
            }
            cout << tablero[i][j] << " ";
        }
        cout << endl;
    }
}

bool esValido(const int tablero[9][9], int fila, int columna, int num) {
   
    for (int i = 0; i < 9; i++) {
        if (tablero[fila][i] == num) {
            return false;
        }
    }
  
    for (int i = 0; i < 9; i++) {
        if (tablero[i][columna] == num) {
            return false;
        }
    }
 
    int inicioFila = (fila / 3) * 3;
    int inicioColumna = (columna / 3) * 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tablero[inicioFila + i][inicioColumna + j] == num) {
                return false;
            }
        }
    }
    return true;
}

bool estaCompleto(const int tablero[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (tablero[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

void playSudoku() {
    system("cls");
    int tablero[9][9] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    cout << BLUE << "Bienvenido al juego de Sudoku!" << RESET << endl;

    while (!estaCompleto(tablero)) {
        imprimirTablero(tablero);

        int fila, columna, num;
        cout << "Ingrese la fila (0-8): ";
        cin >> fila;
        cout << "Ingrese la columna (0-8): ";
        cin >> columna;
        cout << "Ingrese el numero (1-9): ";
        cin >> num;

        if (fila < 0 || fila >= 9 || columna < 0 || columna >= 9 || num < 1 || num > 9) {
            cout << RED << "Entrada inválida. Intente de nuevo." << RESET << endl;
            continue;
        }

        if (tablero[fila][columna] != 0) {
            cout << RED << "La celda ya tiene un número. Intente de nuevo." << RESET << endl;
            continue;
        }

        if (esValido(tablero, fila, columna, num)) {
            tablero[fila][columna] = num;
        } else {
            cout << RED << "Movimiento inválido. Intente de nuevo." << RESET << endl;
        }
    }

    cout << GREEN << "¡FELICIDADES COMPLETASTE EL SUDOKU!" << RESET << endl;
    imprimirTablero(tablero);
    cout << "Presiona cualquier tecla para salir...";
    getch();
    system("cls");
}

// Funcion para la tienda de juegos
void PageStore() {
    if (usuarioActualIndex == -1) {
        system("cls");
        cout << RED << "Primero debe iniciar sesión para acceder a la tienda." << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        return;
    }
    
    int opcion;
    list<string> carrito;
    bool salirTienda = true;
    while (salirTienda == true) {
        system("cls");
        cout << BLUE << "Tienda de Juegos" << RESET << endl;
        cout << BLUE << "=================" << RESET << endl;
        cout << "1. Juego de Snake  (20$)" << endl;
        cout << "2. Juego de Sudoku  (20$)"  << endl;
        cout << "3. Juego de Cuatro en Raya  (20$)" << endl;
        cout << "4. Ver el carrito" << endl;
        cout << "5. Pagar el carrito" << endl;
        cout << "6. Volver al menú principal" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;
        
        switch (opcion) {
            case 1:
                cout << GREEN << "Juego de Snake Seleccionado." << RESET << endl;
                cout << "Agregado al carrito." << endl;
                carrito.push_back("Juego de Snake");
                break;
            case 2:
                cout << GREEN << "Juego de Sudoku Seleccionado." << RESET << endl;
                cout << "Agregado al carrito." << endl;
                carrito.push_back("Juego de Sudoku");
                break;
            case 3:
                cout << GREEN << "Juego de Cuatro en Raya Seleccionado." << RESET << endl;
                cout << "Agregado al carrito." << endl;
                carrito.push_back("Juego de Cuatro en Raya");
                break;
            case 4:
                cout << YELLOW << "Carrito de Compras:" << RESET << endl;
                cout << "===================" << endl;
                if (carrito.empty()) {
                    cout << "El carrito está vacío." << endl;
                } else {
                    for (const auto &item : carrito) {
                        cout << "- " << item << endl;
                    }
                }
                cout << "===================" << endl;
                break;
            case 5:
                if (carrito.empty()) {
                    cout << RED << "El carrito está vacío. No hay nada que pagar." << RESET << endl;
                } else {
                    cout << GREEN << "Total a pagar: " << carrito.size() * 20 << "$" << RESET << endl;
                    cout << "Gracias por su compra!" << endl;
                    
                    // Agregar juegos a la biblioteca del usuario actual
                    for (const auto &item : carrito) {
                        bool yaExiste = false;
                        // Verificar si el juego ya está en la biblioteca
                        for (const auto &game : usuarios[usuarioActualIndex].games) {
                            if (game == item) {
                                yaExiste = true;
                                break;
                            }
                        }
                        // Si no existe, lo agregamos
                        if (!yaExiste) {
                            usuarios[usuarioActualIndex].games.push_back(item);
                        }
                    }
                    carrito.clear();
                }
                break;
            case 6:
                cout << "Volviendo al menú principal..." << endl;
                salirTienda = false;
                break;
            default:
                cout << RED << "Opción no válida. Intente nuevamente." << RESET << endl;
                break;
        }
        
        if (opcion != 6) {
            cout << "Presione cualquier tecla para continuar...";
            getch();
        }
    }
}

// Función para mostrar la biblioteca de juegos y permitir jugar
void PageLibrary() {
    if (usuarioActualIndex == -1) {
        system("cls");
        cout << RED << "Primero debe iniciar sesión para acceder a su biblioteca." << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        return;
    }
    
    system("cls");
    int opcion;
    bool salirBiblioteca = false;
    
    while (!salirBiblioteca) {
        system("cls");
        cout << BLUE << "Biblioteca de Juegos de " << usuarios[usuarioActualIndex].username << RESET << endl;
        cout << BLUE << "====================" << RESET << endl;
        
        if (usuarios[usuarioActualIndex].games.empty()) {
            cout << YELLOW << "No tienes juegos en tu biblioteca." << RESET << endl;
            cout << "Ve a la tienda para comprar juegos." << endl;
            cout << "\nPresione cualquier tecla para volver al menú principal...";
            getch();
            salirBiblioteca = true;
        } else {
            cout << "Tus juegos:" << endl;
            int contador = 1;
            for (const auto &game : usuarios[usuarioActualIndex].games) {
                cout << contador << ". " << game << endl;
                contador++;
            }
            cout << contador << ". Volver al menú principal" << endl;
            
            cout << "\nSelecciona un juego para jugar o " << contador << " para volver: ";
            cin >> opcion;
            
            if (opcion == contador) {
                salirBiblioteca = true;
            } else if (opcion > 0 && opcion < contador) {
                auto it = usuarios[usuarioActualIndex].games.begin();
                advance(it, opcion - 1);
                string juegoSeleccionado = *it;
                
                if (juegoSeleccionado == "Juego de Snake") {
                    playSnake();
                } else if (juegoSeleccionado == "Juego de Sudoku") {
                    playSudoku();
                }
                else if (juegoSeleccionado == "Juego de Cuatro en Raya") {
                    playCuatroEnRaya();
                } else {
                    cout << RED << "Juego no reconocido." << RESET << endl;
                }
            } else {
                cout << RED << "Opción no válida. Intente nuevamente." << RESET << endl;
                cout << "Presione cualquier tecla para continuar...";
                getch();
            }
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    
    int opcion;
    
    cargarUsuariosDesdeArchivo();
    do {
        mostrarMenu();
        cin >> opcion;
        switch (opcion) {
            case 1: 
                PageRegister();
                guardarUsuariosEnArchivo();
                break;
            case 2: 
                PageLogin();
                break;
            case 3: 
                PageStore();
                guardarUsuariosEnArchivo();
                break;
            case 4: 
                PageLibrary();
                break;
            case 5: 
                verUsuariosRegistrados();
                break;
            case 6:
                cerrarSesion();
                guardarUsuariosEnArchivo();
                break;
            case 7:
    			eliminarCuentaActual();
   				break;
            case 8: 
                cout << RED << "\nSaliendo de la aplicación. ¡Hasta pronto!\n" << RESET << endl;
                break;
            default:
                cout << RED << "\nOpción no válida. Intente nuevamente.\n" << RESET << endl;
                cout << "Presione cualquier tecla para continuar...";
                getch();
                system("cls");
        }
        if (opcion != 7) {
            system("cls");
}
    } while (opcion != 7);

    return 0;
}
