#pragma once

#include "vector.h"

enum class CellState{
    HIDDEN,
    REVEALED,
    FLAGGED
};

enum class GuessResult{
    SAFE,
    HIT_MINE,
    ALREADY_REVEALED,
    OUT_OF_BOUNDS,
    WON
};

struct Cell{
    bool isMine = false;
    int adjacentMines = 0;
    CellState state = CellState::HIDDEN;
};

class Board{

    public:

        Board(int width, int height, int numMines);

        GuessResult guess(int x, int y);

        void flag(int x, int y);

        void resetBoard();

        const Cell& getCell(int x, int y) const;

        int getWidth() const;
        int getHeight() const;
        int getNumMines() const;
        int getFlagCount() const;
        bool isWon() const;
        bool isLost() const;

    private:
        int width_;
        int height_;
        int numMines_;
        int flagCount_;
        int revealedCount_;
        bool minesPlaced_;
        bool lost_;
        Vector<Vector<Cell>> gameBoard_;

        bool inBounds(int x, int y) const;
        void placeMines(int safeX, int safeY);
        void computeNeighborCounts();
        void floodReveal(int x, int y);
};
