//
// Created by Jacek on 20.02.2022.
//

#ifndef DILITHIUM_INTERFACE_H
#define DILITHIUM_INTERFACE_H
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <time.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///////////// Interface

///////////// Class announcements

class Buyer;
class Seller;
class BankOfUniverse;

///////////// Transaction

enum class TransactionStatus{
    registered, finalized
};
const std::string statusName[] = {"registered", "finalized"};
struct Transaction{
    int id;
    Buyer *toWhom;
    Seller *fromWho;
    int cnt;
    int itemPrice;
    TransactionStatus status;

    //constructor and copy constructor:
    Transaction(int id, Buyer *toWhom, Seller *fromWhom, int cnt, int itemPrice, TransactionStatus status);
    Transaction(const Transaction&);
};


class Trader{
protected:
    std::string traderName;
    int energyCredit;
    int dilithiumUnits;
    BankOfUniverse& bank;
public:
    std::string name();
    //technical part:
    Trader(std::string traderName, BankOfUniverse &bank, int energyCredit=0, int dilithiumUnits=0);
    Trader(const Trader&) = default;
    Trader& operator=(const Trader&) = delete;
    virtual ~Trader() = default; //prawdopodobnie wywyola podstawowy destruktor dla danej klasy, a co z referencja
};

class Seller: public Trader{
protected:
    int dilithiumPrice;
    virtual int giveDilithium(int cnt); // oddaj dana ilosc dilithium
public:
    virtual int getPrice();
    virtual bool acquire(int cnt)=0;
    virtual int sell(Buyer* toWhom, int cnt); // sprawdz, czy masz wystarczajaco, jak tak, to idz do banku z tym <--- tutaj jest realizowane oddawanie hajsu -> zwraca id
    virtual void takeCredit(int amount); // zbierz hajs za transakce
    //technical part:
    Seller(std::string traderName, BankOfUniverse &bank, int dilithiumPrice, int energyCredit=0, int dilithiumUnits=0):
            Trader(traderName, bank, energyCredit, dilithiumUnits), dilithiumPrice (dilithiumPrice){};
    Seller(const Seller&) = default;
    Seller& operator=(const Seller&) = delete;
    ~Seller() = default;
    friend BankOfUniverse; //oho mamy przyjaciela -> bank moze korzystac z metod sprzedawcy?? ocb
};

class Buyer: public Trader{
    virtual bool accept(int price, int cnt) = 0;
public:
    bool buy(int cnt, Seller *fromWho);  //->spytaj o cene i sprawdz, czy git -> odbierz id (sprzedawca->sprzedaje) -> sprawdz, czy id ok -> idz do banku i finalizuj (bank->finalize)
    virtual int giveCredits(int amount); // -> check if enough credit and give energy point -> or send message and return -1
    virtual void takeDilithium(); // -> check if >0 and change dilithiumUnits
    //technical part:
    Buyer(std::string traderName, BankOfUniverse &bank, int energyCredit=0, int dilithiumUnits=0);
    Buyer(const Buyer&);
    Buyer& operator=(const Buyer&) = delete;
    ~Buyer() override = default;
};

class Ferengi: public Seller{
private:
    std::set<Ferengi*> chain;
public:
    void addNewToGroup(Ferengi*);
    int internalTradePrice();
    bool acquire(int cnt) override;
    int sendMeDilithium(int needed, Ferengi* toWhom);

    //technical:
    Ferengi(std::string traderName, BankOfUniverse &bank, int dilithiumPrice, int energyCredit=0, int dilithiumUnits=0, const std::set<Ferengi*> &chain={}); // -> podczas konstrukcji mogę podać grupe przyjaciol
    Ferengi(const Ferengi&);
    Ferengi& operator=(Ferengi&) = delete;
    ~Ferengi() override = default;
};

class BankOfUniverse{
private:
    std::vector<Transaction> log;
    std::map<int, Transaction> current;
    int curr_id;
    int storedEnergyCredits;
    int storedDilithium;
public:
    int registerTransaction(Seller *pSeller, Buyer *pBuyer, int cnt, int itemPrice);
    bool finalizePurchace(int id, int cnt, int price, Buyer *pBuyer, Seller *pSeller);
    void printLog();
    void printCurrent();
    //technical part;
    explicit BankOfUniverse(int storedEnergyCredits=0, int storedDillithium=0);
    BankOfUniverse(const BankOfUniverse&) = delete; // <------ Tylko jeden jest bank
    BankOfUniverse& operator=(const BankOfUniverse&) = delete;
    ~BankOfUniverse() = default;
};

#endif //DILITHIUM_INTERFACE_H
