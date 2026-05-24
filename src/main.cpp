#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> 
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#define QUANT_BOLAS 3
#define MENOR_T 0.01
#define TAM_X 10
#define TAM_Y 10
#define V_MAX 2;
using namespace std;

struct Bola{
    double x, y, vx, vy, m, raio;
};
void velocidadesAleatorias(double *vx, double *vy);
void iniciarSimulacao(SDL_Window *janela, SDL_Renderer *renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const double v_max);
void atualizarSimulacao(SDL_Window *janela, SDL_Renderer *renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const double v_max);
void desenharBola();

int main(int argc, char* argv[]){ 
    const int quant_bolas = QUANT_BOLAS;
    const double dt = MENOR_T;
    const int tam_x = TAM_X;
    const int tam_y = TAM_Y;
    const double v_max = V_MAX;

    vector<Bola> vetorBolas;
    SDL_Window* janela = nullptr;
    SDL_Renderer* renderizador = nullptr;

    iniciarSimulacao(janela, renderizador, &vetorBolas, quant_bolas, dt, tam_x, tam_y, v_max);
    while(1){
        atualizarSimulacao(janela, renderizador, &vetorBolas, quant_bolas, dt, tam_x, tam_y, v_max);
    }

    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(janela);
    SDL_Quit();
    return 0;
}

void velocidadesAleatorias(double *vx, double *vy){
    
}

void iniciarSimulacao(SDL_Window *janela, SDL_Renderer *renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const double v_max){
    for(int i=0; i<quant_bolas; i++){
        Bola bola;
        bola.x = rand()%tam_x;
        bola.y = rand()%tam_y;
        vetorBolas->push_back(bola);
    }

    SDL_Init(SDL_INIT_VIDEO);
    janela = SDL_CreateWindow("Simulador de Colisões - Física I", tam_x, tam_y, 0);
    renderizador = SDL_CreateRenderer(janela, NULL);
}

void atualizarSimulacao(SDL_Window *janela, SDL_Renderer *renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const double v_max){

}