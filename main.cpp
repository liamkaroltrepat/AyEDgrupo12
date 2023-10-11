#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Estructura para representar los registros de stock
struct StockRecord {
    string producto;
    char pasillo;
    int posicion;
    int stock;

    // Constructor por defecto
    StockRecord() : producto(""), pasillo(' '), posicion(0), stock(0) {}
};

// Estructura para representar los registros de movimientos
struct MovementRecord {
    string producto;
    char pasillo;
    int posicion;
    int movimiento;

    // Constructor por defecto
    MovementRecord() : producto(""), pasillo(' '), posicion(0), movimiento(0) {}
};

int main() {
    // Paso 1: Leer el archivo de existencias iniciales
    vector<StockRecord> stock;
    ifstream stockFile("Existencias.bin", ios::binary);

    if (!stockFile) {
        cerr << "Error: No se pudo abrir el archivo de existencias." << endl;
        return 1;
    }

    while (true) {
        StockRecord record;
        stockFile.read(reinterpret_cast<char*>(&record), sizeof(StockRecord));

        if (stockFile.eof()) {
            break;
        }

        stock.push_back(record);
    }

    stockFile.close();

    // Paso 2: Leer el archivo de movimientos y aplicarlos
    vector<MovementRecord> movimientos;
    ifstream movimientosFile("Movimientos.bin", ios::binary);

    if (!movimientosFile) {
        cerr << "Error: No se pudo abrir el archivo de movimientos." << endl;
        return 1;
    }

    while (true) {
        MovementRecord movimiento;
        movimientosFile.read(reinterpret_cast<char*>(&movimiento), sizeof(MovementRecord));

        if (movimientosFile.eof()) {
            break;
        }

        movimientos.push_back(movimiento);
    }

    movimientosFile.close();

    for (const MovementRecord& movimiento : movimientos) {
        bool encontrado = false;
        for (StockRecord& registro : stock) {
            if (registro.producto == movimiento.producto &&
                registro.pasillo == movimiento.pasillo &&
                registro.posicion == movimiento.posicion) {
                registro.stock += movimiento.movimiento;
                encontrado = true;
                break;
            }
        }

        if (!encontrado) {
            cerr << "Error: Movimiento incorrecto - Producto: " << movimiento.producto
                 << " Pasillo: " << movimiento.pasillo << " Posición: " << movimiento.posicion << endl;
        }
    }

    // Paso 3: Grabar el nuevo archivo con el stock final
    ofstream finalFile("Final.bin", ios::binary);

    if (!finalFile) {
        cerr << "Error: No se pudo crear el archivo Final." << endl;
        return 1;
    }

    for (const StockRecord& registro : stock) {
        finalFile.write(reinterpret_cast<const char*>(&registro), sizeof(StockRecord));
    }

    finalFile.close();

    // Paso 4: Mostrar el listado ordenado por producto, pasillo y posición
    sort(stock.begin(), stock.end(), [](const StockRecord& a, const StockRecord& b) {
        if (a.producto == b.producto) {
            if (a.pasillo == b.pasillo) {
                return a.posicion < b.posicion;
            }
            return a.pasillo < b.pasillo;
        }
        return a.producto < b.producto;
    });

    cout << "Producto Pasillo Posición StockInicial SumatoriaMovimientos StockFinal" << endl;
    for (const StockRecord& registro : stock) {
        cout << registro.producto << " " << registro.pasillo << " " << registro.posicion << " "
             << registro.stock - movimientos[0].movimiento << " " << movimientos[0].movimiento << " " << registro.stock << endl;
    }

    // Paso 5: Leer y listar el contenido del archivo Final.bin como comprobación
    ifstream finalFileRead("Final.bin", ios::binary);

    if (!finalFileRead) {
        cerr << "Error: No se pudo abrir el archivo Final." << endl;
        return 1;
    }

    vector<StockRecord> stockFinal;
    while (true) {
        StockRecord registro;
        finalFileRead.read(reinterpret_cast<char*>(&registro), sizeof(StockRecord));

        if (finalFileRead.eof()) {
            break;
        }

        stockFinal.push_back(registro);
    }

    finalFileRead.close();

    cout << "\nContenido de Final.bin:\n";
    for (const StockRecord& registro : stockFinal) {
        cout << registro.producto << " " << registro.pasillo << " " << registro.posicion << " " << registro.stock << endl;
    }

    return 0;
}