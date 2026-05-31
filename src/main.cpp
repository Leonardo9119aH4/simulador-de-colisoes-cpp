#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h> 
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_timer.h>
#include <vector>
#include <iostream>
#include <random>
#include <ctime>
#include <cmath>

#define QUANT_BOLAS 3
#define MENOR_T 0.01
#define TAM_X 100
#define TAM_Y 100
#define V_MAX 2
#define M_MIN 1
#define M_MAX 10
#define R_BOLA 20 //valor estático, raio da bola
#define SIM_DELAY 16 //delay para cada atualização em ms

using namespace std;

struct Bola{
    double x, y, vx, vy, m;
};
void velocidadesAleatorias(double *vx, double *vy);
void iniciarSimulacao(SDL_Window *janela, SDL_Renderer *renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const double v_max, const double m_min, const double m_max);
void atualizarSimulacao(SDL_Window *janela, SDL_Renderer *renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y);
void desenharBola(SDL_Renderer *renderizador, const Bola &bola);

int main(int argc, char* argv[]){ 
    const int quant_bolas = QUANT_BOLAS; //quantidade de bolinhas a serem geradas
    const double dt = MENOR_T; //intervalo de tempo da simulação
    const int tam_x = TAM_X; //largura da mesa (e da janela)
    const int tam_y = TAM_Y; //altura da mesa (e da janela)
    const double v_max = V_MAX; //velocidade máxima da bolinha a ser gerada
    const double m_min = M_MIN; //massa mínima da bolinha a ser gerada
    const double m_max = M_MAX; //massa máxima da bolinha a ser gerada

    vector<Bola> vetorBolas; //vetor com as bolinhas geradas
    SDL_Window *janela = nullptr; 
    SDL_Renderer *renderizador = nullptr;
    bool em_simulacao = true;

    iniciarSimulacao(janela, renderizador, &vetorBolas, quant_bolas, dt, tam_x, tam_y, v_max, m_min, m_max);
    // SDL_Event evento;
    // while(em_simulacao){
    //     while(SDL_PollEvent(&evento)){
    //         if(evento.type == SDL_EVENT_QUIT){
    //             em_simulacao = false;
    //         }
    //     }
    //     atualizarSimulacao(janela, renderizador, &vetorBolas, quant_bolas, dt, tam_x, tam_y);
    //     SDL_Delay(SIM_DELAY);
    // }

    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(janela);
    SDL_Quit();
    return 0;
}

void velocidadesAleatorias(Bola *bola, double vmax, mt19937 gerador){ //gera uma velocidade aleatória
    double v, ang;
    uniform_real_distribution<double> gera_v(0.0, vmax);
    uniform_real_distribution<double> gera_ang(-M_PI, M_PI);
    v = gera_v(gerador);
    ang = gera_ang(gerador);
    bola->vx = v*cos(ang);
    bola->vy = v*sin(ang);
}

void iniciarSimulacao(SDL_Window *janela, SDL_Renderer *renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const double v_max, const double m_min, const double m_max){
    random_device rd;
    mt19937 gerador(rd()); //semente dos números aleatórios
    uniform_real_distribution<double> gera_pos_x(R_BOLA/2.0, tam_x - R_BOLA/2.0);
    uniform_real_distribution<double> gera_pos_y(R_BOLA/2.0, tam_y - R_BOLA/2.0);
    uniform_real_distribution<double> gera_m(m_min, m_max);
    for(int i=0; i<quant_bolas; i++){
        Bola bola;
        bola.x = gera_pos_x(gerador);
        bola.y = gera_pos_y(gerador);
        bola.m = gera_m(gerador);
        velocidadesAleatorias(&bola, v_max, gerador);
        cout << "x: " << bola.x << " y: " << bola.y << " m: " << bola.m << " Vx: " << bola.vx << " Vy: " << bola.vy << endl; // DEBUG
        vetorBolas->push_back(bola);
    }

    SDL_Init(SDL_INIT_VIDEO);
    janela = SDL_CreateWindow("Simulador de Colisões - Física I", tam_x, tam_y, 0);
    renderizador = SDL_CreateRenderer(janela, NULL);
    SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
    SDL_RenderClear(renderizador);

}

void atualizarSimulacao(SDL_Window *janela, SDL_Renderer *renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y){

}

void desenharBola(SDL_Renderer *renderizador, const Bola &bola) {
    SDL_SetRenderDrawColor(renderizador, 0, 0, 10, 255);

    int x0 = static_cast<int>(bola.x);
    int y0 = static_cast<int>(bola.y);
    int raio = static_cast<int>(R_BOLA);
    int x = raio;
    int y = 0;
    int xChange = 1 - (raio << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y) {
        // DIFERENÇA SDL3: A função SDL_RenderDrawLine mudou para SDL_RenderLine 
        // e agora aceita coordenadas do tipo 'float' para maior precisão de sub-pixel.
        SDL_RenderLine(renderizador, static_cast<float>(x0 - x), static_cast<float>(y0 + y), static_cast<float>(x0 + x), static_cast<float>(y0 + y));
        SDL_RenderLine(renderizador, static_cast<float>(x0 - x), static_cast<float>(y0 - y), static_cast<float>(x0 + x), static_cast<float>(y0 - y));
        SDL_RenderLine(renderizador, static_cast<float>(x0 - y), static_cast<float>(y0 + x), static_cast<float>(x0 + y), static_cast<float>(y0 + x));
        SDL_RenderLine(renderizador, static_cast<float>(x0 - y), static_cast<float>(y0 - x), static_cast<float>(x0 + y), static_cast<float>(y0 - x));

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0) {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}