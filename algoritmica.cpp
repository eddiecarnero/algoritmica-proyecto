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
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

// Funcion segura para convertir string a int
int stringToIntSafe(const string& str) {
    if (str.empty()) {
        return 0;
    }

    // Eliminar espacios en blanco
    string cleanStr = str;
    cleanStr.erase(0, cleanStr.find_first_not_of(" \t\n\r\f\v"));
    cleanStr.erase(cleanStr.find_last_not_of(" \t\n\r\f\v") + 1);

    if (cleanStr.empty()) {
        return 0;
    }

    // Verificar que solo contenga digitos (y opcionalmente signo negativo)
    bool hasDigits = false;
    for (size_t i = 0; i < cleanStr.length(); i++) {
        if (i == 0 && (cleanStr[i] == '-' || cleanStr[i] == '+')) {
            continue;
        }
        if (isdigit(cleanStr[i])) {
            hasDigits = true;
        } else {
            return 0; // Caracter invalido encontrado
        }
    }

    if (!hasDigits) {
        return 0;
    }

    try {
        return stoi(cleanStr);
    } catch (const invalid_argument&) {
        return 0;
    } catch (const out_of_range&) {
        return 0;
    } catch (...) {
        return 0;
    }
}

// Estructura para datos de tarjeta de credito
struct TarjetaCredito {
    string numero;
    string fechaVencimiento;
    string cvv;
    string nombreTitular;
    string tipoTarjeta;
};

// Estructura para historial de compras
struct Compra {
    string juego;
    double precio;
    string fecha;
    string numeroTransaccion;
};

// Estructura para almacenar informacion del usuario
struct Usuario {
    string username;
    string password;
    map<string, int> puntuaciones;  // Puntuaciones por juego
    list<string> games;             // Juegos en biblioteca
    int totalPartidas;
    int puntuacionTotal;
    
    // Campos para personalizacion
    string avatar;          // Emoji o simbolo para representar al usuario
    string estado;          // Estado personalizado
    string colorPreferido;  // Color preferido (codigo ANSI)
    string biografia;       // Biografia corta del usuario
    
    // Campos para sistema de amigos
    vector<string> amigos;      // Lista de nombres de usuario de amigos
    vector<string> solicitudes; // Solicitudes de amistad pendientes
    vector<string> bloqueados;  // Usuarios bloqueados
    
    // Nuevos campos para sistema de pago
    vector<Compra> historialCompras; // Historial de compras realizadas
    double totalGastado;             // Total gastado en la tienda
    int coins;                       // Monedas del usuario para compras en la tienda

    Usuario(string user = "", string pass = "") : username(user), password(pass), 
        totalPartidas(0), puntuacionTotal(0), avatar("??"), estado("Online"), 
        colorPreferido("\033[32m"), biografia("Nuevo jugador"), totalGastado(0.0), coins(0) {}
};

// Funcion para actualizar record de un usuario
void actualizarRecord(Usuario &user, const string &juego, int puntuacion) {
    #define GREEN "\033[32m"
    #define RESET "\033[0m"
    #define YELLOW "\033[33m"
    
    user.totalPartidas++;
    user.puntuacionTotal += puntuacion;
    
    if (user.puntuaciones.find(juego) == user.puntuaciones.end() || puntuacion > user.puntuaciones[juego]) {
        user.puntuaciones[juego] = puntuacion;
        cout << GREEN << "Nuevo record en " << juego << ": " << puntuacion << "!" << RESET << endl;
    } else {
        cout << YELLOW << "PuntuaciOn: " << puntuacion << " (Record actual: " << user.puntuaciones[juego] << ")" << RESET << endl;
    }
}

// Funcion para obtener la fecha actual como string
string obtenerFechaActual() {
    time_t t = time(0);
    struct tm* now = localtime(&t);
    char buffer[80];
    strftime(buffer, 80, "%d/%m/%Y %H:%M", now);
    return string(buffer);
}

string playerName;
vector<Usuario> usuarios;
int usuarioActualIndex = -1;

// Codigos de colores ANSI
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"

// Funcion principal para procesar pago con tarjeta


// Funcion para validar numero de tarjeta usando algoritmo de Luhn
bool validarNumeroTarjeta(const string& numero) {
    string numeroLimpio = numero;
    numeroLimpio.erase(remove_if(numeroLimpio.begin(), numeroLimpio.end(), 
                      [](char c) { return c == ' ' || c == '-'; }), numeroLimpio.end());
    
    if (!all_of(numeroLimpio.begin(), numeroLimpio.end(), ::isdigit)) {
        return false;
    }
    
    if (numeroLimpio.length() < 13 || numeroLimpio.length() > 19) {
        return false;
    }
    
    int suma = 0;
    bool alternar = false;
    
    for (int i = numeroLimpio.length() - 1; i >= 0; i--) {
        int digito = numeroLimpio[i] - '0';
        
        if (alternar) {
            digito *= 2;
            if (digito > 9) {
                digito = (digito % 10) + 1;
            }
        }
        
        suma += digito;
        alternar = !alternar;
    }
    
    return (suma % 10) == 0;
}

// Funcion para determinar tipo de tarjeta
string determinarTipoTarjeta(const string& numero) {
    string numeroLimpio = numero;
    numeroLimpio.erase(remove_if(numeroLimpio.begin(), numeroLimpio.end(), 
                      [](char c) { return c == ' ' || c == '-'; }), numeroLimpio.end());
    
    if (numeroLimpio.length() >= 2) {
        string prefijo2 = numeroLimpio.substr(0, 2);
        string prefijo4 = numeroLimpio.length() >= 4 ? numeroLimpio.substr(0, 4) : "";
        
        if (numeroLimpio[0] == '4') {
            return "Visa";
        } else if (prefijo2 >= "51" && prefijo2 <= "55") {
            return "Mastercard";
        } else if (prefijo2 == "34" || prefijo2 == "37") {
            return "American Express";
        } else if (prefijo4 == "6011" || prefijo2 == "65") {
            return "Discover";
        }
    }
    
    return "Desconocida";
}



// Prototipo de función


// Funcion para validar fecha de vencimiento
bool validarFechaVencimiento(const string& fecha) {
    if (fecha.length() != 5 || fecha[2] != '/') {
        return false;
    }
    
    string mesStr = fecha.substr(0, 2);
    string anoStr = fecha.substr(3, 2);
    
    if (!all_of(mesStr.begin(), mesStr.end(), ::isdigit) || 
        !all_of(anoStr.begin(), anoStr.end(), ::isdigit)) {
        return false;
    }
    
    int mes = stoi(mesStr);
    int ano = stoi(anoStr) + 2000;
    
    if (mes < 1 || mes > 12) {
        return false;
    }
    
    time_t t = time(0);
    struct tm* now = localtime(&t);
    int anoActual = now->tm_year + 1900;
    int mesActual = now->tm_mon + 1;
    
    if (ano < anoActual || (ano == anoActual && mes < mesActual)) {
        return false;
    }
    
    if (ano > anoActual + 10) {
        return false;
    }
    
    return true;
}

// Funcion para simular verificacion con banco
bool verificarTarjetaConBanco(const TarjetaCredito) {
    cout << YELLOW << "Conectando con el banco";
    for (int i = 0; i < 5; i++) {
        cout << ".";
        cout.flush();
        Sleep(400);
    }
    cout << endl;
    
    int resultado = rand() % 100;
    
    if (resultado < 80) {
        cout << GREEN << "Tarjeta verificada exitosamente!" << RESET << endl;
        return true;
    } else if (resultado < 90) {
        cout << RED << "Tarjeta rechazada: Fondos insuficientes" << RESET << endl;
        return false;
    } else if (resultado < 95) {
        cout << RED << "Tarjeta rechazada: Tarjeta bloqueada" << RESET << endl;
        return false;
    } else {
        cout << RED << "Error de conexion con el banco. Intente nuevamente." << RESET << endl;
        return false;
    }
}

// Funcion para generar numero de transaccion
string generarNumeroTransaccion() {
    return "TXN" + to_string(rand() % 900000 + 100000);
}

// Funcion para validar CVV
bool validarCVV(const string& cvv, const string& tipoTarjeta) {
    if (!all_of(cvv.begin(), cvv.end(), ::isdigit)) {
        return false;
    }
    
    if (tipoTarjeta == "American Express") {
        return cvv.length() == 4;
    } else {
        return cvv.length() == 3;
    }
}

// Funcion para validar nombre del titular
bool validarNombreTitular(const string& nombre) {
    if (nombre.length() < 2 || nombre.length() > 50) {
        return false;
    }
    
    return all_of(nombre.begin(), nombre.end(), 
                  [](char c) { return isalpha(c) || c == ' '; });
}

// Funcion para procesar el pago con tarjeta de credito
pair<bool, string> procesarPagoTarjeta(double monto) {
    TarjetaCredito tarjeta;
    
    system("cls");
    cout << BLUE << "========================================" << RESET << endl;
    cout << BLUE << "     SISTEMA DE PAGO SEGURO             " << RESET << endl;
    cout << BLUE << "========================================" << RESET << endl;
    cout << endl;
    cout << YELLOW << "Monto a pagar: $" << fixed << setprecision(2) << monto << RESET << endl;
    cout << BLUE << "----------------------------------------" << RESET << endl;
    cout << endl;
    
    // Solicitar numero de tarjeta
    cout << CYAN << "Ingrese el numero de tarjeta: " << RESET;
    cin.ignore();
    getline(cin, tarjeta.numero);
    
    if (!validarNumeroTarjeta(tarjeta.numero)) {
        cout << RED << "Error: Numero de tarjeta invalido" << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        return make_pair(false, "");
    }
    
    tarjeta.tipoTarjeta = determinarTipoTarjeta(tarjeta.numero);
    cout << GREEN << "Tipo de tarjeta: " << tarjeta.tipoTarjeta << RESET << endl;
    
    // Solicitar fecha de vencimiento
    cout << CYAN << "Fecha de vencimiento (MM/YY): " << RESET;
    getline(cin, tarjeta.fechaVencimiento);
    
    if (!validarFechaVencimiento(tarjeta.fechaVencimiento)) {
        cout << RED << "Error: Fecha invalida o tarjeta vencida" << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        return make_pair(false, "");
    }
    
    // Solicitar CVV
    cout << CYAN << "CVV (" << (tarjeta.tipoTarjeta == "American Express" ? "4" : "3") << " digitos): " << RESET;
    getline(cin, tarjeta.cvv);
    
    if (!validarCVV(tarjeta.cvv, tarjeta.tipoTarjeta)) {
        cout << RED << "Error: CVV invalido" << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        return make_pair(false, "");
    }
    
    // Solicitar nombre del titular
    cout << CYAN << "Nombre del titular: " << RESET;
    getline(cin, tarjeta.nombreTitular);
    
    if (!validarNombreTitular(tarjeta.nombreTitular)) {
        cout << RED << "Error: Nombre del titular invalido" << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        return make_pair(false, "");
    }
    
    // Mostrar resumen del pago
    system("cls");
    cout << BLUE << "========================================" << RESET << endl;
    cout << BLUE << "           RESUMEN DE PAGO              " << RESET << endl;
    cout << BLUE << "========================================" << RESET << endl;
    cout << endl;
    cout << CYAN << "Tarjeta: " << RESET << tarjeta.tipoTarjeta << " ****" << tarjeta.numero.substr(max(0, (int)tarjeta.numero.length() - 4)) << endl;
    cout << CYAN << "Titular: " << RESET << tarjeta.nombreTitular << endl;
    cout << CYAN << "Monto: " << RESET << "$" << fixed << setprecision(2) << monto << endl;
    cout << BLUE << "----------------------------------------" << RESET << endl;
    cout << endl;
    
    cout << YELLOW << "Confirmar pago? (s/n): " << RESET;
    char confirmacion;
    cin >> confirmacion;
    
    if (confirmacion != 's' && confirmacion != 'S') {
        cout << YELLOW << "Pago cancelado por el usuario" << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        return make_pair(false, "");
    }
    
    // Procesar pago
    cout << endl;
    bool pagoExitoso = verificarTarjetaConBanco(tarjeta);
    
    if (pagoExitoso) {
        string numeroTransaccion = generarNumeroTransaccion();
        cout << endl;
        cout << GREEN << "========================================" << RESET << endl;
        cout << GREEN << "         PAGO EXITOSO                   " << RESET << endl;
        cout << GREEN << "========================================" << RESET << endl;
        cout << GREEN << "Transaccion completada!" << RESET << endl;
        cout << GREEN << "Numero de transaccion: " << numeroTransaccion << RESET << endl;
        cout << GREEN << "Fecha: " << obtenerFechaActual() << RESET << endl;
        return make_pair(true, numeroTransaccion);
    }
    
    cout << "Presione cualquier tecla para continuar...";
    getch();
    return make_pair(false, "");
}

// Funcion para gestionar coins (monedas) del usuario
void gestionarCoins() {
    if (usuarioActualIndex == -1) {
        cout << RED << "Debe iniciar sesion primero." << RESET << endl;
        getch();
        return;
    }
    
    Usuario& user = usuarios[usuarioActualIndex];
    bool salir = false;
    
    while (!salir) {
        system("cls");
        cout << BLUE << "========================================" << RESET << endl;
        cout << YELLOW << "       GESTION DE COINS (MONEDAS)       " << RESET << endl;
        cout << BLUE << "========================================" << RESET << endl;
        cout << "Usuario: " << user.username << endl;
        cout << "Coins disponibles: " << GREEN << user.coins << RESET << endl;
        cout << BLUE << "----------------------------------------" << RESET << endl;
        cout << "1. Comprar mas coins" << endl;
        cout << "2. Ver historial de compras" << endl;
        cout << "3. Volver al menu principal" << endl;
        cout << BLUE << "========================================" << RESET << endl;
        cout << "Seleccione una opcion: ";
        
        int opcion;
        cin >> opcion;
        
        switch (opcion) {
            case 1: {
                system("cls");
                cout << BLUE << "========================================" << RESET << endl;
                cout << YELLOW << "          COMPRAR COINS              " << RESET << endl;
                cout << BLUE << "========================================" << RESET << endl;
                cout << "Paquetes disponibles:" << endl;
                cout << "1. 100 coins - $5.00" << endl;
                cout << "2. 250 coins - $10.00" << endl;
                cout << "3. 500 coins - $18.00" << endl;
                cout << "4. 1000 coins - $30.00" << endl;
                cout << BLUE << "----------------------------------------" << RESET << endl;
                cout << "Seleccione un paquete (1-4) o 0 para cancelar: ";
                
                int paquete;
                cin >> paquete;
                
                if (paquete >= 1 && paquete <= 4) {
                    double precios[] = {5.00, 10.00, 18.00, 30.00};
                    int coins[] = {100, 250, 500, 1000};
                    
                    cout << "Procesando pago de $" << precios[paquete-1] << " por " 
                         << coins[paquete-1] << " coins..." << endl;
                    
                    auto resultado = procesarPagoTarjeta(precios[paquete-1]);
                    
                    if (resultado.first) {
                        user.coins += coins[paquete-1];
                        user.totalGastado += precios[paquete-1];
                        
                        // Registrar la compra de coins
                        Compra compraCoins;
                        compraCoins.juego = "Recarga de " + to_string(coins[paquete-1]) + " coins";
                        compraCoins.precio = precios[paquete-1];
                        compraCoins.fecha = obtenerFechaActual();
                        compraCoins.numeroTransaccion = resultado.second;
                        user.historialCompras.push_back(compraCoins);
                        
                        cout << GREEN << "¡Compra exitosa! Ahora tienes " << user.coins << " coins." << RESET << endl;
                    } else {
                        cout << RED << "El pago no se completo. No se añadieron coins." << RESET << endl;
                    }
                }
                break;
            }
            case 2:
                // Mostrar historial (usar función existente)
                break;
            case 3:
                salir = true;
                break;
            default:
                cout << RED << "Opción invalida." << RESET << endl;
        }
        
        if (opcion != 3) {
            cout << "Presione cualquier tecla para continuar...";
            getch();
        }
    }
}

// Funcion para dibujar figuras ASCII para avatares
vector<string> obtenerFigurasAscii() {
    vector<string> figuras = {
        "[*]",      // Estrella simple
        "[@]",      // Arroba
        "[#]",      // Numeral
        "[+]",      // Signo mas
        "[O]",      // Circulo
        "[X]",      // X
        "[!]",      // Exclamacion
        "[?]",      // Interrogacion
        "<|>",      // Flecha
        "{*}",      // Estrella con llaves
        "(*)",      // Estrella con parentesis
        "/|\\",     // Triangulo
        "\\o/",     // Persona con brazos levantados
        "-o-",      // Persona con brazos extendidos
        "<==>",     // Doble flecha
        "[:)]",     // Cara feliz
        "[:(]",     // Cara triste
        "[<3]",     // Corazon
        "[^^]",     // Ojos felices
        "[--]",     // Linea
        "[==]",     // Doble linea
        "[:::]",    // Puntos
        "[%%%]",    // Porcentaje
        "[###]"     // Triple numeral
    };
    return figuras;
}

// Funcion para mostrar una figura ASCII con color
void mostrarFiguraAscii(const string& figura, const string& color) {
    cout << color << figura << RESET;
}

// Funcion para personalizar perfil
void personalizarPerfil() {
    if (usuarioActualIndex == -1) {
        cout << RED << "Debe iniciar sesion primero." << RESET << endl;
        getch();
        return;
    }
    
    bool salirPersonalizacion = false;
    
    while (!salirPersonalizacion) {
        system("cls");
        Usuario& user = usuarios[usuarioActualIndex];
        
        cout << BLUE << "========================================" << RESET << endl;
        cout << YELLOW << "       PERSONALIZACION DE PERFIL       " << RESET << endl;
        cout << BLUE << "========================================" << RESET << endl;
        
        // Mostrar perfil actual
        cout << CYAN << "PERFIL ACTUAL:" << RESET << endl;
        cout << "Avatar: " << user.colorPreferido << user.avatar << RESET << endl;
        cout << "Usuario: " << user.colorPreferido << user.username << RESET << endl;
        cout << "Estado: " << user.estado << endl;
        cout << "Biografia: " << user.biografia << endl;
        cout << "Amigos: " << user.amigos.size() << endl;
        cout << "Partidas jugadas: " << user.totalPartidas << endl;
        cout << "Puntuacion total: " << user.puntuacionTotal << endl;
        
        cout << "\n" << BLUE << "OPCIONES DE PERSONALIZACION:" << RESET << endl;
        cout << "1. Cambiar Avatar" << endl;
        cout << "2. Cambiar Estado" << endl;
        cout << "3. Cambiar Color Preferido" << endl;
        cout << "4. Cambiar Biografia" << endl;
        cout << "5. Ver perfil completo" << endl;
        cout << "6. Volver al menu principal" << endl;
        
        int opcion;
        cout << "\nSeleccione una opcion: ";
        cin >> opcion;
        
        switch (opcion) {
            case 1: {
                system("cls");
                cout << BLUE << "Seleccionar Avatar" << RESET << endl;
                cout << BLUE << "=================" << RESET << endl;
                
                vector<string> figuras = obtenerFigurasAscii();
                
                cout << "Figuras ASCII disponibles:\n";
                for (size_t i = 0; i < figuras.size(); i++) {
                    cout << i+1 << ". " << user.colorPreferido << figuras[i] << RESET;
                    if ((i + 1) % 6 == 0) cout << "\n";
                    else cout << "  ";
                }
                cout << "\n\n";
                
                int seleccion;
                cout << "Seleccione una figura (1-" << figuras.size() << "): ";
                cin >> seleccion;
                
                if (seleccion > 0 && seleccion <= static_cast<int>(figuras.size())) {
                    user.avatar = figuras[seleccion-1];
                    cout << GREEN << "Avatar actualizado a " << user.colorPreferido << user.avatar << RESET << endl;
                } else {
                    cout << RED << "Seleccion invalida." << RESET << endl;
                }
                break;
            }
            case 2: {
                system("cls");
                cout << BLUE << "Cambiar Estado" << RESET << endl;
                cout << BLUE << "==============" << RESET << endl;
                
                cout << "Estados predefinidos:\n";
                cout << "1. Online" << endl;
                cout << "2. Jugando" << endl;
                cout << "3. Ocupado" << endl;
                cout << "4. Ausente" << endl;
                cout << "5. No molestar" << endl;
                cout << "6. Estado personalizado" << endl;
                
                int seleccion;
                cout << "\nSeleccione una opcion: ";
                cin >> seleccion;
                
                switch (seleccion) {
                    case 1: user.estado = "Online"; break;
                    case 2: user.estado = "Jugando"; break;
                    case 3: user.estado = "Ocupado"; break;
                    case 4: user.estado = "Ausente"; break;
                    case 5: user.estado = "No molestar"; break;
                    case 6: {
                        cout << "Ingrese su estado personalizado: ";
                        cin.ignore();
                        getline(cin, user.estado);
                        break;
                    }
                    default:
                        cout << RED << "Opcion invalida." << RESET << endl;
                        continue;
                }
                cout << GREEN << "Estado actualizado a: " << user.estado << RESET << endl;
                break;
            }
            case 3: {
                system("cls");
                cout << BLUE << "Cambiar Color Preferido" << RESET << endl;
                cout << BLUE << "======================" << RESET << endl;
                
                cout << "Colores disponibles:\n";
                cout << GREEN << "1. Verde" << RESET << endl;
                cout << BLUE << "2. Azul" << RESET << endl;
                cout << RED << "3. Rojo" << RESET << endl;
                cout << YELLOW << "4. Amarillo" << RESET << endl;
                cout << MAGENTA << "5. Magenta" << RESET << endl;
                cout << CYAN << "6. Cian" << RESET << endl;
                
                int seleccion;
                cout << "\nSeleccione un color: ";
                cin >> seleccion;
                
                switch (seleccion) {
                    case 1: user.colorPreferido = GREEN; break;
                    case 2: user.colorPreferido = BLUE; break;
                    case 3: user.colorPreferido = RED; break;
                    case 4: user.colorPreferido = YELLOW; break;
                    case 5: user.colorPreferido = MAGENTA; break;
                    case 6: user.colorPreferido = CYAN; break;
                    default:
                        cout << RED << "Opcion invalida." << RESET << endl;
                        continue;
                }
                cout << GREEN << "Color actualizado." << RESET << endl;
                break;
            }
            case 4: {
                system("cls");
                cout << BLUE << "Cambiar Biografia" << RESET << endl;
                cout << BLUE << "=================" << RESET << endl;
                
                cout << "Biografia actual: " << user.biografia << endl;
                cout << "\nIngrese su nueva biografia (maximo 100 caracteres): ";
                cin.ignore();
                getline(cin, user.biografia);
                
                if (user.biografia.length() > 100) {
                    user.biografia = user.biografia.substr(0, 100);
                    cout << YELLOW << "Biografia recortada a 100 caracteres." << RESET << endl;
                }
                
                cout << GREEN << "Biografia actualizada." << RESET << endl;
                break;
            }
            case 5: {
                system("cls");
                cout << CYAN << "========================================" << RESET << endl;
                cout << YELLOW << "       PERFIL COMPLETO DE " << user.username << RESET << endl;
                cout << CYAN << "========================================" << RESET << endl;
                
                cout << "Avatar: " << user.colorPreferido << user.avatar << RESET << endl;
                cout << "Usuario: " << user.colorPreferido << user.username << RESET << endl;
                cout << "Estado: " << user.estado << endl;
                cout << "Biografia: " << user.biografia << endl;
                cout << "\n" << BLUE << "ESTADiSTICAS:" << RESET << endl;
                cout << "Partidas jugadas: " << user.totalPartidas << endl;
                cout << "Puntuacion total: " << user.puntuacionTotal << endl;
                cout << "Amigos: " << user.amigos.size() << endl;
                
                if (user.totalPartidas > 0) {
                    cout << "Promedio por partida: " << (user.puntuacionTotal / user.totalPartidas) << endl;
                }
                
                cout << "\n" << BLUE << "JUEGOS EN BIBLIOTECA:" << RESET << endl;
                if (user.games.empty()) {
                    cout << YELLOW << "No tienes juegos en tu biblioteca." << RESET << endl;
                } else {
                    for (const auto& game : user.games) {
                        cout << "? " << game;
                        if (user.puntuaciones.count(game)) {
                            cout << " (Record: " << user.puntuaciones.at(game) << ")";
                        }
                        cout << endl;
                    }
                }
                
                cout << "\n" << BLUE << "AMIGOS:" << RESET << endl;
                if (user.amigos.empty()) {
                    cout << YELLOW << "No tienes amigos en tu lista." << RESET << endl;
                } else {
                    for (const auto& amigo : user.amigos) {
                        cout << "? " << amigo << endl;
                    }
                }
                break;
            }
            case 6:
                salirPersonalizacion = true;
                break;
            default:
                cout << RED << "Opcion invalida." << RESET << endl;
        }
        
        if (opcion != 6) {
            cout << "\nPresione cualquier tecla para continuar...";
            getch();
        }
    }
}

// Funcion para gestionar amigos
void gestionarAmigos() {
    if (usuarioActualIndex == -1) {
        cout << RED << "Debe iniciar sesion primero." << RESET << endl;
        getch();
        return;
    }
    
    bool salirAmigos = false;
    
    while (!salirAmigos) {
        system("cls");
        Usuario& user = usuarios[usuarioActualIndex];
        
        cout << BLUE << "========================================" << RESET << endl;
        cout << YELLOW << "         GESTION DE AMIGOS              " << RESET << endl;
        cout << BLUE << "========================================" << RESET << endl;
        
        cout << CYAN << "RESUMEN:" << RESET << endl;
        cout << "Amigos: " << user.amigos.size() << endl;
        cout << "Solicitudes pendientes: " << user.solicitudes.size() << endl;
        cout << "Usuarios bloqueados: " << user.bloqueados.size() << endl;
        
        cout << "\n" << BLUE << "OPCIONES:" << RESET << endl;
        cout << "1. Ver lista de amigos" << endl;
        cout << "2. Buscar y anadir usuarios" << endl;
        cout << "3. Ver solicitudes pendientes" << endl;
        cout << "4. Gestionar usuarios bloqueados" << endl;
        cout << "5. Ver perfil de un amigo" << endl;
        cout << "6. Eliminar amigo" << endl;
        cout << "7. Volver al menu principal" << endl;
        
        int opcion;
        cout << "\nSeleccione una Opcion: ";
        cin >> opcion;
        
        switch (opcion) {
            case 1: {
                system("cls");
                cout << BLUE << "Lista de Amigos" << RESET << endl;
                cout << BLUE << "==============" << RESET << endl;
                
                if (user.amigos.empty()) {
                    cout << YELLOW << "No tienes amigos en tu lista." << RESET << endl;
                    cout << "Usa la Opcion 'Buscar y anadir usuarios' para encontrar amigos!" << endl;
                } else {
                    cout << "Tienes " << user.amigos.size() << " amigo(s):\n\n";
                    
                    for (size_t i = 0; i < user.amigos.size(); i++) {
                        const string& amigo = user.amigos[i];
                        cout << i+1 << ". " << amigo;
                        
                        // Buscar informacion del amigo
                        for (const auto& u : usuarios) {
                            if (u.username == amigo) {
                                cout << " " << u.avatar << " - " << u.estado;
                                cout << " (Partidas: " << u.totalPartidas << ", Puntos: " << u.puntuacionTotal << ")";
                                break;
                            }
                        }
                        cout << endl;
                    }
                }
                break;
            }
            case 2: {
                system("cls");
                cout << BLUE << "Buscar y anadir Usuarios" << RESET << endl;
                cout << BLUE << "=======================" << RESET << endl;
                
                string busqueda;
                cout << "Ingrese nombre de usuario a buscar: ";
                cin >> busqueda;
                
                vector<int> usuariosEncontrados;
                
                for (size_t i = 0; i < usuarios.size(); i++) {
                    if (i == static_cast<size_t>(usuarioActualIndex)) continue; // No mostrar al usuario actual
                    
                    if (usuarios[i].username.find(busqueda) != string::npos) {
                        usuariosEncontrados.push_back(i);
                    }
                }
                
                if (usuariosEncontrados.empty()) {
                    cout << YELLOW << "No se encontraron usuarios que coincidan con la busqueda." << RESET << endl;
                } else {
                    cout << "\nUsuarios encontrados:\n";
                    
                    for (size_t j = 0; j < usuariosEncontrados.size(); j++) {
                        int i = usuariosEncontrados[j];
                        cout << j+1 << ". " << usuarios[i].avatar << " " << usuarios[i].username;
                        cout << " - " << usuarios[i].estado;
                        cout << " (Nivel: " << (usuarios[i].totalPartidas / 10 + 1) << ")";
                        
                        // Verificar relacion actual
                        bool esAmigo = find(user.amigos.begin(), user.amigos.end(), usuarios[i].username) != user.amigos.end();
                        bool estaBloqueado = find(user.bloqueados.begin(), user.bloqueados.end(), usuarios[i].username) != user.bloqueados.end();
                        bool solicitudEnviada = find(usuarios[i].solicitudes.begin(), usuarios[i].solicitudes.end(), user.username) != usuarios[i].solicitudes.end();
                        
                        if (esAmigo) {
                            cout << GREEN << " [Amigo]" << RESET;
                        } else if (estaBloqueado) {
                            cout << RED << " [Bloqueado]" << RESET;
                        } else if (solicitudEnviada) {
                            cout << YELLOW << " [Solicitud enviada]" << RESET;
                        }
                        
                        cout << endl;
                    }
                    
                    cout << "\nSeleccione un usuario para ver opciones (0 para cancelar): ";
                    int seleccion;
                    cin >> seleccion;
                    
                    if (seleccion > 0 && seleccion <= static_cast<int>(usuariosEncontrados.size())) {
                        int indiceUsuario = usuariosEncontrados[seleccion - 1];
                        Usuario& usuarioSeleccionado = usuarios[indiceUsuario];
                        
                        system("cls");
                        cout << CYAN << "Perfil de " << usuarioSeleccionado.username << RESET << endl;
                        cout << "Avatar: " << usuarioSeleccionado.avatar << endl;
                        cout << "Estado: " << usuarioSeleccionado.estado << endl;
                        cout << "Biografia: " << usuarioSeleccionado.biografia << endl;
                        cout << "Partidas jugadas: " << usuarioSeleccionado.totalPartidas << endl;
                        cout << "Puntuacion total: " << usuarioSeleccionado.puntuacionTotal << endl;
                        
                        // Verificar relacion actual
                        bool esAmigo = find(user.amigos.begin(), user.amigos.end(), usuarioSeleccionado.username) != user.amigos.end();
                        bool estaBloqueado = find(user.bloqueados.begin(), user.bloqueados.end(), usuarioSeleccionado.username) != user.bloqueados.end();
                        bool solicitudEnviada = find(usuarioSeleccionado.solicitudes.begin(), usuarioSeleccionado.solicitudes.end(), user.username) != usuarioSeleccionado.solicitudes.end();
                        
                        cout << "\nOpciones disponibles:\n";
                        
                        if (esAmigo) {
                            cout << "1. Eliminar de amigos" << endl;
                            cout << "2. Bloquear usuario" << endl;
                        } else if (estaBloqueado) {
                            cout << "1. Desbloquear usuario" << endl;
                        } else if (solicitudEnviada) {
                            cout << "1. Cancelar solicitud de amistad" << endl;
                            cout << "2. Bloquear usuario" << endl;
                        } else {
                            cout << "1. Enviar solicitud de amistad" << endl;
                            cout << "2. Bloquear usuario" << endl;
                        }
                        cout << "0. Volver" << endl;
                        
                        int accion;
                        cout << "\nSeleccione una accion: ";
                        cin >> accion;
                        
                        if (accion == 1) {
                            if (esAmigo) {
                                // Eliminar de amigos
                                user.amigos.erase(find(user.amigos.begin(), user.amigos.end(), usuarioSeleccionado.username));
                                usuarioSeleccionado.amigos.erase(find(usuarioSeleccionado.amigos.begin(), usuarioSeleccionado.amigos.end(), user.username));
                                cout << GREEN << "Has eliminado a " << usuarioSeleccionado.username << " de tu lista de amigos." << RESET << endl;
                            } else if (estaBloqueado) {
                                // Desbloquear
                                user.bloqueados.erase(find(user.bloqueados.begin(), user.bloqueados.end(), usuarioSeleccionado.username));
                                cout << GREEN << "Has desbloqueado a " << usuarioSeleccionado.username << RESET << endl;
                            } else if (solicitudEnviada) {
                                // Cancelar solicitud
                                usuarioSeleccionado.solicitudes.erase(find(usuarioSeleccionado.solicitudes.begin(), usuarioSeleccionado.solicitudes.end(), user.username));
                                cout << GREEN << "Has cancelado la solicitud de amistad a " << usuarioSeleccionado.username << RESET << endl;
                            } else {
                                // Enviar solicitud
                                usuarioSeleccionado.solicitudes.push_back(user.username);
                                cout << GREEN << "Solicitud de amistad enviada a " << usuarioSeleccionado.username << RESET << endl;
                            }
                        } else if (accion == 2 && !estaBloqueado) {
                            // Bloquear usuario
                            if (esAmigo) {
                                // Eliminar de amigos primero
                                user.amigos.erase(find(user.amigos.begin(), user.amigos.end(), usuarioSeleccionado.username));
                                usuarioSeleccionado.amigos.erase(find(usuarioSeleccionado.amigos.begin(), usuarioSeleccionado.amigos.end(), user.username));
                            }
                            if (solicitudEnviada) {
                                // Cancelar solicitud
                                usuarioSeleccionado.solicitudes.erase(find(usuarioSeleccionado.solicitudes.begin(), usuarioSeleccionado.solicitudes.end(), user.username));
                            }
                            user.bloqueados.push_back(usuarioSeleccionado.username);
                            cout << GREEN << "Has bloqueado a " << usuarioSeleccionado.username << RESET << endl;
                        }
                    }
                }
                break;
            }
            case 3: {
                system("cls");
                cout << BLUE << "Solicitudes de Amistad Pendientes" << RESET << endl;
                cout << BLUE << "==============================" << RESET << endl;
                
                if (user.solicitudes.empty()) {
                    cout << YELLOW << "No tienes solicitudes pendientes." << RESET << endl;
                } else {
                    cout << "Tienes " << user.solicitudes.size() << " solicitud(es) pendiente(s):\n\n";
                    
                    for (size_t i = 0; i < user.solicitudes.size(); i++) {
                        const string& solicitante = user.solicitudes[i];
                        cout << i+1 << ". " << solicitante;
                        
                        // Buscar informacion del solicitante
                        for (const auto& u : usuarios) {
                            if (u.username == solicitante) {
                                cout << " " << u.avatar << " - " << u.estado;
                                cout << " (Partidas: " << u.totalPartidas << ", Puntos: " << u.puntuacionTotal << ")";
                                break;
                            }
                        }
                        cout << endl;
                    }
                    
                    cout << "\nOpciones:\n";
                    cout << "1. Aceptar solicitud" << endl;
                    cout << "2. Rechazar solicitud" << endl;
                    cout << "3. Ver perfil del solicitante" << endl;
                    cout << "0. Volver" << endl;
                    
                    int accion;
                    cout << "\nSeleccione una accion: ";
                    cin >> accion;
                    
                    if (accion >= 1 && accion <= 3) {
                        cout << "Ingrese el numero de la solicitud: ";
                        int numSolicitud;
                        cin >> numSolicitud;
                        
                        if (numSolicitud > 0 && numSolicitud <= static_cast<int>(user.solicitudes.size())) {
                            string solicitante = user.solicitudes[numSolicitud-1];
                            
                            if (accion == 1) {
                                // Aceptar solicitud
                                user.amigos.push_back(solicitante);
                                
                                // anadir al usuario actual como amigo del solicitante
                                for (auto& u : usuarios) {
                                    if (u.username == solicitante) {
                                        u.amigos.push_back(user.username);
                                        break;
                                    }
                                }
                                
                                // Eliminar la solicitud
                                user.solicitudes.erase(user.solicitudes.begin() + numSolicitud - 1);
                                
                                cout << GREEN << "Has aceptado la solicitud de " << solicitante << "! Ahora son amigos." << RESET << endl;
                            } else if (accion == 2) {
                                // Rechazar solicitud
                                user.solicitudes.erase(user.solicitudes.begin() + numSolicitud - 1);
                                cout << YELLOW << "Has rechazado la solicitud de " << solicitante << RESET << endl;
                            } else if (accion == 3) {
                                // Ver perfil
                                for (const auto& u : usuarios) {
                                    if (u.username == solicitante) {
                                        system("cls");
                                        cout << CYAN << "Perfil de " << u.username << RESET << endl;
                                        cout << "Avatar: " << u.avatar << endl;
                                        cout << "Estado: " << u.estado << endl;
                                        cout << "Biografia: " << u.biografia << endl;
                                        cout << "Partidas jugadas: " << u.totalPartidas << endl;
                                        cout << "Puntuacion total: " << u.puntuacionTotal << endl;
                                        break;
                                    }
                                }
                            }
                        } else {
                            cout << RED << "Numero de solicitud invalido." << RESET << endl;
                        }
                    }
                }
                break;
            }
            case 4: {
                system("cls");
                cout << BLUE << "Gestionar Usuarios Bloqueados" << RESET << endl;
                cout << BLUE << "============================" << RESET << endl;
                
                if (user.bloqueados.empty()) {
                    cout << YELLOW << "No tienes usuarios bloqueados." << RESET << endl;
                } else {
                    cout << "Usuarios bloqueados:\n\n";
                    
                    for (size_t i = 0; i < user.bloqueados.size(); i++) {
                        cout << i+1 << ". " << user.bloqueados[i] << endl;
                    }
                    
                    cout << "\n1. Desbloquear usuario" << endl;
                    cout << "0. Volver" << endl;
                    
                    int accion;
                    cout << "\nSeleccione una accion: ";
                    cin >> accion;
                    
                    if (accion == 1) {
                        cout << "Ingrese el numero del usuario a desbloquear: ";
                        int numUsuario;
                        cin >> numUsuario;
                        
                        if (numUsuario > 0 && numUsuario <= static_cast<int>(user.bloqueados.size())) {
                            string usuarioBloqueado = user.bloqueados[numUsuario-1];
                            user.bloqueados.erase(user.bloqueados.begin() + numUsuario - 1);
                            cout << GREEN << "Has desbloqueado a " << usuarioBloqueado << RESET << endl;
                        } else {
                            cout << RED << "Numero invalido." << RESET << endl;
                        }
                    }
                }
                break;
            }
            case 5: {
                system("cls");
                cout << BLUE << "Ver Perfil de un Amigo" << RESET << endl;
                cout << BLUE << "=====================" << RESET << endl;
                
                if (user.amigos.empty()) {
                    cout << YELLOW << "No tienes amigos en tu lista." << RESET << endl;
                } else {
                    cout << "Selecciona un amigo:\n\n";
                    
                    for (size_t i = 0; i < user.amigos.size(); i++) {
                        cout << i+1 << ". " << user.amigos[i] << endl;
                    }
                    
                    cout << "\nIngrese el numero del amigo: ";
                    int numAmigo;
                    cin >> numAmigo;
                    
                    if (numAmigo > 0 && numAmigo <= static_cast<int>(user.amigos.size())) {
                        string nombreAmigo = user.amigos[numAmigo-1];
                        
                        // Buscar y mostrar perfil del amigo
                        for (const auto& u : usuarios) {
                            if (u.username == nombreAmigo) {
                                system("cls");
                                cout << CYAN << "========================================" << RESET << endl;
                                cout << YELLOW << "       PERFIL DE " << u.username << RESET << endl;
                                cout << CYAN << "========================================" << RESET << endl;
                                
                                cout << "Avatar: " << u.avatar << endl;
                                cout << "Usuario: " << u.colorPreferido << u.username << RESET << endl;
                                cout << "Estado: " << u.estado << endl;
                                cout << "Biografia: " << u.biografia << endl;
                                cout << "\n" << BLUE << "ESTADISTICAS:" << RESET << endl;
                                cout << "Partidas jugadas: " << u.totalPartidas << endl;
                                cout << "Puntuacion total: " << u.puntuacionTotal << endl;
                                
                                if (u.totalPartidas > 0) {
                                    cout << "Promedio por partida: " << (u.puntuacionTotal / u.totalPartidas) << endl;
                                }
                                
                                cout << "\n" << BLUE << "RECORDS:" << RESET << endl;
                                if (u.puntuaciones.empty()) {
                                    cout << YELLOW << "No tiene records registrados." << RESET << endl;
                                } else {
                                    for (const auto& record : u.puntuaciones) {
                                        cout << "? " << record.first << ": " << record.second << " puntos" << endl;
                                    }
                                }
                                break;
                            }
                        }
                    } else {
                        cout << RED << "Numero invalido." << RESET << endl;
                    }
                }
                break;
            }
            case 6: {
                system("cls");
                cout << BLUE << "Eliminar Amigo" << RESET << endl;
                cout << BLUE << "==============" << RESET << endl;
                
                if (user.amigos.empty()) {
                    cout << YELLOW << "No tienes amigos en tu lista." << RESET << endl;
                } else {
                    cout << "Selecciona el amigo a eliminar:\n\n";
                    
                    for (size_t i = 0; i < user.amigos.size(); i++) {
                        cout << i+1 << ". " << user.amigos[i] << endl;
                    }
                    
                    cout << "\nIngrese el numero del amigo a eliminar (0 para cancelar): ";
                    int numAmigo;
                    cin >> numAmigo;
                    
                    if (numAmigo > 0 && numAmigo <= static_cast<int>(user.amigos.size())) {
                        string nombreAmigo = user.amigos[numAmigo-1];
                        
                        cout << RED << "Estas seguro de que quieres eliminar a " << nombreAmigo << " de tu lista de amigos? (s/n): " << RESET;
                        char confirmacion;
                        cin >> confirmacion;
                        
                        if (confirmacion == 's' || confirmacion == 'S') {
                            // Eliminar de la lista del usuario actual
                            user.amigos.erase(user.amigos.begin() + numAmigo - 1);
                            
                            // Eliminar al usuario actual de la lista del amigo
                            for (auto& u : usuarios) {
                                if (u.username == nombreAmigo) {
                                    auto it = find(u.amigos.begin(), u.amigos.end(), user.username);
                                    if (it != u.amigos.end()) {
                                        u.amigos.erase(it);
                                    }
                                    break;
                                }
                            }
                            
                            cout << GREEN << "Has eliminado a " << nombreAmigo << " de tu lista de amigos." << RESET << endl;
                        } else {
                            cout << YELLOW << "Operacion cancelada." << RESET << endl;
                        }
                    } else if (numAmigo != 0) {
                        cout << RED << "Numero invalido." << RESET << endl;
                    }
                }
                break;
            }
            case 7:
                salirAmigos = true;
                break;
            default:
                cout << RED << "Opcion invalida." << RESET << endl;
        }
        
        if (opcion != 7) {
            cout << "\nPresione cualquier tecla para continuar...";
            getch();
        }
    }
}
void limpiarArchivoCorrupto() {
    cout << YELLOW << "Detectado archivo de usuarios corrupto o problemas de carga." << RESET << endl;
    cout << YELLOW << "Creando archivo nuevo..." << RESET << endl;

    // Eliminar archivo corrupto
    remove("usuarios.txt");

    // Crear archivo nuevo vacio
    ofstream nuevoArchivo("usuarios.txt");
    nuevoArchivo.close();

    cout << GREEN << "Archivo limpio creado exitosamente." << RESET << endl;
    cout << "Presione cualquier tecla para continuar...";
    getch();
}

void mostrarEstadisticasJugador(const Usuario& user) {
    system("cls");
    cout << CYAN << "========================================" << RESET << endl;
    cout << YELLOW << "    ESTADISTICAS DE " << user.username << RESET << endl;
    cout << CYAN << "========================================" << RESET << endl;
    
    cout << GREEN << "Partidas totales jugadas: " << user.totalPartidas << RESET << endl;
    cout << GREEN << "Puntuacion total acumulada: " << user.puntuacionTotal << RESET << endl;
    
    if (user.totalPartidas > 0) {
        cout << GREEN << "Promedio por partida: " << (user.puntuacionTotal / user.totalPartidas) << RESET << endl;
    }
    
    cout << "\n" << BLUE << "RECORDS POR JUEGO:" << RESET << endl;
    cout << BLUE << "==================" << RESET << endl;
    
    if (user.puntuaciones.empty()) {
        cout << YELLOW << "No hay records registrados aun." << RESET << endl;
    } else {
        for (const auto& record : user.puntuaciones) {
            cout << MAGENTA << "- " << record.first << ": " << GREEN << record.second << " puntos" << RESET << endl;
        }
    }
    
    cout << "\n" << BLUE << "JUEGOS EN BIBLIOTECA:" << RESET << endl;
    cout << BLUE << "=====================" << RESET << endl;
    
    if (user.games.empty()) {
        cout << YELLOW << "No tienes juegos en tu biblioteca." << RESET << endl;
    } else {
        for (const auto& game : user.games) {
            cout << CYAN << "- " << game;
            if (user.puntuaciones.count(game)) {
                cout << GREEN << " (Record: " << user.puntuaciones.at(game) << ")" << RESET;
            }
            cout << endl;
        }
    }
    
    // Mostrar historial de compras
    cout << "\n" << BLUE << "HISTORIAL DE COMPRAS:" << RESET << endl;
    cout << BLUE << "=====================" << RESET << endl;
    
    if (user.historialCompras.empty()) {
        cout << YELLOW << "No has realizado compras." << RESET << endl;
    } else {
        cout << "Total gastado: $" << fixed << setprecision(2) << user.totalGastado << endl;
        for (const auto& compra : user.historialCompras) {
            cout << "- " << compra.juego << " - $" << fixed << setprecision(2) << compra.precio;
            cout << " (" << compra.fecha << ")" << endl;
            cout << "  Transaccion: " << compra.numeroTransaccion << endl;
        }
    }
    
    cout << "\nPresione cualquier tecla para continuar...";
    getch();
    system("cls");
}

void mostrarRankingGlobal() {
    system("cls");
    cout << CYAN << "========================================" << RESET << endl;
    cout << YELLOW << "       RANKING GLOBAL DE JUGADORES     " << RESET << endl;
    cout << CYAN << "========================================" << RESET << endl;
    
    if (usuarios.empty()) {
        cout << YELLOW << "No hay jugadores registrados." << RESET << endl;
        cout << "\nPresione cualquier tecla para continuar...";
        getch();
        system("cls");
        return;
    }
    
    // Crear una copia del vector para ordenar
    vector<Usuario> ranking = usuarios;
    
    // Ordenar por puntuacion total (descendente)
    sort(ranking.begin(), ranking.end(), [](const Usuario& a, const Usuario& b) {
        return a.puntuacionTotal > b.puntuacionTotal;
    });
    
    cout << left << setw(5) << "POS" << setw(15) << "JUGADOR" << setw(12) << "PUNT.TOTAL" 
        << setw(10) << "PARTIDAS" << setw(10) << "PROMEDIO" << endl;
    cout << BLUE << "================================================================" << RESET << endl;
    
    for (size_t i = 0; i < ranking.size(); i++) {
        string posicion;
        if (i == 0) posicion = " 1";
        else if (i == 1) posicion = " 2";
        else if (i == 2) posicion = " 3";
        else posicion = to_string(i + 1);
        
        int promedio = ranking[i].totalPartidas > 0 ? ranking[i].puntuacionTotal / ranking[i].totalPartidas : 0;
        
        cout << left << setw(5) << posicion 
            << setw(15) << ranking[i].username
            << setw(12) << ranking[i].puntuacionTotal
            << setw(10) << ranking[i].totalPartidas
            << setw(10) << promedio << endl;
    }
    
    cout << "\n" << GREEN << "Presione cualquier tecla para ver records por juego..." << RESET;
    getch();
    
    // Mostrar records por juego
    system("cls");
    cout << CYAN << "========================================" << RESET << endl;
    cout << YELLOW << "       RECORDS POR JUEGO               " << RESET << endl;
    cout << CYAN << "========================================" << RESET << endl;
    
    vector<string> juegos = {"Juego de Snake", "Juego de Sudoku", "Juego de Cuatro en Raya", "Juego de Buscaminas", "Juego del Ahorcado"};
    
    for (const string& juego : juegos) {
        cout << "\n" << MAGENTA << "- " << juego << ":" << RESET << endl;
        cout << BLUE << "=================" << RESET << endl;
        
        vector<pair<string, int>> recordsJuego;
        
        for (const auto& user : usuarios) {
            if (user.puntuaciones.count(juego)) {
                recordsJuego.push_back({user.username, user.puntuaciones.at(juego)});
            }
        }
        
        if (recordsJuego.empty()) {
            cout << YELLOW << "No hay records para este juego." << RESET << endl;
        } else {
            sort(recordsJuego.begin(), recordsJuego.end(), 
                 [](const pair<string, int>& a, const pair<string, int>& b) {
                     return a.second > b.second;
                 });
            
            for (size_t i = 0; i < min(recordsJuego.size(), size_t(5)); i++) {
                string medalla = "";
                if (i == 0) medalla = "1. ";
                else if (i == 1) medalla = "2. ";
                else if (i == 2) medalla = "3. ";
                else medalla = to_string(i+1) + ". ";
                
                cout << medalla << left << setw(15) << recordsJuego[i].first 
                     << GREEN << recordsJuego[i].second << " puntos" << RESET << endl;
            }
        }
    }
    
    cout << "\nPresione cualquier tecla para continuar...";
    getch();
    system("cls");
}

void mostrarMenu() {
    cout << BLUE << "======================================" << RESET << endl;
    cout << YELLOW << "       PLATAFORMA DE JUEGOS          " << RESET << endl;
    cout << BLUE << "======================================" << RESET << endl;
    
    // Seccion de Juegos
    cout << CYAN << "-- JUEGOS --" << RESET << endl;
    cout << BLUE << "1. Tienda de Juegos" << RESET << endl;
    cout << BLUE << "2. Biblioteca de Juegos" << RESET << endl;
    
    // Seccion Social
    cout << CYAN << "-- SOCIAL --" << RESET << endl;
    cout << MAGENTA << "3. Mis Estadisticas" << RESET << endl;
    cout << MAGENTA << "4. Ranking Global" << RESET << endl;
    cout << MAGENTA << "5. Personalizar Perfil" << RESET << endl;
    cout << MAGENTA << "6. Gestionar Amigos" << RESET << endl;
    
    //Seccion Coins
    cout << CYAN << "-- COINS --" << RESET << endl;
    cout << GREEN << "7. Comprar Coins" << RESET << endl;
    cout << GREEN << "8. Ver Saldo e Historial" << RESET << endl;

    // Seccion Sistema
    cout << CYAN << "-- SISTEMA --" << RESET << endl;
    cout << YELLOW << "9. Ver Usuarios Registrados" << RESET << endl;
    cout << YELLOW << "10. Limpiar Archivo Corrupto" << RESET << endl;
    cout << YELLOW << "11. Cerrar Sesion" << RESET << endl;
    cout << BLUE << "======================================" << RESET << endl;

    if (usuarioActualIndex != -1) {
        Usuario& currentUser = usuarios[usuarioActualIndex];
        cout << GREEN << "Usuario: " << currentUser.avatar << " " << currentUser.colorPreferido << currentUser.username << RESET << endl;
        cout << CYAN << "Estado: " << currentUser.estado << RESET << endl;
        cout << CYAN << "Partidas: " << currentUser.totalPartidas 
             << " | Puntos: " << currentUser.puntuacionTotal;
             
        // Mostrar informacion de amigos
        if (!currentUser.amigos.empty()) {
            cout << " | Amigos: " << currentUser.amigos.size();
        }
        
        cout << RESET << endl;
        
        // Notificar solicitudes pendientes
        if (!currentUser.solicitudes.empty()) {
            cout << YELLOW << "Tienes " << currentUser.solicitudes.size() 
                 << " solicitud(es) de amistad pendiente(s)!" << RESET << endl;
        }
    } else {
        cout << YELLOW << "No hay usuario logueado" << RESET << endl;
    }

    cout << "Seleccione una opcion: ";
}

void mostrarMenuInicial() {
    cout << BLUE << "======================================" << RESET << endl;
    cout << YELLOW << "       PLATAFORMA DE JUEGOS          " << RESET << endl;
    cout << BLUE << "======================================" << RESET << endl;
    
    // Seccion de Usuario
    cout << CYAN << "-- USUARIO --" << RESET << endl;
    cout << GREEN << "1. Registrarse" << RESET << endl;
    cout << GREEN << "2. Loguearse" << RESET << endl;
    cout << RED << "3. Salir" << RESET << endl;
    cout << CYAN << "Seleccione una opcion: " << RESET << endl;
}

// Funcion para guardar usuarios en archivo de texto
void guardarUsuariosEnArchivo() {
    ofstream archivo("usuarios.txt");
    if (!archivo) {
        cerr << RED << "Error al abrir el archivo para guardar usuarios." << RESET << endl;
        return;
    }

    for (const auto& user : usuarios) {
        archivo << user.username << ";" << user.password << ";" 
                << user.totalPartidas << ";" << user.puntuacionTotal << ";";

        // Guardar juegos
        bool first = true;
        for (const auto& game : user.games) {
            if (!first) archivo << ",";
            archivo << game;
            first = false;
        }
        archivo << ";";

        // Guardar puntuaciones
        first = true;
        for (const auto& punt : user.puntuaciones) {
            if (!first) archivo << ",";
            archivo << punt.first << ":" << punt.second;
            first = false;
        }
        archivo << ";";
        
        // Guardar campos de personalizacion
        archivo << user.avatar << ";" << user.estado << ";" << user.colorPreferido << ";" << user.biografia << ";";
        
        // Guardar amigos
        first = true;
        for (const auto& amigo : user.amigos) {
            if (!first) archivo << ",";
            archivo << amigo;
            first = false;
        }
        archivo << ";";
        
        // Guardar solicitudes
        first = true;
        for (const auto& solicitud : user.solicitudes) {
            if (!first) archivo << ",";
            archivo << solicitud;
            first = false;
        }
        archivo << ";";
        
        // Guardar bloqueados
        first = true;
        for (const auto& bloqueado : user.bloqueados) {
            if (!first) archivo << ",";
            archivo << bloqueado;
            first = false;
        }
        archivo << ";";
        
        // Guardar total gastado
        archivo << user.totalGastado << ";";
        
        // Guardar historial de compras
        first = true;
        for (const auto& compra : user.historialCompras) {
            if (!first) archivo << ",";
            archivo << compra.juego << ":" << compra.precio << ":" << compra.fecha << ":" << compra.numeroTransaccion;
            first = false;
        }
        
        archivo << endl;
    }
    archivo.close();
    cout << GREEN << "Datos guardados exitosamente." << RESET << endl;
}

// Funcion para cargar usuarios desde archivo de texto
void cargarUsuariosDesdeArchivo() {
    ifstream archivo("usuarios.txt");
    if (!archivo) {
        cout << YELLOW << "No se encontro archivo de usuarios. Se creara uno nuevo." << RESET << endl;
        return;
    }

    usuarios.clear();
    string linea;

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ';')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 4) {
            cout << RED << "Error: Formato de archivo incorrecto." << RESET << endl;
            continue;
        }

        Usuario user;
        user.username = tokens[0];
        user.password = tokens[1];
        user.totalPartidas = stringToIntSafe(tokens[2]);
        user.puntuacionTotal = stringToIntSafe(tokens[3]);

        // Cargar juegos
        if (tokens.size() > 4 && !tokens[4].empty()) {
            stringstream ssGames(tokens[4]);
            string game;
            while (getline(ssGames, game, ',')) {
                user.games.push_back(game);
            }
        }

        // Cargar puntuaciones
        if (tokens.size() > 5 && !tokens[5].empty()) {
            stringstream ssPuntuaciones(tokens[5]);
            string puntuacion;
            while (getline(ssPuntuaciones, puntuacion, ',')) {
                size_t pos = puntuacion.find(':');
                if (pos != string::npos) {
                    string juego = puntuacion.substr(0, pos);
                    int valor = stringToIntSafe(puntuacion.substr(pos + 1));
                    user.puntuaciones[juego] = valor;
                }
            }
        }
        
        // Cargar campos de personalizacion
        if (tokens.size() > 6) user.avatar = tokens[6];
        if (tokens.size() > 7) user.estado = tokens[7];
        if (tokens.size() > 8) user.colorPreferido = tokens[8];
        if (tokens.size() > 9) user.biografia = tokens[9];
        
        // Cargar amigos
        if (tokens.size() > 10 && !tokens[10].empty()) {
            stringstream ssAmigos(tokens[10]);
            string amigo;
            while (getline(ssAmigos, amigo, ',')) {
                user.amigos.push_back(amigo);
            }
        }
        
        // Cargar solicitudes
        if (tokens.size() > 11 && !tokens[11].empty()) {
            stringstream ssSolicitudes(tokens[11]);
            string solicitud;
            while (getline(ssSolicitudes, solicitud, ',')) {
                user.solicitudes.push_back(solicitud);
            }
        }
        
        // Cargar bloqueados
        if (tokens.size() > 12 && !tokens[12].empty()) {
            stringstream ssBloqueados(tokens[12]);
            string bloqueado;
            while (getline(ssBloqueados, bloqueado, ',')) {
                user.bloqueados.push_back(bloqueado);
            }
        }
        
        // Cargar total gastado
        if (tokens.size() > 13) {
            try {
                user.totalGastado = stod(tokens[13]);
            } catch (...) {
                user.totalGastado = 0.0;
            }
        }
        
        // Cargar historial de compras
        if (tokens.size() > 14 && !tokens[14].empty()) {
            stringstream ssCompras(tokens[14]);
            string compraStr;
            while (getline(ssCompras, compraStr, ',')) {
                stringstream ssCompra(compraStr);
                string compraToken;
                vector<string> compraTokens;
                
                while (getline(ssCompra, compraToken, ':')) {
                    compraTokens.push_back(compraToken);
                }
                
                if (compraTokens.size() >= 4) {
                    Compra compra;
                    compra.juego = compraTokens[0];
                    try {
                        compra.precio = stod(compraTokens[1]);
                    } catch (...) {
                        compra.precio = 0.0;
                    }
                    compra.fecha = compraTokens[2];
                    compra.numeroTransaccion = compraTokens[3];
                    user.historialCompras.push_back(compra);
                }
            }
        }

        usuarios.push_back(user);
    }

    archivo.close();
    cout << GREEN << "Usuarios cargados exitosamente: " << usuarios.size() << " usuarios." << RESET << endl;
}

// Funcion para eliminar la cuenta actual
void eliminarCuentaActual() {
    system("cls");

    if (usuarioActualIndex == -1) {
        cout << YELLOW << "No hay ninguna sesion activa." << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        system("cls");
        return;
    }

    string nombre = usuarios[usuarioActualIndex].username;

    cout << RED << "Estas a punto de eliminar tu cuenta: " << nombre << RESET << endl;
    cout << "Esta accion no se puede deshacer." << endl;
    cout << "Estas seguro de que deseas continuar? (s/n): ";

    char confirmacion;
    cin >> confirmacion;

    if (confirmacion == 's' || confirmacion == 'S') {
        usuarios.erase(usuarios.begin() + usuarioActualIndex);
        usuarioActualIndex = -1;
        guardarUsuariosEnArchivo();

        cout << GREEN << "La cuenta ha sido eliminada exitosamente." << RESET << endl;
    } else {
        cout << YELLOW << "Operacion cancelada. Tu cuenta no fue eliminada." << RESET << endl;
    }

    cout << "Presione cualquier tecla para continuar...";
    getch();
    system("cls");
}

// Funcion para registrar un nuevo usuario
void registrarUsuario() {
    system("cls");

    // Verificar si ya hay un usuario logueado
    if (usuarioActualIndex != -1) {
        cout << YELLOW << "Ya hay un usuario logueado: " << usuarios[usuarioActualIndex].username << RESET << endl;
        cout << "Debe cerrar sesion antes de registrar un nuevo usuario." << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        system("cls");
        return;
    }
    cout << BLUE << "========================================" << RESET << endl;
    cout << YELLOW << "       REGISTRO DE NUEVO USUARIO       " << RESET << endl;
    cout << BLUE << "========================================" << RESET << endl;

    string username, password;
    cout << "Ingrese nombre de usuario: ";
    cin >> username;

    // Verificar si el usuario ya existe
    for (const auto& user : usuarios) {
        if (user.username == username) {
            cout << RED << "Error: El nombre de usuario ya esta en uso." << RESET << endl;
            cout << "Presione cualquier tecla para continuar...";
            getch();
            return;
        }
    }

    cout << "Ingrese contrasena: ";
    cin >> password;

    // Crear nuevo usuario
    Usuario nuevoUsuario(username, password);
    usuarios.push_back(nuevoUsuario);
    guardarUsuariosEnArchivo();

    cout << GREEN << "Usuario registrado exitosamente!" << RESET << endl;
    cout << "Presione cualquier tecla para continuar...";
    getch();
}

// Funcion para ver la lista de usuarios registrados
void mostrarUsuariosRegistrados() {
    system("cls");
    cout << BLUE << "========================================" << RESET << endl;
    cout << YELLOW << "       USUARIOS REGISTRADOS            " << RESET << endl;
    cout << BLUE << "========================================" << RESET << endl;

    if (usuarios.empty()) {
        cout << YELLOW << "No hay usuarios registrados." << RESET << endl;
    } else {
        cout << "Total de usuarios: " << usuarios.size() << endl << endl;
        for (const auto& user : usuarios) {
            cout << CYAN << "Usuario: " << user.username << RESET << endl;
            cout << "Partidas jugadas: " << user.totalPartidas << endl;
            cout << "Puntuacion total: " << user.puntuacionTotal << endl;
            cout << "Juegos: " << user.games.size() << endl;
            cout << "Amigos: " << user.amigos.size() << endl;
            cout << "Total gastado: $" << fixed << setprecision(2) << user.totalGastado << endl;
            cout << "----------------------------------------" << endl;
        }
    }

    cout << "Presione cualquier tecla para continuar...";
    getch();
}

// Funcion para iniciar sesion
void iniciarSesion() {
    system("cls");
    cout << BLUE << "========================================" << RESET << endl;
    cout << YELLOW << "           INICIAR SESION              " << RESET << endl;
    cout << BLUE << "========================================" << RESET << endl;

    string username, password;
    cout << "Ingrese nombre de usuario: ";
    cin >> username;
    cout << "Ingrese contrasena: ";
    cin >> password;

    for (size_t i = 0; i < usuarios.size(); i++) {
        if (usuarios[i].username == username && usuarios[i].password == password) {
            usuarioActualIndex = i;
            cout << GREEN << "Sesion iniciada exitosamente!" << RESET << endl;
            cout << "Bienvenido, " << usuarios[i].username << "!" << endl;
            cout << "Presione cualquier tecla para continuar...";
            getch();
            return;
        }
    }

    cout << RED << "Error: Usuario o contrasena incorrectos." << RESET << endl;
    cout << "Presione cualquier tecla para continuar...";
    getch();
}

// Funcion para cerrar sesion
void cerrarSesion() {
    if (usuarioActualIndex != -1) {
        cout << GREEN << "Sesion cerrada exitosamente." << RESET << endl;
        usuarioActualIndex = -1;
    } else {
        cout << YELLOW << "No hay sesion activa." << RESET << endl;
    }
    cout << "Presione cualquier tecla para continuar...";
    getch();
}

/*
    Juego de Cuatro en Raya
*/
// Definicion de constantes
const int FILAS = 6;
const int COLUMNAS = 7;
char tablero[FILAS][COLUMNAS];

// Funcion para inicializar el tablero
void inicializarTablero4EnRaya() {
    for (int i = 0; i < FILAS; ++i)
        for (int j = 0; j < COLUMNAS; ++j)
            tablero[i][j] = ' ';
}

// Funcion para imprimir el tablero
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
    cout << "\n\n";
    cout << "=============================================================";
    cout<< "\nJugador: " << playerName << endl;
    
    // Mostrar record personal si existe
    if (usuarioActualIndex != -1) {
        if (usuarios[usuarioActualIndex].puntuaciones.count("Juego de Cuatro en Raya")) {
            cout << "Tu record: " << usuarios[usuarioActualIndex].puntuaciones["Juego de Cuatro en Raya"] << " victorias" << endl;
        } else {
            cout << "Aun no tienes record en este juego" << endl;
        }
    }
    cout << "============================================================="<<endl;
}

// Funcion para insertar una ficha en el tablero
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

// Funcion para verificar si hay un ganador
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

// Funcion para la computadora, SOLO elige una columna aleatoria
int elegirColumnaComputadora() {
    srand(time(0));  // Inicializa la semilla aleatoria
    int columna;
    do {
        columna = rand() % COLUMNAS;  // Elige una columna aleatoria
        // Verifica si la columna esta disponible
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

// Funcion para jugar Cuatro en Raya
void playCuatroEnRaya() {
    if (usuarioActualIndex == -1) {
        cout << RED << "Debe iniciar sesion primero." << RESET << endl;
        getch();
        return;
    }
    
    system("cls");
    inicializarTablero4EnRaya();
    char jugador = 'X';
    bool juegoTerminado = false;
    playerName = usuarios[usuarioActualIndex].username;
    int victorias = 0;

    // Obtener record actual si existe
    if (usuarios[usuarioActualIndex].puntuaciones.count("Juego de Cuatro en Raya")) {
        victorias = usuarios[usuarioActualIndex].puntuaciones["Juego de Cuatro en Raya"];
    }

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
                cout << "El jugador " << jugador << " ha ganado!" << endl;
                
                // Actualizar puntuacion solo si el jugador humano gana
                if (jugador == 'X') {
                    victorias++;
                    cout << "\nVictorias: " << victorias << endl;
                    
                    // Actualizar record en el usuario
                    actualizarRecord(usuarios[usuarioActualIndex], "Juego de Cuatro en Raya", victorias);
                    guardarUsuariosEnArchivo();
                }
                
                juegoTerminado = true;
            } else if (tableroLleno()) {
                imprimirTablero4enRaya();
                cout << "Es un empate!" << endl;
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
    Juego del Ahorcado
*/

// Categorías y palabras para el ahorcado
map<string, vector<string>> palabrasPorCategoria = {
    {"Animales", {"ELEFANTE", "JIRAFA", "TIGRE", "LEON", "CANGURO", "DELFIN", 
	"TORTUGA", "COCODRILO", "OSO", "KOALA", "ARMADILLO", "NUTRIA", "WOMBAT", 
	"CAPIBARA", "TAPIR", "MARMOTA", "ERIZO", "MURCIELAGO", "ALCE", "PERRO", 
	"BALLENA", "VENADO", "ORANGUTAN", "GORILA", "CHIMPANCE", "HUMANO", "RINOCERONTE", 
	"MANDRIL", "ZARIGUELLA", "ZURICATA", "GATO", "OTORONGO", "GUEPARDO", "MARMOTA", "CASTOR", 
	"PUMA", "COLIBRI", "PINGÜINO", "AVESTRUZ", "CACATUA", "GUACAMAYO",
    "KIWI", "PELICANO", "FLAMENCO", "AGUILA", "BUHO",
    "IGUANA", "CAMALEON", "SERPIENTE", "SAPO", "SALAMANDRA",
    "MANATI", "NARVAL", "ORCA", "FOCA", "PULPO", "PEZLUNA", 
	"PEZGATO", "PEZGLOBO", "MERLUZA", "ESTRELLAMAR",
    "CABALLITOMAR", "MEDUSA", "ANGUILLA", "PEZESPADA", "PEZPIEDRA",
    "MANTISRELIGIOSA", "ESCORPION", "CIEMPIES", "MARIQUITA", "LIBELULA",
    "SALTAMONTES", "LUCIERNAGA", "HORMIGALEON", "ARAÑACAMELLO", "MILPIES"}},
    {"Paises", {
    "ARGENTINA", "BRASIL", "CANADA", "ESPAÑA", "JAPON", "ITALIA", "ALEMANIA", "AUSTRALIA",
    "PERU", "CHILE", "COLOMBIA", "MEXICO", "VENEZUELA", "ECUADOR", "URUGUAY", "PARAGUAY",
    "BOLIVIA", "PANAMA", "CUBA", "NICARAGUA", "GUATEMALA", "HONDURAS", "EL SALVADOR",
    "ESTADOS UNIDOS", "REINO UNIDO", "PORTUGAL", "FRANCIA", "SUIZA", "BELGICA", "PAISES BAJOS",
    "NORUEGA", "SUECIA", "DINAMARCA", "FINLANDIA", "RUSIA", "CHINA", "COREA DEL SUR", "INDIA",
    "PAKISTAN", "IRAN", "ISRAEL", "ARABIA SAUDITA", "SUDAFRICA", "NIGERIA", "EGIPTO", "KENIA",
    "MARRUECOS", "TUNEZ", "TURQUIA", "GRECIA", "POLONIA", "UCRANIA", "CHEQUIA", "HUNGRIA",
    "SERBIA", "CROACIA", "VIETNAM", "TAILANDIA", "MALASIA", "FILIPINAS", "INDONESIA",
    "NUEVA ZELANDA"}},
    {"Frutas", {
    "MANZANA", "BANANA", "NARANJA", "FRESA", "SANDIA", "MELON", "KIWI", "PINA",
    "PERA", "CIRUELA", "MANGO", "PAPAYA", "GUAYABA", "GRANADA", "LIMA", "LIMON",
    "MARACUYA", "COCO", "HIGO", "TAMARINDO", "FRAMBUESA", "MORA", "ARANDANO",
    "MANDARINA", "CARAMBOLA", "CHIRIMOYA", "DURAZNO", "NECTARINA", "PALTA", "PITAJAYA"
	}},
    {"Deportes", {
    "FUTBOL", "BALONCESTO", "TENIS", "NATACION", "CICLISMO", "VOLEIBOL", "BEISBOL", "ATLETISMO",
    "GIMNASIA", "GOLF", "RUGBY", "BOXEO", "ESGRIMA", "JUDO", "TAEKWONDO", "KARATE",
    "SKATEBOARDING", "SURF", "ESCALADA", "HOCKEY", "PATINAJE", "REMO", "BILLAR",
    "PESAS", "LUCHA", "BADMINTON", "PINGPONG", "TIROCONARCO", "ESQUI", "TRIATLON", "HALTEROFILIA"
}},
    {"Profesiones", {
    "MEDICO", "INGENIERO", "PROFESOR", "ARQUITECTO", "PROGRAMADOR", "BOMBERO", "POLICIA", "CHEF",
    "ABOGADO", "ENFERMERO", "ELECTRICISTA", "CARPINTERO", "MECANICO", "FOTOGRAFO", "PANADERO",
    "FARMACEUTICO", "PINTOR", "DISEÑADOR", "CONTADOR", "VETERINARIO", "ACTOR", "MUSICO",
    "CAMARERO", "PILOTO", "ESCRITOR", "BARBERO", "REPORTERO", "PSICOLOGO", "GUARDAESPALDAS"
}},
};

void mostrarMenuAhorcado() {
    cout << "\n\n";
    cout << "======================================\n";
    cout << "           JUEGO DEL AHORCADO         \n";
    cout << "======================================\n\n";

    cout << "Bienvenido al juego del Ahorcado!\n\n";

    // Si hay un usuario logueado, usamos su nombre
    if (usuarioActualIndex != -1) {
        playerName = usuarios[usuarioActualIndex].username;
        cout << "Jugador: " << playerName << "\n\n";
    } else {
        cout << "Por favor, inicia sesion primero" << endl;
        cout << "Presiona cualquier tecla para volver...";
        getch();
        return;
    }

    cout << "Hola " << playerName << "! Estas son las reglas del juego:\n\n";
    cout << "1. Se te presentara una palabra oculta de una categoría especifica.\n";
    cout << "2. Debes adivinar las letras que componen la palabra.\n";
    cout << "3. Tienes 6 intentos para adivinar la palabra.\n";
    cout << "4. Cada letra correcta revelara su posicion en la palabra.\n";
    cout << "5. Cada letra incorrecta reducira tus intentos restantes.\n";
    cout << "6. Si adivinas la palabra antes de quedarte sin intentos, ganas.\n";
    cout << "7. Cada palabra adivinada suma 100 puntos.\n";
    
    // Mostrar record personal si existe
    if (usuarioActualIndex != -1 && usuarios[usuarioActualIndex].puntuaciones.count("Juego del Ahorcado")) {
        cout << "\nTu record actual: " << usuarios[usuarioActualIndex].puntuaciones["Juego del Ahorcado"] << " puntos" << endl;
    } else {
        cout << "\nAun no tienes record en este juego" << endl;
    }
    
    cout << "\nPresiona cualquier tecla para comenzar...";
    getch();
}

void dibujarAhorcado(int intentosRestantes) {
    cout << endl;
    cout << "Intentos restantes: " << intentosRestantes << endl;
    cout << "  _______" << endl;
    cout << "  |     |" << endl;
    cout << "  |     " << (intentosRestantes < 6 ? "O" : " ") << endl;
    cout << "  |    " << (intentosRestantes < 4 ? "/" : " ") 
                      << (intentosRestantes < 5 ? "|" : " ") 
                      << (intentosRestantes < 3 ? "\\" : " ") << endl;
    cout << "  |    " << (intentosRestantes < 2 ? "/" : " ") 
                      << " " 
                      << (intentosRestantes < 1 ? "\\" : " ") << endl;
    cout << " _|_" << endl;
    cout << endl;
}

void playAhorcado() {
    if (usuarioActualIndex == -1) {
        cout << RED << "Debe iniciar sesion primero." << RESET << endl;
        getch();
        return;
    }
    
    system("cls");
    mostrarMenuAhorcado();
    
    bool jugarOtraVez = true;
    int puntuacionTotal = 0;
    
    if (usuarioActualIndex != -1 && usuarios[usuarioActualIndex].puntuaciones.count("Juego del Ahorcado")) {
        puntuacionTotal = usuarios[usuarioActualIndex].puntuaciones["Juego del Ahorcado"];
    }
    
    while (jugarOtraVez) {
        system("cls");
        
        // Seleccionar categoría
        cout << "Selecciona una categoria:\n";
        int i = 1;
        for (const auto& categoria : palabrasPorCategoria) {
            cout << i << ". " << categoria.first << endl;
            i++;
        }
        
        int opcionCategoria;
        cout << "\nIngresa el número de la categoría: ";
        cin >> opcionCategoria;
        
        if (opcionCategoria < 1 || static_cast<size_t>(opcionCategoria) > palabrasPorCategoria.size()) {
            cout << RED << "Opción inválida. Seleccionando categoría aleatoria..." << RESET << endl;
            opcionCategoria = rand() % palabrasPorCategoria.size() + 1;
            Sleep(2000);
        }
        
        auto it = palabrasPorCategoria.begin();
        advance(it, opcionCategoria - 1);
        string categoriaSeleccionada = it->first;
        vector<string> palabras = it->second;
        
        // Seleccionar palabra aleatoria
        srand(time(0));
        string palabra = palabras[rand() % palabras.size()];
        string palabraAdivinada(palabra.length(), '_');
        
        int intentosRestantes = 6;
        vector<char> letrasIntentadas;
        bool palabraAdivinadaCompleta = false;
        
        while (intentosRestantes > 0 && !palabraAdivinadaCompleta) {
            system("cls");
            cout << "Categoría: " << categoriaSeleccionada << endl;
            dibujarAhorcado(intentosRestantes);
            
            cout << "Palabra: ";
            for (char c : palabraAdivinada) {
                cout << c << " ";
            }
            cout << endl << endl;
            
            cout << "Letras intentadas: ";
            for (char c : letrasIntentadas) {
                cout << c << " ";
            }
            cout << endl << endl;
            
            cout << "Ingresa una letra: ";
            char letra;
            cin >> letra;
            letra = toupper(letra);
            
            // Verificar si la letra ya fue intentada
            if (find(letrasIntentadas.begin(), letrasIntentadas.end(), letra) != letrasIntentadas.end()) {
                cout << YELLOW << "Ya intentaste esa letra. Intenta con otra." << RESET << endl;
                Sleep(1000);
                continue;
            }
            
            letrasIntentadas.push_back(letra);
            
            // Verificar si la letra está en la palabra
            bool letraAdivinada = false;
            for (size_t i = 0; i < palabra.length(); i++) {
                if (palabra[i] == letra) {
                    palabraAdivinada[i] = letra;
                    letraAdivinada = true;
                }
            }
            
            if (!letraAdivinada) {
                intentosRestantes--;
                cout << RED << "La letra " << letra << " no está en la palabra." << RESET << endl;
            } else {
                cout << GREEN << "¡Correcto! La letra " << letra << " está en la palabra." << RESET << endl;
            }
            
            // Verificar si se adivinó toda la palabra
            if (palabraAdivinada == palabra) {
                palabraAdivinadaCompleta = true;
            }
            
            Sleep(1000);
        }
        
        system("cls");
        if (palabraAdivinadaCompleta) {
            puntuacionTotal += 100;
            cout << GREEN << "¡Felicidades! Adivinaste la palabra: " << palabra << RESET << endl;
            cout << GREEN << "Ganaste 100 puntos. Puntuación total: " << puntuacionTotal << RESET << endl;
        } else {
            cout << RED << "¡Oh no! Te quedaste sin intentos." << RESET << endl;
            cout << "La palabra era: " << palabra << endl;
        }
        
        dibujarAhorcado(intentosRestantes);
        
        // Preguntar si quiere jugar otra vez
        cout << "\n¿Quieres jugar otra vez? (s/n): ";
        char respuesta;
        cin >> respuesta;
        
        if (tolower(respuesta) != 's') {
            jugarOtraVez = false;
        }
    }
    
    // Actualizar record en el usuario
    if (usuarioActualIndex != -1) {
        actualizarRecord(usuarios[usuarioActualIndex], "Juego del Ahorcado", puntuacionTotal);
        guardarUsuariosEnArchivo();
    }
    
    cout << "\nPuntuacion final: " << puntuacionTotal << " puntos." << endl;
    cout << "Presiona cualquier tecla para volver al menu...";
    getch();
    system("cls");
}


/*
    Juego de Snake 
*/

#include <windows.h>
#include <iostream>
#include <vector>
using namespace std;

// Dimensiones del tablero
const int WIDTH = 20;
const int HEIGHT = 20;

// Posicion de la fruta
int fruitX, fruitY;

// Buffer para el tablero
char gameBoard[HEIGHT][WIDTH + 1];

// Función para posicionar el cursor en una coordenada específica
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Función para ocultar el cursor
void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// Funcion para generar una nueva fruta en una posicion aleatoria
void GenerateFruit() {
    fruitX = rand() % (WIDTH - 2) + 1;
    fruitY = rand() % (HEIGHT - 2) + 1;
}

// Función para inicializar el tablero base
void InitializeBoard() {
    // Inicializar todo el tablero con espacios
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                gameBoard[y][x] = '#'; // Bordes
            } else {
                gameBoard[y][x] = ' '; // Espacios vacíos
            }
        }
        gameBoard[y][WIDTH] = '\0'; // Terminador de cadena
    }
}

// Función para actualizar solo las posiciones que cambiaron
void UpdateBoard(const vector<pair<int, int>>& snake, const vector<pair<int, int>>& prevSnake, int score) {
    // Limpiar posiciones anteriores de la serpiente (solo si cambió)
    if (!prevSnake.empty()) {
        // Limpiar la cola anterior si la serpiente no creció
        if (prevSnake.size() >= snake.size()) {
            auto lastTail = prevSnake.back();
            gameBoard[lastTail.second][lastTail.first] = ' ';
            gotoxy(lastTail.first, lastTail.second);
            cout << ' ';
        }
    }
    
    // Dibujar la nueva serpiente (solo las partes nuevas)
    for (size_t i = 0; i < snake.size(); i++) {
        int x = snake[i].first;
        int y = snake[i].second;
        
        // Solo actualizar si la posición cambió
        bool needUpdate = true;
        if (!prevSnake.empty() && i < prevSnake.size()) {
            if (prevSnake[i].first == x && prevSnake[i].second == y) {
                needUpdate = false;
            }
        }
        
        if (needUpdate) {
            char symbol = (i == 0) ? 'O' : 'o'; // Cabeza 'O', cuerpo 'o'
            gameBoard[y][x] = symbol;
            gotoxy(x, y);
            cout << symbol;
        }
    }
    
    // Dibujar la fruta
    gameBoard[fruitY][fruitX] = 'F';
    gotoxy(fruitX, fruitY);
    cout << 'F';
    
    // Actualizar información del jugador (en la parte inferior)
    gotoxy(0, HEIGHT + 1);
    cout << "Jugador: " << playerName << " | Puntaje: " << score << "        ";
    
    // Mostrar record personal si existe
    gotoxy(0, HEIGHT + 2);
    if (usuarioActualIndex != -1) {
        if (usuarios[usuarioActualIndex].puntuaciones.count("Juego de Snake")) {
            cout << "Tu record: " << usuarios[usuarioActualIndex].puntuaciones["Juego de Snake"] << " puntos        ";
        } else {
            cout << "Aun no tienes record en este juego        ";
        }
    }
}

// Funcion para dibujar el tablero completo (solo la primera vez)
void DrawBoard(const vector<pair<int, int>>& snake, int score) {
    system("cls");
    hideCursor();
    
    // Dibujar el tablero completo
    for (int y = 0; y < HEIGHT; y++) {
        // Inicializar la fila
        for (int x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                gameBoard[y][x] = '#'; // Bordes
            } else {
                gameBoard[y][x] = ' '; // Espacios vacíos
            }
        }
        gameBoard[y][WIDTH] = '\0';
    }
    
    // Colocar la serpiente
    for (size_t i = 0; i < snake.size(); i++) {
        int x = snake[i].first;
        int y = snake[i].second;
        gameBoard[y][x] = (i == 0) ? 'O' : 'o';
    }
    
    // Colocar la fruta
    gameBoard[fruitY][fruitX] = 'F';
    
    // Dibujar todo el tablero
    for (int y = 0; y < HEIGHT; y++) {
        gotoxy(0, y);
        cout << gameBoard[y];
    }
    
    // Mostrar información del jugador
    gotoxy(0, HEIGHT + 1);
    cout << "Jugador: " << playerName << " | Puntaje: " << score << endl;
    
    // Mostrar record personal si existe
    if (usuarioActualIndex != -1) {
        if (usuarios[usuarioActualIndex].puntuaciones.count("Juego de Snake")) {
            cout << "Tu record: " << usuarios[usuarioActualIndex].puntuaciones["Juego de Snake"] << " puntos" << endl;
        } else {
            cout << "Aun no tienes record en este juego" << endl;
        }
    }
}

// Funcion para mostrar el menu principal
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
        cout << "Por favor, inicia sesion primero" << endl;
        cout << "Presiona cualquier tecla para volver...";
        getch();
        return;
    }

    cout << "Hola " << playerName << "! Estas son las reglas del juego:\n\n";
    cout << "1. Controlaras una serpiente que se mueve por el tablero.\n";
    cout << "2. Usa las teclas W, A, S, D para moverte (arriba, izquierda, abajo, derecha).\n";
    cout << "3. Debes comer las frutas (representadas por 'F') para crecer.\n";
    cout << "4. Si chocas contra los bordes o contra ti mismo, pierdes.\n";
    cout << "5. El objetivo es hacer que la serpiente crezca lo mas posible.\n\n";
    
    // Mostrar record personal si existe
    if (usuarios[usuarioActualIndex].puntuaciones.count("Juego de Snake")) {
        cout << "Tu record actual: " << usuarios[usuarioActualIndex].puntuaciones["Juego de Snake"] << " puntos" << endl;
    } else {
        cout << "Aun no tienes record en este juego" << endl;
    }
    
    cout << "\nPresiona cualquier tecla para comenzar...";
    getch();
}

// Funcion principal del juego
void game() {
    const int DELAY = 150;
    int dx = 0, dy = 0;  // Direccion inicial: detenida
    int score = 0;
    bool paused = false;
    bool firstDraw = true;

    // Inicializa la serpiente en el centro del tablero
    vector<pair<int, int>> snake = {{WIDTH / 2, HEIGHT / 2}};
    vector<pair<int, int>> prevSnake; // Para almacenar la posición anterior
    GenerateFruit();  // Genera la primera fruta

    bool gameRunning = true;

    while (gameRunning) {
        // Si el juego esta pausado, solo revisamos si el usuario quiere despausar
        if (paused) {
            if (_kbhit()) {
                char key = _getch();
                if (key == 'p') {
                    paused = false;
                    firstDraw = true; // Redibujar completamente al despausar
                } else if (key == 'q') {
                    gameRunning = false;
                }
            }
            Sleep(100); // Reducir el uso de CPU mientras esta pausado
            continue;   // Salta el resto del ciclo mientras esta pausado
        }

        // Entrada de teclado
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'w': 
                    // No permitir ir hacia abajo si ya va hacia arriba
                    if (dy != 1) { dy = -1; dx = 0; }
                    break;
                case 's': 
                    // No permitir ir hacia arriba si ya va hacia abajo
                    if (dy != -1) { dy = 1; dx = 0; }
                    break;
                case 'a': 
                    // No permitir ir hacia la derecha si ya va hacia la izquierda
                    if (dx != 1) { dx = -1; dy = 0; }
                    break;
                case 'd': 
                    // No permitir ir hacia la izquierda si ya va hacia la derecha
                    if (dx != -1) { dx = 1; dy = 0; }
                    break;
                case 'q': gameRunning = false; break;
                case 'p':  // Pausar el juego
                    paused = true;
                    system("cls");
                    cout << "Juego Pausado! Presiona 'P' para continuar o 'Q' para salir..." << endl;
                    continue; // Salta al siguiente ciclo para procesar la pausa
            }
        }

        // Calcula la nueva posicion de la cabeza
        int newHeadX = snake[0].first + dx;
        int newHeadY = snake[0].second + dy;

        // Verifica colision los bordes
        if (newHeadX <= 0 || newHeadX >= WIDTH - 1 || newHeadY <= 0 || newHeadY >= HEIGHT - 1) {
            break; // Fin del juego
        }

        // Verifica colision consigo misma
        if (dx != 0 || dy != 0) {
            for (size_t i = 1; i < snake.size(); i++) {
                if (snake[i].first == newHeadX && snake[i].second == newHeadY) {
                    gameRunning = false;
                    break;
                }
            }
        }

        // Guardar estado anterior de la serpiente
        prevSnake = snake;

        // Verifica si comio la fruta
        bool grow = false;
        if (newHeadX == fruitX && newHeadY == fruitY) {
            score += 10;
            GenerateFruit();  // Nueva fruta
            grow = true;
        }

        // Mueve la serpiente
        snake.insert(snake.begin(), {newHeadX, newHeadY}); // Anade la nueva cabeza
        if (!grow) {
            snake.pop_back();  // Si no comio, elimina la cola
        }

        // Dibuja el tablero
        if (firstDraw) {
            DrawBoard(snake, score); // Dibujo completo la primera vez
            firstDraw = false;
        } else {
            UpdateBoard(snake, prevSnake, score); // Solo actualizar cambios
        }

        // Control de velocidad del juego
        Sleep(DELAY);
    }

    cout << "\nJuego terminado! Puntaje final: " << score << endl;
    
    // Actualizar record en el usuario
    if (usuarioActualIndex != -1) {
        actualizarRecord(usuarios[usuarioActualIndex], "Juego de Snake", score);
        guardarUsuariosEnArchivo();
    }
    
    getch();
}

void playSnake() {
    if (usuarioActualIndex == -1) {
        cout << RED << "Debe iniciar sesion primero." << RESET << endl;
        getch();
        return;
    }
    
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
    // Verificar fila
    for (int i = 0; i < 9; i++) {
        if (tablero[fila][i] == num) {
            return false;
        }
    }

    // Verificar columna
    for (int i = 0; i < 9; i++) {
        if (tablero[i][columna] == num) {
            return false;
        }
    }

    // Verificar cuadrante 3x3
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
    if (usuarioActualIndex == -1) {
        cout << RED << "Debe iniciar sesion primero." << RESET << endl;
        getch();
        return;
    }
    
    system("cls"); // Limpia la pantalla inicial
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
    playerName = usuarios[usuarioActualIndex].username;
    cout << "Jugador: " << playerName << endl;
    cout << YELLOW << "Presione 'Q' para salir" << RESET << endl << endl;

    // Mostrar record si existe
    if (usuarios[usuarioActualIndex].puntuaciones.count("Juego de Sudoku")) {
        cout << "Record actual: " << usuarios[usuarioActualIndex].puntuaciones["Juego de Sudoku"] << " sudokus completados" << endl;
    }

    while (!estaCompleto(tablero)) {
        system("cls"); // Limpia la pantalla en cada iteración
        cout << BLUE << "Sudoku - Jugador: " << playerName << RESET << endl;
        cout << YELLOW << "Presione 'Q' para salir" << RESET << endl << endl;
        
        imprimirTablero(tablero); // Imprime el tablero actualizado

        cout << "\nIngrese fila (0-8): ";
        string input;
        cin >> input;
        
        if (input == "Q" || input == "q") {
            cout << YELLOW << "\nPartida no completada. Saliendo..." << RESET << endl;
            getch();
            return;
        }

        int fila, columna, num;
        try {
            fila = stoi(input);
            cout << "Ingrese columna (0-8): ";
            cin >> input;
            columna = stoi(input);
            cout << "Ingrese numero (1-9): ";
            cin >> input;
            num = stoi(input);
        } catch (...) {
            cout << RED << "Entrada invalida. Intente de nuevo." << RESET << endl;
            getch();
            continue;
        }

        // Validar coordenadas
        if (fila < 0 || fila >= 9 || columna < 0 || columna >= 9) {
            cout << RED << "Coordenadas invalidas." << RESET << endl;
            getch();
            continue;
        }

        // Validar número
        if (num < 1 || num > 9) {
            cout << RED << "Numero debe ser entre 1-9." << RESET << endl;
            getch();
            continue;
        }

        // Verificar celda vacía
        if (tablero[fila][columna] != 0) {
            cout << RED << "Celda ocupada. Intente otra." << RESET << endl;
            getch();
            continue;
        }

        // Verificar movimiento válido
        if (!esValido(tablero, fila, columna, num)) {
            cout << RED << "Movimiento invalido (conflicto con fila/columna/cuadrante)." << RESET << endl;
            getch();
            continue;
        }

        tablero[fila][columna] = num; // Actualizar tablero
    }

    // Al completar el sudoku
    system("cls");
    cout << GREEN << "¡FELICIDADES! Sudoku completado." << RESET << endl;
    imprimirTablero(tablero);
    
    // Actualizar record
    int nuevosCompletados = 1;
    if (usuarios[usuarioActualIndex].puntuaciones.count("Juego de Sudoku")) {
        nuevosCompletados = usuarios[usuarioActualIndex].puntuaciones["Juego de Sudoku"] + 1;
    }
    actualizarRecord(usuarios[usuarioActualIndex], "Juego de Sudoku", nuevosCompletados);
    guardarUsuariosEnArchivo();
    
    cout << "\nSudokus completados: " << nuevosCompletados << endl;
    cout << "Presione cualquier tecla para continuar...";
    getch();
}

/*
    Juego Buscaminas
*/

// Constantes del juego
const int FILASMINAS = 5;
const int COLUMNASMINAS = 5;
const int MINAS = 5;

// Variables del juego
char tablerominas[FILASMINAS][COLUMNASMINAS];
bool descubierto[FILASMINAS][COLUMNASMINAS];
bool minas[FILASMINAS][COLUMNASMINAS];

void ShowMenuMinas() {
    cout << "\n\n";
    cout << "======================================\n";
    cout << "           JUEGO DE BUSCAMINAS        \n";
    cout << "======================================\n\n";

    cout << "Bienvenido al juego de Buscaminas!\n\n";

    // Si hay un usuario logueado, usamos su nombre
    if (usuarioActualIndex != -1) {
        playerName = usuarios[usuarioActualIndex].username;
        cout << "Jugador: " << playerName << "\n\n";
    } else {
        cout << "Por favor, inicia sesion primero" << endl;
        cout << "Presiona cualquier tecla para volver...";
        getch();
        return;
    }
    
    cout << "Hola " << playerName << "! Estas son las reglas del juego:\n\n";
    cout << "1. Revela las casillas digitando la fila y columna correspondiente.\n";
    cout << "2. Si la casilla revela un numero, indica las bombas proximas\n";
    cout << "3. Cada casilla revelada suma 10 puntos\n";
    cout << "4. Hay 5 bombas en total\n";
    cout << "5. El juego termina cuando descubres una casilla que contiene una bomba\n";
    
    // Mostrar record personal si existe
    if (usuarios[usuarioActualIndex].puntuaciones.count("Juego de Buscaminas")) {
        cout << "\nTu record actual: " << usuarios[usuarioActualIndex].puntuaciones["Juego de Buscaminas"] << " puntos" << endl;
    } else {
        cout << "\nAun no tienes record en este juego" << endl;
    }
    
    cout << "\nPresiona cualquier tecla para comenzar...";
    getch();
    system("cls");
}

void inicializarTablerominas() {
    for (int i = 0; i < FILASMINAS; i++) {
        for (int j = 0; j < COLUMNASMINAS; j++) {
            tablerominas[i][j] = '-';
            descubierto[i][j] = false;
            minas[i][j] = false;
        }
    }

    // Colocar minas aleatorias
    int colocadas = 0;
    while (colocadas < MINAS) {
        int f = rand() % FILASMINAS;
        int c = rand() % COLUMNASMINAS;
        if (!minas[f][c]) {
            minas[f][c] = true;
            colocadas++;
        }
    }
}

int contarMinasVecinas(int f, int c) {
    int totalminas = 0;
    for (int i = f - 1; i <= f + 1; i++) {
        for (int j = c - 1; j <= c + 1; j++) {
            if (i >= 0 && i < FILASMINAS && j >= 0 && j < COLUMNASMINAS) {
                if (minas[i][j]) totalminas++;
            }
        }
    }
    return totalminas;
}

void mostrarTablerominas() {
    cout << "   ";
    for (int j = 0; j < COLUMNASMINAS; j++) cout << j << " ";
    cout << endl;
    for (int i = 0; i < FILASMINAS; i++) {
        cout << i << "  ";
        for (int j = 0; j < COLUMNASMINAS; j++) {
            if (descubierto[i][j]) {
                cout << tablerominas[i][j] << " ";
            } else {
                cout << "#" << " ";
            }
        }
        cout << endl;
    }
}

bool revelar(int f, int c) {
    if (f < 0 || f >= FILASMINAS || c < 0 || c >= COLUMNASMINAS || descubierto[f][c]) return true;

    descubierto[f][c] = true;

    if (minas[f][c]) {
        return false; // Perdiste!
    }

    int minasCerca = contarMinasVecinas(f, c);
    tablerominas[f][c] = minasCerca + '0';

    if (minasCerca == 0) {
        // Revelar alrededor si no hay minas cercanas
        for (int i = f - 1; i <= f + 1; i++) {
            for (int j = c - 1; j <= c + 1; j++) {
                revelar(i, j);
            }
        }
    }

    return true;
}

bool juegoGanado() {
    int descubiertas = 0;
    for (int i = 0; i < FILASMINAS; i++) {
        for (int j = 0; j < COLUMNASMINAS; j++) {
            if (descubierto[i][j]) descubiertas++;
        }
    }
    return descubiertas == (FILASMINAS * COLUMNASMINAS - MINAS);
}

void gameminas() {
    srand(time(0));
    inicializarTablerominas();

    bool jugando = true;
    int puntuacion = 0;

    while (jugando) {
        mostrarTablerominas();
        cout << "Jugador: " << playerName << " | Puntuacion: " << puntuacion << endl;
        
        // Mostrar record personal si existe
        if (usuarios[usuarioActualIndex].puntuaciones.count("Juego de Buscaminas")) {
            cout << "Tu record: " << usuarios[usuarioActualIndex].puntuaciones["Juego de Buscaminas"] << " puntos" << endl;
        }
        
        int f, c;
        cout << "Ingresa fila (0-4): ";
        cin >> f;
        cout << "Ingresa columna (0-4): ";
        cin >> c;

        // Solo incrementar si la casilla no estaba descubierta
        if (!descubierto[f][c]) {
            if (!revelar(f, c)) {
                cout << "?? PISASTE UNA MINA! GAME OVER!\n";
                jugando = false;
            } else {
                puntuacion += 10;
                cout << "? Casilla segura. Puntuacion actual: " << puntuacion << " puntos.\n";
                if (juegoGanado()) {
                    cout << "?? FELICIDADES! GANASTE EL JUEGO\n";
                    jugando = false;
                }
            }
        } else {
            cout << "?? Esa casilla ya fue descubierta. Elige otra.\n";
        }
    }

    // Mostrar puntuacion final
    cout << "\n?? Puntuacion final: " << puntuacion << " puntos.\n";
    
    // Actualizar record en el usuario
    if (usuarioActualIndex != -1) {
        actualizarRecord(usuarios[usuarioActualIndex], "Juego de Buscaminas", puntuacion);
        guardarUsuariosEnArchivo();
    }

    // Mostrar todas las minas
    cout << "\nTablero final:\n";
    for (int i = 0; i < FILASMINAS; i++) {
        for (int j = 0; j < COLUMNASMINAS; j++) {
            if (minas[i][j]) cout << "* ";
            else cout << tablerominas[i][j] << " ";
        }
        cout << endl;
    }
}

void playminas() {
    if (usuarioActualIndex == -1) {
        cout << RED << "Debe iniciar sesion primero." << RESET << endl;
        getch();
        return;
    }
    
    system("cls");
    ShowMenuMinas();
    gameminas();
    cout << "Gracias por jugar, " << playerName << "!" << endl;
    cout << "Presiona cualquier tecla para salir...";
    getch();
    system("cls");
}

// Funcion para comprar coins
void comprarCoins() {
    if (usuarioActualIndex == -1) {
        cout << RED << "Debe iniciar sesion primero." << RESET << endl;
        getch();
        return;
    }

    Usuario& user = usuarios[usuarioActualIndex];
    bool salir = false;
    
    while (!salir) {
        system("cls");
        cout << BLUE << "========================================" << RESET << endl;
        cout << YELLOW << "          COMPRAR COINS               " << RESET << endl;
        cout << BLUE << "========================================" << RESET << endl;
        cout << "Usuario: " << user.username << endl;
        cout << "Coins actuales: " << GREEN << user.coins << RESET << endl;
        cout << BLUE << "----------------------------------------" << RESET << endl;
        cout << "Paquetes disponibles:" << endl << endl;
        cout << "1. 100 coins - $5.00" << endl;
        cout << "2. 250 coins (+50 bonus) - $10.00" << endl;
        cout << "3. 500 coins (+100 bonus) - $18.00" << endl;
        cout << "4. 1000 coins (+300 bonus) - $30.00" << endl;
        cout << BLUE << "----------------------------------------" << RESET << endl;
        cout << "5. Volver al menú principal" << endl;
        cout << BLUE << "========================================" << RESET << endl;
        cout << "Seleccione una opción: ";
        
        int opcion;
        cin >> opcion;
        
        if (opcion >= 1 && opcion <= 4) {
            double precios[] = {5.00, 10.00, 18.00, 30.00};
            int coins[] = {100, 300, 600, 1300}; // Incluye bonificación
            
            cout << endl << "Está a punto de comprar " << coins[opcion-1] 
                 << " coins por $" << fixed << setprecision(2) << precios[opcion-1] << endl;
            cout << "¿Desea continuar? (s/n): ";
            
            char confirmacion;
            cin >> confirmacion;
            
            if (tolower(confirmacion) == 's') {
                auto resultado = procesarPagoTarjeta(precios[opcion-1]);
                
                if (resultado.first) {
                    user.coins += coins[opcion-1];
                    user.totalGastado += precios[opcion-1];
                    
                    // Registrar la compra
                    Compra compraCoins;
                    compraCoins.juego = "Paquete de " + to_string(coins[opcion-1]) + " coins";
                    compraCoins.precio = precios[opcion-1];
                    compraCoins.fecha = obtenerFechaActual();
                    compraCoins.numeroTransaccion = resultado.second;
                    user.historialCompras.push_back(compraCoins);
                    
                    cout << endl << GREEN << "¡Compra exitosa!" << RESET << endl;
                    cout << "Se han añadido " << coins[opcion-1] << " coins a tu cuenta." << endl;
                    cout << "Nuevo saldo: " << GREEN << user.coins << " coins" << RESET << endl;
                } else {
                    cout << endl << RED << "El pago no se completó. No se añadieron coins." << RESET << endl;
                }
            } else {
                cout << endl << YELLOW << "Compra cancelada." << RESET << endl;
            }
        } else if (opcion == 5) {
            salir = true;
            continue;
        } else {
            cout << endl << RED << "Opción inválida." << RESET << endl;
        }
        
        cout << endl << "Presione cualquier tecla para continuar...";
        getch();
    }
}

// Funcion para la tienda de juegos
void PageStore() {
    if (usuarioActualIndex == -1) {
        system("cls");
        cout << RED << "Primero debe iniciar sesion para acceder a la tienda." << RESET << endl;
        cout << "Presione cualquier tecla para continuar...";
        getch();
        return;
    }

    int opcion;
    list<string> carrito;
    bool salirTienda = false;
    Usuario& user = usuarios[usuarioActualIndex];

    // Precios de los juegos en coins
    map<string, int> preciosCoins = {
        {"Juego de Snake", 50},
        {"Juego de Sudoku", 50},
        {"Juego de Cuatro en Raya", 50},
        {"Juego de Buscaminas", 50},
        {"Juego del Ahorcado", 50}
    };

    while (!salirTienda) {
        system("cls");
        cout << BLUE << "========================================" << RESET << endl;
        cout << BLUE << "            TIENDA DE JUEGOS            " << RESET << endl;
        cout << BLUE << "========================================" << RESET << endl;
        cout << endl;
        
        // Mostrar información del usuario
        cout << CYAN << "Usuario: " << user.username << RESET << endl;
        cout << CYAN << "Coins disponibles: " << GREEN << user.coins << RESET << endl;
        cout << BLUE << "----------------------------------------" << RESET << endl;
        
        cout << CYAN << "Juegos disponibles:" << RESET << endl;
        cout << "----------------------------------------" << endl;
        cout << "1. Juego de Snake - 50 coins" << endl;
        cout << "2. Juego de Sudoku - 50 coins" << endl;
        cout << "3. Juego de Cuatro en Raya - 50 coins" << endl;
        cout << "4. Juego de Buscaminas - 50 coins" << endl;
        cout << "5. Juego del Ahorcado - 50 coins" << endl;
        cout << "----------------------------------------" << endl;
        cout << "6. Ver el carrito (" << carrito.size() << " items)" << endl;
        cout << "7. Comprar juegos seleccionados" << endl;
        cout << "8. Ver mis juegos comprados" << endl;
        cout << "9. Volver al menu principal" << endl;
        cout << BLUE << "========================================" << RESET << endl;
        cout << endl;
        
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                if (find(user.games.begin(), user.games.end(), "Juego de Snake") == user.games.end()) {
                    carrito.push_back("Juego de Snake");
                    cout << GREEN << "Juego de Snake agregado al carrito." << RESET << endl;
                } else {
                    cout << YELLOW << "Ya posees este juego." << RESET << endl;
                }
                break;
            case 2:
                if (find(user.games.begin(), user.games.end(), "Juego de Sudoku") == user.games.end()) {
                    carrito.push_back("Juego de Sudoku");
                    cout << GREEN << "Juego de Sudoku agregado al carrito." << RESET << endl;
                } else {
                    cout << YELLOW << "Ya posees este juego." << RESET << endl;
                }
                break;
            case 3:
                if (find(user.games.begin(), user.games.end(), "Juego de Cuatro en Raya") == user.games.end()) {
                    carrito.push_back("Juego de Cuatro en Raya");
                    cout << GREEN << "Juego de Cuatro en Raya agregado al carrito." << RESET << endl;
                } else {
                    cout << YELLOW << "Ya posees este juego." << RESET << endl;
                }
                break;
            case 4:
                if (find(user.games.begin(), user.games.end(), "Juego de Buscaminas") == user.games.end()) {
                    carrito.push_back("Juego de Buscaminas");
                    cout << GREEN << "Juego de Buscaminas agregado al carrito." << RESET << endl;
                } else {
                    cout << YELLOW << "Ya posees este juego." << RESET << endl;
                }
                break;
            case 5:
                if (find(user.games.begin(), user.games.end(), "Juego del Ahorcado") == user.games.end()) {
                    carrito.push_back("Juego del Ahorcado");
                    cout << GREEN << "Juego del Ahorcado agregado al carrito." << RESET << endl;
                } else {
                    cout << YELLOW << "Ya posees este juego." << RESET << endl;
                }
                break;
            case 6: {
                system("cls");
                cout << YELLOW << "========================================" << RESET << endl;
                cout << YELLOW << "           CARRITO DE COMPRAS           " << RESET << endl;
                cout << YELLOW << "========================================" << RESET << endl;
                cout << endl;
                
                if (carrito.empty()) {
                    cout << "El carrito esta vacio." << endl;
                } else {
                    int totalCoins = 0;
                    cout << "Juegos en el carrito:" << endl;
                    cout << "----------------------------------------" << endl;
                    
                    for (const auto &item : carrito) {
                        cout << "- " << item << " - " << preciosCoins[item] << " coins" << endl;
                        totalCoins += preciosCoins[item];
                    }
                    
                    cout << "----------------------------------------" << endl;
                    cout << YELLOW << "Total: " << totalCoins << " coins" << RESET << endl;
                    cout << "Tus coins disponibles: " << user.coins << endl;
                    
                    if (user.coins < totalCoins) {
                        cout << RED << "Faltan " << (totalCoins - user.coins) 
                             << " coins para completar la compra." << RESET << endl;
                    }
                    
                    cout << endl << "1. Eliminar item del carrito" << endl;
                    cout << "2. Vaciar carrito" << endl;
                    cout << "3. Volver a la tienda" << endl;
                    cout << "Seleccione una opcion: ";
                    
                    int opcionCarrito;
                    cin >> opcionCarrito;
                    
                    if (opcionCarrito == 1 && !carrito.empty()) {
                        cout << "Ingrese el numero del item a eliminar (1-" << carrito.size() << "): ";
                        int itemEliminar;
                        cin >> itemEliminar;
                        
                        if (itemEliminar > 0 && static_cast<size_t>(itemEliminar) <= carrito.size()) {
                            auto it = carrito.begin();
                            advance(it, itemEliminar - 1);
                            cout << "Eliminando: " << *it << endl;
                            carrito.erase(it);
                        } else {
                            cout << RED << "Numero invalido." << RESET << endl;
                        }
                    } else if (opcionCarrito == 2) {
                        carrito.clear();
                        cout << GREEN << "Carrito vaciado." << RESET << endl;
                    }
                }
                break;
            }
            case 7: {
                if (carrito.empty()) {
                    cout << RED << "El carrito esta vacio. No hay nada que comprar." << RESET << endl;
                } else {
                    // Calcular total de coins necesarios
                    int totalCoins = 0;
                    for (const auto &item : carrito) {
                        totalCoins += preciosCoins[item];
                    }
                    
                    cout << "Resumen de compra:" << endl;
                    cout << "----------------------------------------" << endl;
                    for (const auto &item : carrito) {
                        cout << "- " << item << " (" << preciosCoins[item] << " coins)" << endl;
                    }
                    cout << "----------------------------------------" << endl;
                    cout << YELLOW << "Total a pagar: " << totalCoins << " coins" << RESET << endl;
                    cout << "Tus coins disponibles: " << user.coins << endl;
                    
                    if (user.coins >= totalCoins) {
                        cout << "¿Confirmar compra? (s/n): ";
                        char confirmar;
                        cin >> confirmar;
                        
                        if (tolower(confirmar) == 's') {
                            // Procesar la compra
                            user.coins -= totalCoins;
                            
                            // Añadir juegos a la biblioteca
                            for (const auto &item : carrito) {
                                if (find(user.games.begin(), user.games.end(), item) == user.games.end()) {
                                    user.games.push_back(item);
                                    
                                    // Registrar la compra en el historial
                                    Compra compra;
                                    compra.juego = item;
                                    compra.precio = 0.0; // Pagado con coins
                                    compra.fecha = obtenerFechaActual();
                                    compra.numeroTransaccion = "COINS-" + generarNumeroTransaccion();
                                    user.historialCompras.push_back(compra);
                                }
                            }
                            
                            cout << GREEN << "¡Compra realizada con éxito!" << RESET << endl;
                            cout << "Juegos añadidos a tu biblioteca." << endl;
                            cout << "Nuevo saldo: " << user.coins << " coins" << endl;
                            
                            carrito.clear();
                            guardarUsuariosEnArchivo();
                        } else {
                            cout << YELLOW << "Compra cancelada." << RESET << endl;
                        }
                    } else {
                        cout << RED << "No tienes suficientes coins." << RESET << endl;
                        cout << "Necesitas " << totalCoins << " coins (tienes " << user.coins << ")." << endl;
                        cout << "Visita la sección 'Añadir Fondos' del menú principal para obtener más coins." << endl;
                    }
                }
                break;
            }
            case 8: {
                system("cls");
                cout << BLUE << "========================================" << RESET << endl;
                cout << BLUE << "         MIS JUEGOS COMPRADOS           " << RESET << endl;
                cout << BLUE << "========================================" << RESET << endl;
                cout << endl;
                
                if (user.games.empty()) {
                    cout << YELLOW << "No has comprado ningún juego aún." << RESET << endl;
                    cout << "Visita la tienda para adquirir juegos." << endl;
                } else {
                    cout << "Tus juegos:" << endl;
                    cout << "----------------------------------------" << endl;
                    for (const auto& juego : user.games) {
                        cout << "- " << juego << endl;
                    }
                    cout << "----------------------------------------" << endl;
                    cout << "Total de juegos: " << user.games.size() << endl;
                }
                break;
            }
            case 9:
                salirTienda = true;
                break;
            default:
                cout << RED << "Opcion no valida. Intente nuevamente." << RESET << endl;
                break;
        }

        if (opcion != 9) {
            cout << "Presione cualquier tecla para continuar...";
            getch();
        }
    }
}

// Funcion para ver saldo e historial
void verSaldoYHistorial() {
    if (usuarioActualIndex == -1) {
        cout << RED << "Debe iniciar sesion primero." << RESET << endl;
        getch();
        return;
    }

    const Usuario& user = usuarios[usuarioActualIndex];
    bool salir = false;
    
    while (!salir) {
        system("cls");
        cout << BLUE << "========================================" << RESET << endl;
        cout << YELLOW << "       SALDO E HISTORIAL DE COINS      " << RESET << endl;
        cout << BLUE << "========================================" << RESET << endl;
        cout << "Usuario: " << user.username << endl;
        cout << "Coins disponibles: " << GREEN << user.coins << RESET << endl;
        cout << BLUE << "----------------------------------------" << RESET << endl;
        
        // Mostrar solo transacciones relacionadas con coins
        cout << "HISTORIAL DE COMPRAS DE COINS:" << endl;
        cout << "----------------------------------------" << endl;
        
        bool hayHistorial = false;
        for (const auto& compra : user.historialCompras) {
            if (compra.juego.find("coins") != string::npos || 
                compra.juego.find("Coins") != string::npos) {
                cout << "- " << compra.juego << endl;
                cout << "  Precio: $" << fixed << setprecision(2) << compra.precio << endl;
                cout << "  Fecha: " << compra.fecha << endl;
                cout << "  Transacción: " << compra.numeroTransaccion << endl << endl;
                hayHistorial = true;
            }
        }
        
        if (!hayHistorial) {
            cout << YELLOW << "No hay historial de compras de coins." << RESET << endl;
        }
        
        cout << BLUE << "========================================" << RESET << endl;
        cout << "1. Volver al menú principal" << endl;
        cout << BLUE << "========================================" << RESET << endl;
        cout << "Seleccione una opción: ";
        
        int opcion;
        cin >> opcion;
        
        if (opcion == 1) {
            salir = true;
        }
    }
}

// Funcion para mostrar la biblioteca de juegos y permitir jugar
void PageLibrary() {
    if (usuarioActualIndex == -1) {
        system("cls");
        cout << RED << "Primero debe iniciar sesion para acceder a su biblioteca." << RESET << endl;
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
            cout << "\nPresione cualquier tecla para volver al menu principal...";
            getch();
            salirBiblioteca = true;
        } else {
            cout << "Tus juegos:" << endl;
            int contador = 1;
            for (const auto &game : usuarios[usuarioActualIndex].games) {
                cout << contador << ". " << game << endl;
                contador++;
            }
            cout << contador << ". Volver al menu principal" << endl;

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
                }
                else if(juegoSeleccionado=="Juego de Buscaminas") {
                  playminas();
                }
                else if (juegoSeleccionado == "Juego del Ahorcado") {
    			playAhorcado();
				}
                else {cout << RED << "Juego no reconocido." << RESET << endl;
                }
            } else {
                cout << RED << "Opcion no valida. Intente nuevamente." << RESET << endl;
                cout << "Presione cualquier tecla para continuar...";
                getch();
            }
        }
    }
}

int main() {
    srand(time(0));
    SetConsoleOutputCP(CP_UTF8);
    
    // Cargar usuarios desde archivo
    cargarUsuariosDesdeArchivo();
    
    int opcion;
    bool salir = false;
    
    do {
        system("cls");
        mostrarMenuInicial();
        cin >> opcion;        
        switch (opcion) {
            case 1:
                registrarUsuario();
                break;
            case 2:
                iniciarSesion();
                if (usuarioActualIndex != -1) {
                    cout << GREEN << "Bienvenido, " << usuarios[usuarioActualIndex].username << "!" << RESET << endl;
                    int opcionUsuario;
                    bool volverMenuInicial = false;
                    do {
                        system("cls");
                        mostrarMenu();
                        cin >> opcionUsuario;
                        switch (opcionUsuario) {
                            case 1:
                                PageStore();
                                break;
                            case 2:
                                PageLibrary();
                                break;
                            case 3:
                                if (usuarioActualIndex != -1) {
                                    mostrarEstadisticasJugador(usuarios[usuarioActualIndex]);
                                } else {
                                    cout << RED << "Debe iniciar sesion primero." << RESET << endl;
                                    cout << "Presione cualquier tecla para continuar...";
                                    getch();
                                }
                                break;
                            case 4:
                                mostrarRankingGlobal();
                                break;
                            case 5:
                                personalizarPerfil();
                                break;
                            case 6:
                                gestionarAmigos();
                                break;
                            case 7:
                                comprarCoins();
                                break;
                            case 8:
                                verSaldoYHistorial();
                                break;
                            case 9:
                                mostrarUsuariosRegistrados();
                                break;
                            case 10:
                                limpiarArchivoCorrupto();
                                break;
                            case 11:
                                cerrarSesion();
                                getch();
                                volverMenuInicial = true;
                                break;
                            default:
                                cout << RED << "Opcion invalida. Intente nuevamente." << RESET << endl;
                                cout << "Presione cualquier tecla para continuar...";
                                getch();
                                break;
                        }
                    } while (!volverMenuInicial); 
                }
                break;
            case 3: {
                system("cls");
                cout << RED << "Saliendo de la plataforma..." << RESET << endl;
                cout << "Gracias por usar la plataforma de juegos!" << endl;
                cout << "Presione cualquier tecla para continuar...";
                getch();
                salir = true;
                break;
            }
            default:
                cout << RED << "Opcion invalida. Intente nuevamente." << RESET << endl;
                cout << "Presione cualquier tecla para continuar...";
                getch();
                break;
        } 
    } while (!salir);

    return 0;
}
