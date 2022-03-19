#include "Percolation.h"

Percolation::Percolation(const size_t dimension)
    : number_of_open_cells(0)
    , map(dimension, std::vector<bool>(dimension, false))
    , full(dimension, std::vector<bool>(dimension, false))
{
}

void Percolation::open(const size_t row, const size_t column)
{
    if (is_open(row, column))
        return;
    map[row][column] = true;
    number_of_open_cells++;
    if (row == 0) {
        full[row][column] = true;
    }
    else {
        for (size_t i = 0; i < shift_size; i++) {
            if (is_inside(row + row_shift[i], column + column_shift[i]) &&
                is_full(row + row_shift[i], column + column_shift[i])) {
                full[row][column] = true;
            }
        }
    }
    if (full[row][column]) {
        std::deque<std::pair<int, int>> deq;
        deq.push_back({row, column});
        while (!deq.empty()) {
            const int pos_row = deq.front().first;
            const int pos_column = deq.front().second;
            deq.pop_front();
            for (size_t i = 0; i < shift_size; i++) {
                std::pair<int, int> new_pos = {pos_row + row_shift[i], pos_column + column_shift[i]};
                if (is_inside(new_pos.first, new_pos.second) &&
                    is_open(new_pos.first, new_pos.second) &&
                    !is_full(new_pos.first, new_pos.second)) {
                    full[new_pos.first][new_pos.second] = true;
                    deq.push_back({new_pos.first, new_pos.second});
                }
            }
        }
    }
}

bool Percolation::is_open(const size_t row, const size_t column) const
{
    return map[row][column];
}

bool Percolation::is_full(const size_t row, const size_t column) const
{
    return full[row][column];
}

size_t Percolation::get_numbet_of_open_cells() const
{
    return number_of_open_cells;
}

bool Percolation::has_percolation() const
{
    for (size_t i = 0; i < map.back().size(); i++) {
        if (is_full(map.size() - 1, i)) {
            return true;
        }
    }
    return false;
}
bool Percolation::is_inside(const size_t row, const size_t column) const
{
    return row < map.size() && column < map.back().size();
}
