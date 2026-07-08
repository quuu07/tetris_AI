#include "GeneticAlgorithm.h"
#include "Game.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <windows.h>

static std::string getExecutableDirectory() {
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (length == 0 || length == MAX_PATH) {
        return {};
    }
    std::filesystem::path exePath(buffer);
    return exePath.parent_path().string();
}

static std::filesystem::path getGAWeightPath() {
    return std::filesystem::path(getExecutableDirectory()) / "ga_best_weights.txt";
}

// 全局随机数引擎
static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

// 个体结构
struct Individual {
    float weights[6];
    float fitness;
};

static const float kSeedWeights[6] = {
    6.92504f, 1.10562f, -1.02271f, -9.26922f, -1.71044f, -3.80692f
};

static void saveBestWeights(const Individual& best) {
    std::filesystem::path weightPath = getGAWeightPath();
    std::ofstream out(weightPath, std::ios::trunc);
    if (!out) {
        std::cerr << "Failed to open " << weightPath.string() << " for writing.\n";
        return;
    }
    out << best.weights[0] << ' ' << best.weights[1] << ' ' << best.weights[2] << ' '
        << best.weights[3] << ' ' << best.weights[4] << ' ' << best.weights[5] << '\n';
    out.close();
    std::cout << "Saved best weights to " << weightPath.string() << "\n";
}

// 生成随机个体，优先围绕当前较好的权重做局部搜索
static Individual randomIndividual(bool aroundSeed) {
    Individual ind;
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
    std::normal_distribution<float> noise(0.0f, 1.2f);
    for (int i = 0; i < 6; ++i) {
        if (aroundSeed) {
            ind.weights[i] = kSeedWeights[i] + noise(rng);
        } else {
            ind.weights[i] = dist(rng);
        }
        ind.weights[i] = std::clamp(ind.weights[i], -20.0f, 20.0f);
    }
    ind.fitness = 0.0f;
    return ind;
}

// 变异：更适合连续值调参
static void mutate(Individual& ind, float rate = 0.25f) {
    std::normal_distribution<float> noise(0.0f, 0.8f);
    for (int i = 0; i < 6; ++i) {
        if (std::uniform_real_distribution<float>(0.0f, 1.0f)(rng) < rate) {
            ind.weights[i] += noise(rng);
            ind.weights[i] = std::clamp(ind.weights[i], -20.0f, 20.0f);
        }
    }
}

// 交叉：使用线性融合，避免单点交叉过于离散
static Individual crossover(const Individual& a, const Individual& b) {
    Individual child;
    std::uniform_real_distribution<float> blend(0.0f, 1.0f);
    std::normal_distribution<float> noise(0.0f, 0.4f);
    for (int i = 0; i < 6; ++i) {
        float alpha = blend(rng);
        child.weights[i] = alpha * a.weights[i] + (1.0f - alpha) * b.weights[i];
        if (std::uniform_real_distribution<float>(0.0f, 1.0f)(rng) < 0.05f) {
            child.weights[i] += noise(rng);
        }
        child.weights[i] = std::clamp(child.weights[i], -20.0f, 20.0f);
    }
    return child;
}

// 遗传算法主函数
void runGA() {
    constexpr int populationSize = 48;
    constexpr int generations = 30;
    constexpr int gamesPerIndividual = 4;

    Game game(true);
    std::vector<Individual> population;
    for (int i = 0; i < populationSize; ++i) {
        population.push_back(randomIndividual(i < populationSize / 2));
    }

    for (int gen = 0; gen < generations; ++gen) {
        for (std::size_t idx = 0; idx < population.size(); ++idx) {
            Individual& ind = population[idx];
            float totalScore = 0.0f;
            for (int g = 0; g < gamesPerIndividual; ++g) {
                const int seed = 20260708 + gen * 1000 + g * 100 + static_cast<int>(idx) * 17;
                std::srand(seed);
                totalScore += game.simulateOneGame(ind.weights);
            }
            ind.fitness = totalScore / static_cast<float>(gamesPerIndividual);
        }

        std::sort(population.begin(), population.end(),
            [](const Individual& a, const Individual& b) {
                return a.fitness > b.fitness;
            });

        std::cout << "Generation " << gen
                  << " best fitness: " << population[0].fitness << std::endl;

        int eliteCount = std::max(2, populationSize / 8);
        std::vector<Individual> newPopulation;
        for (int i = 0; i < eliteCount; ++i) {
            newPopulation.push_back(population[i]);
        }

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
            mutate(child, 0.2f + 0.01f * (generations - gen));
            newPopulation.push_back(child);
        }

        population = std::move(newPopulation);
    }

    std::cout << "\nOptimal weights found:\n";
    std::cout << "Landing Height: " << population[0].weights[0] << "\n";
    std::cout << "Rows Eliminated: " << population[0].weights[1] << "\n";
    std::cout << "Row Transitions: " << population[0].weights[2] << "\n";
    std::cout << "Col Transitions: " << population[0].weights[3] << "\n";
    std::cout << "Holes: " << population[0].weights[4] << "\n";
    std::cout << "Well Sums: " << population[0].weights[5] << "\n";

    // 保存最优权重到可执行文件同目录下的 ga_best_weights.txt
    saveBestWeights(population[0]);
}