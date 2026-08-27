#include<iostream>
#include <vector>
#include <chrono> 


void Avg(double aos, double soa);
std::pair<double,double> Test_2( int units);
std::pair<double,double> Test_1( int units);
double Test_3(int units);

class AoS{
    public:
    int rand_no = 0;
    int attack = 0;
    int hp = 0;
};

struct hp{
    int health = 0;
};
struct attack{
    int attack = 0;
};
struct rand_no{
    int rand_no = 0;
};

const int batch = 8;
struct AoSoA{

    int rand_no[batch];
    int attack[batch];
    int hp[batch];


};



int main(){
    int num = 1;
    double AoS_T1 = 0.0;
    double SoA_T1 = 0.0;
    std::cout<< "FOLLOWING IS A TEST FOR CACHE EFFICIENCY BETWEEN AoS & SoA. FIRST TEST CONSISTS OF A SINGLE COMPONENT: \n\n";

    while (num != 100000000)
    {
        auto numb =Test_1(num);
        AoS_T1 +=numb.first;
        SoA_T1 +=numb.second;
        num *= 10;
    }
    Avg(AoS_T1,SoA_T1);

    std::cout<< "FOLLOWING IS A 2nd TEST FOR CACHE EFFICIENCY BETWEEN AoS & SoA. SECOND TEST CONSISTS OF MULTIPLE COMPONENTS : \n\n";
    
    int num2 = 1;
    double AoS_T2 = 0.0;
    double SoA_T2 = 0.0;

    while (num2 != 100000000)
    {
        auto numb2 = Test_2(num2);
        AoS_T2 +=numb2.first;
        SoA_T2 +=numb2.second;
        num2 *= 10;
    }
    Avg(AoS_T2,SoA_T2);

    std::cout<< "FOLLOWING IS A 3rd TEST FOR CACHE EFFICIENCY OF AoSoA. ALSO CONSIDERED THE BEST OF BOTH WORLD : \n\n";

    int num3 = 1;
    double AoSoA_T3 = 0.0;
    while (num3 != 100000000)
    {
        double time = Test_3(num3);
        AoSoA_T3 += time;
        num3 *= 10;
    }
    std::cout << "AoSoA AVERAGE - " << AoSoA_T3/8 << " A CLEAR DROP FROM PREVIOUS TESTS, THIS IS TO SHOW THAT AOSOA REQUIRES MUCH SOPHESTICATED LOW LEVEL CODE TO BE WRITTEN." << "\n\n";
}

std::pair<double,double> Test_2( int units){

    std::vector<AoS> oops;
    std::vector<hp> dods1;
    std::vector<attack> dods2;
    std::vector<rand_no> dods3;

    std::cout << "UNITS - " << units << '\n';
    for (size_t i = 0; i < units; i++)
    {
        AoS a;
        oops.push_back(a);
        hp h;
        attack a1;
        rand_no r;
        dods1.push_back(h);
        dods2.push_back(a1);
        dods3.push_back(r);
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < units; i++)
    {
        oops[i].hp += 100;
        oops[i].attack += 100;
        oops[i].rand_no += 100;

    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "TIME TAKEN BY AoS (OOPS): " << duration.count() << '\n';

    auto SoA_start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < units; i++)
    {
        dods1[i].health += 100;
        dods2[i].attack += 100;
        dods3[i].rand_no += 100;
    }
    auto SoA_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> Duration = SoA_end - SoA_start;

    std::cout << "TIME TAKEN BY SoA (DOD): " << Duration.count() << "\n\n";

    return {duration.count(),Duration.count()};
}

std::pair<double,double> Test_1( int units){

    std::vector<AoS> oops;
    std::vector<hp> dods1;
    
    
    std::cout << "UNITS - " << units << '\n';
    for (size_t i = 0; i < units; i++)
    {
        AoS a;
        oops.push_back(a);
        hp h;
        dods1.push_back(h);
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < units; i++)
    {
        oops[i].hp += 100;
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "TIME TAKEN BY AoS (OOPS): " << duration.count() << '\n';

    auto SoA_start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < units; i++)
    {
        dods1[i].health += 100;
    }
    auto SoA_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> Duration = SoA_end - SoA_start;

    std::cout << "TIME TAKEN BY SoA (DOD): " << Duration.count() << "\n\n";
    return {duration.count(),Duration.count()};
}

double Test_3(int units){

    std::vector<AoSoA> data;
    std::cout << "UNITS - " << units << '\n';
    data.resize(units / batch);
   
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < data.size(); i++)
    {
        for (size_t j = 0; j <  batch ; j++)
        {
            data[i].hp[j] += 100;
            data[i].attack[j] += 100;
            data[i].rand_no[j] += 100;
        }
        
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> Duration = end - start;
    std::cout << "TIME TAKEN BY AoSoA: " << Duration.count() << "\n\n";
    return Duration.count();

}




void Avg(double aos, double soa){
    std::cout << "AoS AVERAGE - " << aos/8 << '\n';
    std::cout << "SoA AVERAGE - " << soa/8 << "\n\n";
}


