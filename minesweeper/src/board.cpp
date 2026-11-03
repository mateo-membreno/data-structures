#include "board.h"
#include "vector.h"

#include <random>

Board::Board(int width, int height, int numMines) : 
width_(width), 
height_(height), 
numMines_(numMines),
flagCount_(0),
revealedCount_(0),
minesPlaced_(false),
lost_(false),
gameBoard_(width, Vector<Cell>(height, Cell()))
{
    if (numMines_ > width_ * height_ - 1) numMines_ = width_ * height_ - 1;
    if (numMines_ < 0) numMines_ = 0;
}

GuessResult Board::guess(int x, int y){
    if (!inBounds(x, y)) return GuessResult::OUT_OF_BOUNDS;

    Cell& cell = gameBoard_[x][y];

    if (cell.state == CellState::REVEALED) return GuessResult::ALREADY_REVEALED;
   
    if (cell.state == CellState::FLAGGED) return GuessResult::SAFE;

    if (!minesPlaced_){
        placeMines(x, y);
        computeNeighborCounts();
        minesPlaced_ = true;
    }

    if (cell.isMine){
        cell.state = CellState::REVEALED;
        lost_ = true;
        return GuessResult::HIT_MINE;
    }

    floodReveal(x, y);

    if (isWon()) return GuessResult::WON;
    return GuessResult::SAFE;
}

void Board::flag(int x, int y){
    if(!inBounds(x,y)) return;

    if(gameBoard_[x][y].state == CellState::HIDDEN && flagCount_ < numMines_){
        gameBoard_[x][y].state = CellState::FLAGGED;
        flagCount_++;
        return;
    }
    else if (gameBoard_[x][y].state == CellState::FLAGGED){
        gameBoard_[x][y].state = CellState::HIDDEN;
        flagCount_--;
        return;
    }
    return;
}

void Board::resetBoard(){
    for (int i = 0; i < width_; i++){
        for (int j = 0; j < height_; j++){
            gameBoard_[i][j].isMine = false;
            gameBoard_[i][j].adjacentMines = 0;
            gameBoard_[i][j].state = CellState::HIDDEN;
        }
    }
    flagCount_ = 0;
    revealedCount_ = 0;
    minesPlaced_ = false;
    lost_ = false;
}

const Cell& Board::getCell(int x, int y) const{
    return gameBoard_[x][y];
}

int Board::getWidth() const{
    return width_;
}

int Board::getHeight() const{
    return height_;
}

int Board::getNumMines() const{
    return numMines_;
}

int Board::getFlagCount() const{
    return flagCount_;
}

bool Board::isWon() const{
    return !lost_ && minesPlaced_ && revealedCount_ == width_ * height_ - numMines_;
}

bool Board::isLost() const{
    return lost_;
}

bool Board::inBounds(int x, int y) const{
    if (x >= 0 && x < width_ && y >= 0 && y < height_){
        return true;
    }
    return false;
}

void Board::placeMines(int safeX, int safeY){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distX(0, width_ - 1);
    std::uniform_int_distribution<int> distY(0, height_ - 1);

    int placed = 0;
    while (placed < numMines_){
        int x = distX(gen);
        int y = distY(gen);
        if ((x == safeX && y == safeY) || gameBoard_[x][y].isMine){
            continue;
        }
        gameBoard_[x][y].isMine = true;
        placed++;
    }
}

void Board::computeNeighborCounts(){
    for (int x = 0; x < width_; x++){
        for (int y = 0; y < height_; y++){
            if (gameBoard_[x][y].isMine) continue;

            int count = 0;
            for (int dx = -1; dx <= 1; dx++){
                for (int dy = -1; dy <= 1; dy++){
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx;
                    int ny = y + dy;
                    if (inBounds(nx, ny) && gameBoard_[nx][ny].isMine){
                        count++;
                    }
                }
            }
            gameBoard_[x][y].adjacentMines = count;
        }
    }
}

void Board::floodReveal(int x, int y){
    Vector<int> stack;
    gameBoard_[x][y].state = CellState::REVEALED;
    revealedCount_++;
    stack.push_back(x * height_ + y);

    while (!stack.empty()){
        int code = stack[stack.size() - 1];
        stack.pop_back();
        int cx = code / height_;
        int cy = code % height_;

        if (gameBoard_[cx][cy].adjacentMines != 0) continue;

        for (int dx = -1; dx <= 1; dx++){
            for (int dy = -1; dy <= 1; dy++){
                int nx = cx + dx;
                int ny = cy + dy;
                if (!inBounds(nx, ny)) continue;
                Cell& n = gameBoard_[nx][ny];
                if (n.state != CellState::HIDDEN || n.isMine) continue;
                n.state = CellState::REVEALED;
                revealedCount_++;
                stack.push_back(nx * height_ + ny);
            }
        }
    }
}

