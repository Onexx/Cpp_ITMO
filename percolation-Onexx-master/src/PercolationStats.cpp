#include "PercolationStats.h"

#include "Percolation.h"

#include <random>

PercolationStats::PercolationStats(const size_t dimension, const size_t trials)
    : dimension(dimension)
    , trials(trials)
{
    execute();
}

double PercolationStats::get_mean() const
{
    return mean;
}

double PercolationStats::get_standard_deviation() const
{
    return standard_deviation;
}

double PercolationStats::get_confidence_low() const
{
    return confidence_low;
}

double PercolationStats::get_confidence_high() const
{
    return confidence_high;
}

void PercolationStats::execute()
{
    std::mt19937 rnd{std::random_device{}()};

    for (size_t i = 0; i < trials; i++) {
        Percolation percolation(dimension);

        std::vector<std::pair<size_t, size_t>> rnd_pool;
        for (size_t row = 0; row < dimension; row++) {
            for (size_t column = 0; column < dimension; column++) {
                rnd_pool.push_back({row, column});
            }
        }
        while (!percolation.has_percolation()) {
            std::uniform_real_distribution<> dist(0, rnd_pool.size() - 1);
            const size_t rnd_idx = dist(rnd);
            const auto [row, column] = rnd_pool[rnd_idx];
            percolation.open(row, column);
            std::swap(rnd_pool[rnd_idx], rnd_pool.back());
            rnd_pool.pop_back();
        }
        const double x_i = static_cast<double>(percolation.get_numbet_of_open_cells()) / (dimension * dimension);
        x.push_back(x_i);
        mean += x_i;
    }
    mean /= trials;
    for (const auto x_i : x) {
        standard_deviation += pow(x_i - mean, 2.0);
    }
    standard_deviation /= trials - 1;
    standard_deviation = sqrt(standard_deviation);

    confidence_low = mean - 1.96 * standard_deviation / sqrt(trials);
    confidence_high = mean + 1.96 * standard_deviation / sqrt(trials);
}
