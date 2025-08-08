#include <iostream>
#include <cstring>

using namespace std;

class Viaje {
public:
    virtual ~Viaje() {}
    virtual float calcularCosto() const = 0;
    virtual void mostrar(ostream& os) const = 0;
};

ostream& operator<<(ostream& os, const Viaje& viaje) {
    viaje.mostrar(os);
    return os;
}

/*-------------------------------------------------------------------------------------------*/

class ViajeComun : public Viaje {
private:
    char* destino;
    float montoFijo;
public:
    ViajeComun(const char* dest, float monto);
    ~ViajeComun();
    float calcularCosto() const override;
    void mostrar(ostream& os) const override;
};

ViajeComun::ViajeComun(const char* dest, float monto) : montoFijo(monto) {
    destino = new char[strlen(dest) + 1];
    strcpy(destino, dest);
}

ViajeComun::~ViajeComun() {
    delete[] destino;
}

float ViajeComun::calcularCosto() const {
    return montoFijo;
}

void ViajeComun::mostrar(ostream& os) const {
    os << "Viaje Comun a " << destino << " - Costo: $" << montoFijo;
}

/*-------------------------------------------------------------------------------------------*/

class ViajePorDestino : public Viaje {
private:
    char* destino;
    float distanciaKm;
    float precioPorKm;
public:
    ViajePorDestino(const char* dest, float dist, float precio);
    ~ViajePorDestino();
    float calcularCosto() const override;
    void mostrar(ostream& os) const override;
};

ViajePorDestino::ViajePorDestino(const char* dest, float dist, float precio) : distanciaKm(dist), precioPorKm(precio) {
    destino = new char[strlen(dest) + 1];
    strcpy(destino, dest);
}

ViajePorDestino::~ViajePorDestino() {
    delete[] destino;
}

float ViajePorDestino::calcularCosto() const {
    return distanciaKm * precioPorKm;
}

void ViajePorDestino::mostrar(ostream& os) const {
    os << "Viaje por Destino a " << destino << " - " << distanciaKm 
       << "km * $" << precioPorKm << "/km - Costo: $" << calcularCosto();
}

/*-------------------------------------------------------------------------------------------*/

class ViajeMixto : public Viaje {
private:
    ViajeComun* viajeComun;
    ViajePorDestino** viajesDestino; // Vector dinámico
    int cantidadViajesDestino;
    int capacidadViajesDestino;
public:
    ViajeMixto(ViajeComun* comun);
    ~ViajeMixto();
    void agregarViajeDestino(ViajePorDestino* viaje);
    float calcularCosto() const override;
    void mostrar(ostream& os) const override;
};

ViajeMixto::ViajeMixto(ViajeComun* comun) : viajeComun(comun), cantidadViajesDestino(0), capacidadViajesDestino(100) {
    viajesDestino = new ViajePorDestino*[capacidadViajesDestino];
}

ViajeMixto::~ViajeMixto() {
    delete viajeComun;
    for (int i = 0; i < cantidadViajesDestino; i++) {
        delete viajesDestino[i];
    }
    delete[] viajesDestino;
}

void ViajeMixto::agregarViajeDestino(ViajePorDestino* viaje) { // Redimensionar el vector dinámico
    if (cantidadViajesDestino == capacidadViajesDestino) {
        capacidadViajesDestino *= 2;
        ViajePorDestino** nuevo = new ViajePorDestino*[capacidadViajesDestino];
        for (int i = 0; i < cantidadViajesDestino; i++) {
            nuevo[i] = viajesDestino[i];
        }
        delete[] viajesDestino;
        viajesDestino = nuevo;
    }
    viajesDestino[cantidadViajesDestino++] = viaje;
}

float ViajeMixto::calcularCosto() const {
    float total = viajeComun->calcularCosto();
    for (int i = 0; i < cantidadViajesDestino; i++) {
        total += viajesDestino[i]->calcularCosto();
    }
    return total;
}

void ViajeMixto::mostrar(ostream& os) const {
    os << "Viaje Mixto:\n";
    os << "  - " << *viajeComun << "\n";
    for (int i = 0; i < cantidadViajesDestino; i++) {
        os << "  - " << *viajesDestino[i] << "\n";
    }
    os << "Costo Total: $" << calcularCosto();
}

/*-------------------------------------------------------------------------------------------*/

class Gestor {
private:
    Viaje** viajes;
    int cantidadViajes;
    int capacidadViajes;
public:
    Gestor();
    ~Gestor();
    void agregarViaje(Viaje* viaje);
    void mostrarTodos() const;
    float calcularTotal() const;
};

Gestor::Gestor() : cantidadViajes(0), capacidadViajes(10) {
    viajes = new Viaje*[capacidadViajes];
}

Gestor::~Gestor() {
    for (int i = 0; i < cantidadViajes; i++) {
        delete viajes[i];
    }
    delete[] viajes;
}

void Gestor::agregarViaje(Viaje* viaje) {
    if (cantidadViajes == capacidadViajes) {
        capacidadViajes *= 2;
        Viaje** nuevo = new Viaje*[capacidadViajes];
        for (int i = 0; i < cantidadViajes; i++) {
            nuevo[i] = viajes[i];
        }
        delete[] viajes;
        viajes = nuevo;
    }
    viajes[cantidadViajes++] = viaje;
}

void Gestor::mostrarTodos() const {
    for (int i = 0; i < cantidadViajes; i++) {
        cout << *viajes[i] << "\n\n";
    }
}

float Gestor::calcularTotal() const {
    float total = 0;
    for (int i = 0; i < cantidadViajes; i++) {
        total += viajes[i]->calcularCosto();
    }
    return total;
}

/*-------------------------------------------------------------------------------------------*/

int main() {
    Gestor objeto;
    
    // Viaje común
    objeto.agregarViaje(new ViajeComun("Crespo", 2000));
    
    // Viaje por destino
    objeto.agregarViaje(new ViajePorDestino("Diamante", 30, 100));
    
    // Viaje mixto
    ViajeMixto* mixto = new ViajeMixto(new ViajeComun("Crespo", 2000));
    mixto->agregarViajeDestino(new ViajePorDestino("Parque Predelta", 20, 100));
    objeto.agregarViaje(mixto);
    
    // Mostrar todos los viajes
    objeto.mostrarTodos();
    
    // Calcular total
    cout << "Total a abonar por todos los viajes: $" << objeto.calcularTotal() << endl;
    
    return 0;
}