#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h> 
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <random>
#include <cmath>
#include <fstream>

#define QUANT_BOLAS 3
#define MENOR_T 0.01 
#define TAM_X 600
#define TAM_Y 600
#define V_MAX 150
#define M_MIN 1
#define M_MAX 10
#define R_BOLA 20 

using namespace std;

struct Bola{
    double x, y, vx, vy, m;
};
void carregarEntrada(const string &nomeArquivo, int *quant_bolas, double *dt, int *tam_x, int *tam_y, double *v_max, double *m_min, double *m_max, int *r_bola);
void velocidadesAleatorias(Bola *bola, double vmax, mt19937 gerador);
bool iniciarSimulacao(SDL_Window **janela, SDL_Renderer **renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const double v_max, const double m_min, const double m_max, const int r_bola);
void atualizarSimulacao(SDL_Window **janela, SDL_Renderer **renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const int r_bola);
void desenharBola(SDL_Renderer *renderizador, const Bola *bola, const int r_bola);
void atualizarBolas(vector<Bola> *bolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const int r_bola);
void calculaColisaoEntreBolas(Bola *bola1, Bola *bola2, double nx, double ny);

int main(int argc, char* argv[]){
    int quant_bolas = QUANT_BOLAS; //quantidade de bolinhas a serem geradas
    double dt = MENOR_T; //intervalo de tempo da simulação, em segundos
    int tam_x = TAM_X; //largura da mesa (e da janela)
    int tam_y = TAM_Y; //altura da mesa (e da janela)
    double v_max = V_MAX; //velocidade máxima da bolinha a ser gerada
    double m_min = M_MIN; //massa mínima da bolinha a ser gerada
    double m_max = M_MAX; //massa máxima da bolinha a ser gerada
    int r_bola = R_BOLA; //raio das bolas
    vector<Bola> vetorBolas; //vetor com as bolinhas geradas
    SDL_Window *janela = nullptr; 
    SDL_Renderer *renderizador = nullptr;
    bool em_simulacao = true;
    SDL_Event evento;

    carregarEntrada("entrada.txt", &quant_bolas, &dt, &tam_x, &tam_y, &v_max, &m_min, &m_max, &r_bola);
    while(SDL_PollEvent(&evento)){
        if(evento.type == SDL_EVENT_QUIT){
            em_simulacao = false;
        }
    }
    if(!iniciarSimulacao(&janela, &renderizador, &vetorBolas, quant_bolas, dt, tam_x, tam_y, v_max, m_min, m_max, r_bola)){
        return -1;
    }
    while(em_simulacao){
        while(SDL_PollEvent(&evento)){
            if(evento.type == SDL_EVENT_QUIT){
                em_simulacao = false;
            }
        }
        atualizarSimulacao(&janela, &renderizador, &vetorBolas, quant_bolas, dt, tam_x, tam_y, r_bola);
        SDL_Delay(static_cast<Uint32>(dt * 1000.0)); //delay entre cada frame
    }

    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(janela);
    SDL_Quit();
    return 0;
}

void carregarEntrada(const string &nomeArquivo, int *quant_bolas, double *dt, int *tam_x, int *tam_y, double *v_max, double *m_min, double *m_max, int *r_bola){
    ifstream entrada(nomeArquivo);
    string rotulo;
    if(!entrada.is_open()){
        cout << "Nenhum arquivo de entrada detectado, usando valores padrões" << endl;
        return;
    }
    entrada >> rotulo >> *quant_bolas;
    entrada >> rotulo >> *dt;
    entrada >> rotulo >> *tam_x;
    entrada >> rotulo >> *tam_y;
    entrada >> rotulo >> *v_max;
    entrada >> rotulo >> *m_min;
    entrada >> rotulo >> *m_max;
    entrada >> rotulo >> *r_bola;
    entrada.close();
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

bool iniciarSimulacao(SDL_Window **janela, SDL_Renderer **renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const double v_max, const double m_min, const double m_max, const int r_bola){
    random_device rd;
    mt19937 gerador(rd()); //semente dos números aleatórios
    uniform_real_distribution<double> gera_pos_x(r_bola/2.0, tam_x - r_bola/2.0);
    uniform_real_distribution<double> gera_pos_y(r_bola/2.0, tam_y - r_bola/2.0);
    uniform_real_distribution<double> gera_m(m_min, m_max);
    for(int i=0; i<quant_bolas; i++){
        Bola bola;
        bola.x = gera_pos_x(gerador);
        bola.y = gera_pos_y(gerador);
        bola.m = gera_m(gerador);
        velocidadesAleatorias(&bola, v_max, gerador);
        cout << "x: " << bola.x << ", y: " << bola.y << ", m: " << bola.m << ", Vx: " << bola.vx << ", Vy: " << bola.vy << endl; // DEBUG
        vetorBolas->push_back(bola);
    }

    if(!SDL_Init(SDL_INIT_VIDEO)){
        cout << "Erro SDL: " << SDL_GetError() << endl;
        return false;
    }
    *janela = SDL_CreateWindow("Simulador de Colisões - Física I", tam_x, tam_y, 0);
    if(!janela){
        cout << "Erro SDL: " << SDL_GetError() << endl;
        return false;
    }
    *renderizador = SDL_CreateRenderer(*janela, "opengl");
    if(!renderizador){
        cout << "Erro SDL: " << SDL_GetError() << endl;
        return false;
    }
    return true;
}

void atualizarSimulacao(SDL_Window **janela, SDL_Renderer **renderizador, vector<Bola> *vetorBolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const int r_bola){
    SDL_SetRenderDrawColor(*renderizador, 0, 0, 0, 255); //Cor de fundo
    SDL_RenderClear(*renderizador); //Limpa a tela e coloca a cor de fundo
    atualizarBolas(vetorBolas, quant_bolas, dt, tam_x, tam_y, r_bola);
    for(int i=0; i<quant_bolas; i++){
        desenharBola(*renderizador, &vetorBolas->at(i), r_bola);
    }
    SDL_RenderPresent(*renderizador);
    SDL_ShowWindow(*janela);
}

void desenharBola(SDL_Renderer *renderizador, const Bola *bola, const int r_bola) {
    SDL_SetRenderDrawColor(renderizador, 0, 0, 100, 255); //Cor da bolinha

    int x0 = static_cast<int>(bola->x);
    int y0 = static_cast<int>(bola->y);
    int raio = static_cast<int>(r_bola);
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

void atualizarBolas(vector<Bola> *bolas, const int quant_bolas, const double dt, const int tam_x, const int tam_y, const int r_bola){
    for(int i=0; i<quant_bolas; i++){ //atualiza as posições de cada bolinha
        bolas->at(i).x += bolas->at(i).vx*dt;
        bolas->at(i).y += bolas->at(i).vy*dt;
    }
    for(int i=0; i<quant_bolas; i++){ //verifica se há colisão
        double x = bolas->at(i).x;
        double y = bolas->at(i).y;
        if(x<=r_bola || x>=tam_x-r_bola){ //colisão com uma parede horizontal
            bolas->at(i).vx *= -1;
        }
        if(y<=r_bola || y>=tam_y-r_bola){ //colisão com uma parede vertical
            bolas->at(i).vy *= -1;
        }
        for(int j=i+1; j<quant_bolas; j++){ //percorre todas as bolas sucessivas verificando se há colisão
            double dis_x = bolas->at(i).x - bolas->at(j).x;
            double dis_y = (bolas->at(i).y - bolas->at(j).y);
            double dis = sqrt(dis_x*dis_x + dis_y*dis_y);
            if(dis<=2*r_bola){ //detecta colisão com outra bola
                calculaColisaoEntreBolas(&bolas->at(i), &bolas->at(j), dis_x, dis_y);
            }
        }
    }
}

void calculaColisaoEntreBolas(Bola *bola1, Bola *bola2, double nx, double ny){
    double vcmx = (bola1->m*bola1->vx + bola2->m*bola2->vx) / (bola1->m + bola2->m);
    double vcmy = (bola1->m*bola1->vy + bola2->m*bola2->vy) / (bola1->m + bola2->m);
    nx /= sqrt(nx*nx + ny*ny);
    ny /= sqrt(nx*nx + ny*ny);
    double vcmn = vcmx*nx + vcmy*ny;
    double v1n = bola1->vx*nx + bola1->vy*ny;
    double v2n = bola2->vx*nx + bola2->vy*ny;
    double deltaV1 = 2*(vcmn - v1n);
    double deltaV2 = 2*(vcmn - v2n);
    bola1->vx += deltaV1*nx;
    bola1->vy += deltaV1*ny;
    bola2->vx += deltaV2*nx;
    bola2->vy += deltaV2*ny;
    
}

