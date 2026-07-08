#include "GeneticAlgorithm.h"
#include "Game.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>

// 全局随机数引擎
static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

// 个体结构
struct Individual {
    float weights[6];
    float fitness;
};

// 生成随机个体
static Individual randomIndividual() {
    Individual ind;
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
    for (int i = 0; i < 6; ++i) ind.weights[i] = dist(rng);
    ind.fitness = 0;
    return ind;
}

// 变异
static void mutate(Individual& ind, float rate = 0.1) {
    std::normal_distribution<float> noise(0.0f, 0.5f);
    for (int i = 0; i < 6; ++i) {
        if (std::uniform_real_distribution<float>(0,1)(rng) < rate) {
            ind.weights[i] += noise(rng);
            ind.weights[i] = std::clamp(ind.weights[i], -15.0f, 15.0f);
        }
    }
}

// 交叉
static Individual crossover(const Individual& a, const Individual& b) {
    Individual child;
    int point = rng() % 6;
    for (int i = 0; i < 6; ++i) {
        child.weights[i] = (i < point) ? a.weights[i] : b.weights[i];
    }
    return child;
}

// 遗传算法主函数
void runGA() {
    int populationSize = 30;
    int generations = 20;
    int gamesPerIndividual = 2;

    // 创建无窗口的 Game 对象（需要 Game 支持 headless 构造函数）
    Game game(true);

    // 初始化种群
    std::vector<Individual> population;
    for (int i = 0; i < populationSize; ++i) {
        population.push_back(randomIndividual());
    }

    for (int gen = 0; gen < generations; ++gen) {
        // 评估适应度
        for (auto& ind : population) {
            float totalScore = 0;
            for (int g = 0; g < gamesPerIndividual; ++g) {
                totalScore += game.simulateOneGame(ind.weights);
            }
            ind.fitness = totalScore / gamesPerIndividual;
        }

        // 按适应度降序排序
        std::sort(population.begin(), population.end(),
            [](const Individual& a, const Individual& b) {
                return a.fitness > b.fitness;
            });

        std::cout << "Generation " << gen
                  << " best fitness: " << population[0].fitness << std::endl;

        // 精英选择（前20%）
        int eliteCount = static_cast<int>(populationSize * 0.2);
        std::vector<Individual> newPopulation;
        for (int i = 0; i < eliteCount; ++i) {
            newPopulation.push_back(population[i]);
        }

        // 锦标赛选择填充剩余
        while (newPopulation.size() < populationSize) {
            int i1 = rng() % populationSize;
            int i2 = rng() % populationSize;
            const Individual& parent1 = (population[i1].fitness > population[i2].fitness)
                                        ? population[i1] : population[i2];

            i1 = rng() % populationSize;
            i2 = rng() % populationSize;
            const Individual& parent2 = (population[i1].fitness > population[i2].fitness)
                                        ? population[i1] : population[i2];

            Individual child = crossover(parent1, parent2);
            mutate(child);
            newPopulation.push_back(child);
        }

        population = std::move(newPopulation);
    }

    // 输出最优权重
    std::cout << "\nOptimal weights found:\n";
    std::cout << "Landing Height: " << population[0].weights[0] << "\n";
    std::cout << "Rows Eliminated: " << population[0].weights[1] << "\n";
    std::cout << "Row Transitions: " << population[0].weights[2] << "\n";
    std::cout << "Col Transitions: " << population[0].weights[3] << "\n";
    std::cout << "Holes: " << population[0].weights[4] << "\n";
    std::cout << "Well Sums: " << population[0].weights[5] << "\n";
}