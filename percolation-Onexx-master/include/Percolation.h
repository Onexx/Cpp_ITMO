#pragma once

#include <deque>
#include <stdio.h>
#include <vector>

struct Percolation
{
    /**
     * Construct a new Percolation object
     * @param dimension dimension of percolation table
     */
    Percolation(size_t dimension);

    /**
     * Opens the cell[row, column] if it's not opened already
     * @param row row index
     * @param column column index
     */
    void open(size_t row, size_t column);

    /**
     * Checks if cell[row, column] is open
     * @param row row index
     * @param column column index
     * @return true cell is open
     * @return false cell is blocked
     */
    bool is_open(size_t row, size_t column) const;

    /**
     * Checks if cell[row, column] is full
     * @param row row index
     * @param column column index
     * @return true cell is full
     * @return false cell is not full
     */
    bool is_full(size_t row, size_t column) const;

    /**
     * Gets number of open cells in table
     * @return number of open cells
     */
    size_t get_numbet_of_open_cells() const;

    /**
     * Checks if system percolates
     * @return true system percolates
     * @return false system doesn't percolate
     */
    bool has_percolation() const;

private:
    size_t number_of_open_cells;
    std::vector<std::vector<bool>> map;
    std::vector<std::vector<bool>> full;

    const size_t shift_size = 4;
    static constexpr long row_shift[4] = {1, -1, 0, 0};
    static constexpr long column_shift[4] = {0, 0, 1, -1};

private:
    bool is_inside(size_t row, size_t column) const;
};