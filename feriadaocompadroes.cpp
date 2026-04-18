#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

class Subject;

class Observer { // interface observer
public:
    virtual ~Observer() = default;
    virtual void update(Subject* subject) = 0;
};

class Subject {
private:
    std::vector<Observer*> observers;

public:
    virtual ~Subject() = default;

    void addObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void removeObserver(Observer* observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

protected:
    void notifyObservers() {
        for (Observer* observer : observers) {
            observer->update(this);
        }
    }
};

class EstacaoMonitoramento: public Subject {
private:
    int id;
    std::string nomeEstacao;
    std::string localizacao;
    std::string tipoAmbienteMonitorado;
    double temperatura;
    double umidade;
    double pressao;
    double ph;

public:
    EstacaoMonitoramento(): id(0), nomeEstacao(""), localizacao(""), tipoAmbienteMonitorado(""), temperatura(0.0), umidade(0.0), pressao(0.0), ph(7.0) {}

    void setId(int id) { this->id = id; }
    int getId() const { return id; }

    void setNomeEstacao(const std::string& nomeEstacao) {
        this->nomeEstacao = nomeEstacao;
    }

    std::string getNomeEstacao() const {
        return nomeEstacao;
    }

    void setLocalizacao(const std::string& localizacao) {
        this->localizacao = localizacao;
    }

    std::string getLocalizacao() const {
        return localizacao;
    }

    void setTipoAmbienteMonitorado(const std::string& tipoAmbienteMonitorado) {
        this->tipoAmbienteMonitorado = tipoAmbienteMonitorado;
    }

    std::string getTipoAmbienteMonitorado() const {
        return tipoAmbienteMonitorado;
    }

    double getTemperatura() const { return temperatura; }
    double getUmidade() const { return umidade; }
    double getPressao() const { return pressao; }
    double getPh() const { return ph; }

    bool atualizarLeituras(double temperatura, double umidade, double pressao, double ph) {
        if (umidade < 0.0 || umidade > 100.0) {
            std::cout << "Erro: umidade inválida na estação " << nomeEstacao << ".\n";
            return false;
        }

        if (pressao < 0.0) {
            std::cout << "Erro: pressão inválida na estação " << nomeEstacao << ".\n";
            return false;
        }

        if (ph < 0.0 || ph > 14.0) {
            std::cout << "Erro: pH inválido na estação " << nomeEstacao << ".\n";
            return false;
        }

        this->temperatura = temperatura;
        this->umidade = umidade;
        this->pressao = pressao;
        this->ph = ph;

        notifyObservers();
        return true;
    }
};

class EstacaoMonitoramentoFactory { // factory
public:
    static std::unique_ptr<EstacaoMonitoramento> criarEstacao(int id, const std::string& nome, const std::string& localizacao, 
                                                                                                            const std::string& tipo) {
        auto estacao = std::make_unique<EstacaoMonitoramento>();
        estacao->setId(id);
        estacao->setNomeEstacao(nome);
        estacao->setLocalizacao(localizacao);
        estacao->setTipoAmbienteMonitorado(tipo);
        return estacao;
    }
};

class InstituicaoObserver: public Observer {
private:
    std::string nome;

public:
    InstituicaoObserver() : nome("") {}

    void setNome(const std::string& nome) {
        this->nome = nome;
    }

    std::string getNome() const {
        return nome;
    }

    void update(Subject* subject) override {
        EstacaoMonitoramento* estacao = dynamic_cast<EstacaoMonitoramento*>(subject);
        if (!estacao) return;

        std::cout << nome << " recebeu atualizacao da estacao " << estacao->getNomeEstacao() << " (ID " << estacao->getId() << ")\n";
        std::cout << "Localização: " << estacao->getLocalizacao() << "\n";
        std::cout << "Ambiente: " << estacao->getTipoAmbienteMonitorado() << "\n";
        std::cout << "Temperatura: " << estacao->getTemperatura() << " C\n";
        std::cout << "Umidade: " << estacao->getUmidade() << " %\n";
        std::cout << "Pressão: " << estacao->getPressao() << " hPa\n";
        std::cout << "pH: " << estacao->getPh() << "\n\n";
    }
};

class AlertaObserver: public Observer {
public:
    void update(Subject* subject) override {
        EstacaoMonitoramento* estacao = dynamic_cast<EstacaoMonitoramento*>(subject);
        if (!estacao) return;

        bool alerta = false;

        std::cout << "-Sistema de Alerta- Verificando leituras da estação "
                  << estacao->getNomeEstacao() << "...\n";

        if (estacao->getTemperatura() > 35.0) {
            std::cout << "Alerta: temperatura elevada.\n";
            alerta = true;
        }

        if (estacao->getPh() < 6.0 || estacao->getPh() > 8.5) {
            std::cout << "Alerta: pH fora da faixa esperada.\n";
            alerta = true;
        }

        if (estacao->getUmidade() > 90.0) {
            std::cout << "Alerta: umidade muito alta.\n";
            alerta = true;
        }

        if (!alerta) {
            std::cout << "Nenhuma anomalia detectada.\n";
        }

        std::cout << "\n";
    }
};

class CentralMonitoramento { // singleton
private:
    std::string nomeCentral;
    std::vector<std::unique_ptr<EstacaoMonitoramento>> estacoes;

    CentralMonitoramento(): nomeCentral("Estação Central de Monitoramento") {}
    ~CentralMonitoramento() = default;

    CentralMonitoramento(const CentralMonitoramento&) = delete;
    CentralMonitoramento& operator=(const CentralMonitoramento&) = delete;
    CentralMonitoramento(CentralMonitoramento&&) = delete;
    CentralMonitoramento& operator=(CentralMonitoramento&&) = delete;

public:
    static CentralMonitoramento& getInstance() {
        static CentralMonitoramento instance;
        return instance;
    }

    void addEstacao(std::unique_ptr<EstacaoMonitoramento> estacao) { estacoes.push_back(std::move(estacao)); }
    EstacaoMonitoramento* buscarEstacaoId(int id) {
        for(const auto& estacao: estacoes) {
            if (estacao->getId() == id) {
                return estacao.get();
            }
        }
        return nullptr;
    }
};

int main() {
    CentralMonitoramento& central = CentralMonitoramento::getInstance();

    central.addEstacao(EstacaoMonitoramentoFactory::criarEstacao(1, "Estacao 1", "Porto Triste - RS", "Rio"));
    central.addEstacao(EstacaoMonitoramentoFactory::criarEstacao(2, "Estacao 2", "São José dos Morros - SP", "Lago"));
    central.addEstacao(EstacaoMonitoramentoFactory::criarEstacao(3, "Estacao 3", "Pelotinhas - RS", "Rio"));

    InstituicaoObserver unifesp;
    unifesp.setNome("UNIFESP");

    InstituicaoObserver pucrs;
    pucrs.setNome("PUCRS");

    AlertaObserver alerta;

    EstacaoMonitoramento* estacao1 = central.buscarEstacaoId(1);
    EstacaoMonitoramento* estacao2 = central.buscarEstacaoId(2);
    EstacaoMonitoramento* estacao3 = central.buscarEstacaoId(3);

    if (estacao1 != nullptr) {
        estacao1->addObserver(&unifesp);
        estacao1->addObserver(&pucrs);
        estacao1->addObserver(&alerta);
    }

    if (estacao2 != nullptr) {
        estacao2->addObserver(&unifesp);
        estacao2->addObserver(&alerta);
    }
    
    if (estacao3 != nullptr) {
        estacao3->addObserver(&pucrs);
        estacao3->addObserver(&alerta);
    }
    
    if (estacao1) estacao1->atualizarLeituras(25.0, 68.0, 1013.0, 7.1);
    if (estacao2) estacao2->atualizarLeituras(36.5, 92.0, 1008.0, 5.8);
    if (estacao3) estacao3->atualizarLeituras(22.0, 81.0, 1011.0, 7.4);

    return 0;
}