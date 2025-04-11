#include <iostream>
#include <cstdint>
#include <chrono>
#include <cstring>
#include <new>
#include <thread>
#include <mutex>
#include "memory.hpp"  // La tua classe Memory
#include "cpu.hpp"     // La tua classe CPU

constexpr size_t CACHE_LINE_SIZE = 64;  // 64 byte per linea

// Struttura della CacheLine con un puntatore raw per il blocco di dati
struct CacheLine {
    uint64_t tag;  // Identifica il blocco memorizzato
    uint64_t* data; // Puntatore al blocco di dati (allocato con new allineato)
    bool valid;    // Bit di validità

    CacheLine() : tag(0), data(nullptr), valid(false) {}
};

// Classe Cache
class Cache {
private:
    CacheLine* lines;  // Array di CacheLine
    size_t lineSize;   // Dimensione della linea (in byte), qui 64
    size_t numLines;   // Numero totale di linee
    Memory& memory;    // Riferimento alla memoria principale

public:
    // totalSize: dimensione totale della cache in byte, lineSize: dimensione di ogni linea
    Cache(size_t totalSize, size_t lineSize, Memory& mem) : lineSize(lineSize), memory(mem) {
        numLines = totalSize / lineSize;
        // Allocazione dell'array delle linee usando l'overload di operator new con allineamento
        lines = new(std::align_val_t(64)) CacheLine[numLines];
        for (size_t i = 0; i < numLines; ++i) {
            // Allocazione del blocco dati per ciascuna linea, per lineSize byte
            lines[i].data = static_cast<uint64_t*>(::operator new[](lineSize, std::align_val_t(64)));
        }
    }

    ~Cache() {
        for (size_t i = 0; i < numLines; ++i) {
            ::operator delete[](lines[i].data, std::align_val_t(64));
        }
        ::operator delete[](lines, std::align_val_t(64));
    }

    // Funzione di lettura dalla cache: se c'è un hit restituisce il dato, altrimenti carica il blocco dalla memoria
    uint64_t read(uint64_t addr) {
        // Calcola l'indirizzo del blocco allineato (blocco: linea da 64 byte)
        uint64_t blockAddr = (addr / lineSize) * lineSize;
        uint64_t tag = blockAddr / lineSize;  // Tag univoco per il blocco
        // Indice della cache: semplice modulo sul numero di linee
        size_t index = (blockAddr / lineSize) % numLines;

        CacheLine& line = lines[index];

        if (line.valid && line.tag == tag) {
            // Cache hit
            size_t offset = addr % lineSize;
            uint64_t value;
            std::memcpy(&value, reinterpret_cast<uint8_t*>(line.data) + offset, sizeof(uint64_t));
            //std::cout << "Cache hit: " << value << std::endl;


            return value;
        } else {
            // Cache miss: copia l'intero blocco dalla memoria principale
            std::memcpy(line.data, &memory.getData()[blockAddr], lineSize);
            line.tag = tag;
            line.valid = true;
            size_t offset = addr % lineSize;
            uint64_t value;
            std::memcpy(&value, reinterpret_cast<uint8_t*>(line.data) + offset, sizeof(uint64_t));
            return value;
        }
    }
};

std::mutex mtx;  // Mutex per la sincronizzazione della somma

void readCache(uint64_t start, uint64_t end, Cache& cache, uint64_t& result) {
    uint64_t sum = 0;
    for (uint64_t i = start; i < end; ++i) {
        sum += cache.read(i * 8);
    }
    // Sincronizzazione dell'accesso alla variabile condivisa
    std::lock_guard<std::mutex> lock(mtx);
    result += sum;
}

int main() {
    const size_t MEM_SIZE = 10 * 1024 * 1024; // 10 MB di memoria
    CPU cpu;
    Memory memory(MEM_SIZE, cpu);

    // Inizializza la memoria con valori da 0 a 999999 (ogni valore a 8 byte)
    for (size_t i = 0; i < 1000000; ++i) {
        memory.writeGeneric<uint64_t>(i * 8, i);
    }

    // Crea una cache da 1 MB con linee di 64 byte
    Cache cache(1 * 1024 * 1024, CACHE_LINE_SIZE, memory);

    const size_t numThreads = 8;  // Numero di thread da utilizzare
    std::vector<std::thread> threads;
    uint64_t sum_cache = 0;
    uint64_t sum_memory = 0; // Somma per la memoria principale

    // Test sequenziale sulla memoria
    auto start_seq = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 1000000; ++i) {
        sum_memory += memory.readGeneric<uint64_t>(i * 8);
    }
    auto end_seq = std::chrono::high_resolution_clock::now();

    auto duration_seq = std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq);






    // Test parallelo sulla cache
    auto start_cache = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(readCache, i * 125000, (i + 1) * 125000, std::ref(cache), std::ref(sum_cache)));
    }
    for (auto& t : threads) {
        t.join();
    }
    auto end_cache = std::chrono::high_resolution_clock::now();
    auto duration_cache = std::chrono::duration_cast<std::chrono::milliseconds>(end_cache - start_cache);

    std::cout << "Tempo lettura sequenziale dalla memoria: " << duration_seq.count() << " ms\n";
    std::cout << "Tempo lettura parallela dalla cache:   " << duration_cache.count() << " ms\n";
    std::cout << "Somma cache: " << sum_cache << "\n";
    std::cout << "Somma memoria: " << sum_memory << "\n"; // La somma dovrebbe essere la stessa

    return 0;
}
