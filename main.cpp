#include <iostream>
#include <thread>
#include <vector>
#include <numeric> 
#include <mutex>

class FactorialCalculator {
private:
    unsigned long long number;       
    int num_threads;                  
    std::mutex mtx;                   
    std::vector<unsigned long long> results; 


    void partial_factorial(unsigned long long start, unsigned long long end, unsigned long long &result) {
        unsigned long long partial_result = 1;
        for (unsigned long long i = start; i <= end; ++i) {
            partial_result *= i;
        }
        
        std::lock_guard<std::mutex> lock(mtx);
        result *= partial_result;
    }

public:
   
    FactorialCalculator(unsigned long long num, int threads)
        : number(num), num_threads(threads), results(threads, 1) {}

    unsigned long long calculate() {
        if (number == 0 || number == 1) return 1;

        std::vector<std::thread> threads;       
        auto range = number / num_threads; 
        auto start = 1;

        for (int i = 0; i < num_threads; ++i) {
            auto end = (i == num_threads - 1) ? number : start + range - 1;
            threads.emplace_back(&FactorialCalculator::partial_factorial, this, start, end, std::ref(results[i]));
            start = end + 1;
        }

        // Ожидание завершения потоков
        for (auto &thread : threads) {
            thread.join();
        }

        // Умножаем все результаты потоков
        return std::accumulate(results.begin(), results.end(), 1ULL, std::multiplies<unsigned long long>());
    }
};

int main() {
    unsigned long long number;
    int num_threads;

    std::cout << "Введите число для вычисления факториала: ";
    std::cin >> number;

    std::cout << "Введите количество потоков: ";
    std::cin >> num_threads;

    FactorialCalculator calculator(number, num_threads);
    auto result = calculator.calculate();

    std::cout << "Факториал числа " << number << " равен " << result << std::endl;

    return 0;
}