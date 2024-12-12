#include <iostream>
#include <vector>
#include <thread>
#include <conio.h>
#include <random>

using namespace std;

const int ROWS = 20;
const int COLS = 20;
const int MINE_COUNT = 30;

int minesLeft = MINE_COUNT; // winning condition for the game. All tiles except mines should be flipped
int spaceCount = 0;         // spacing of mine tiles
vector<array<int, 2>> minePositions;
bool gameLost = false;

int directions[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

// -3 -> marked mine
// -2 -> empty space
// -1 -> mine
// 0 -> square (not opened)
// 1-9 -> number of mines nearby

int gridValues[ROWS][COLS];  // store values of the minefield
int gridDisplay[ROWS][COLS]; // grid meant for displaying
int spotsLeft = ROWS * COLS;

// initialize grid values after randomizing mine positions
// contains values of tiles based on mines
void initializeGridValues()
{
    int i, j;

    for (i = 0; i < minePositions.size(); i++)
    {
        gridValues[minePositions[i][0]][minePositions[i][1]] = -1;
    }

    for (i = 0; i < ROWS; i++)
    {

        for (j = 0; j < COLS; j++)
        {
            if (gridValues[i][j] != -1)
            {
                int mineCount = 0;

                for (int k = 0; k < 8; k++)
                {
                    int x = directions[k][0] + i;
                    int y = directions[k][1] + j;
                    if (x >= 0 && x < ROWS && y >= 0 && y < COLS && gridValues[x][y] == -1)
                    {
                        mineCount++;
                    }
                }
                gridValues[i][j] = mineCount;
            }
        }
    }
}

// all tiles unflipped (value = 0)
void initializeGridDisplay()
{
    int i, j;
    for (i = 0; i < ROWS; i++)
    {
        for (j = 0; j < COLS; j++)
        {
            gridDisplay[i][j] = 0; // square
        }
    }
}

// create mines at random positions.
void initializeMines()
{
    int max = ROWS * COLS;
    int count = 0;

    while (count < MINE_COUNT)
    {
        // Create a random device and a Mersenne Twister engine
        random_device rd;  // Seed for the random number engine
        mt19937 gen(rd()); // Mersenne Twister engine

        // Define a uniform integer distribution between 0 and ROWS-1/COLS-1
        uniform_int_distribution<> randRows(0, ROWS - 1);
        uniform_int_distribution<> randCols(0, COLS - 1);

        // Generate random row and col
        int randomRow = randRows(gen);
        int randomCol = randCols(gen);

        minePositions.insert(minePositions.end(), {randomRow, randomCol});
        count++;
    }
}

// Print the grid (the game uses this print)
// -2 -> empty space
// -1 -> mine
// 0 -> square (not opened)
// 1-9 -> number of mines nearby
void printGrid()
{

    system("cls");
    int i, j;
    cout << "\n";
    int tileCount = 0;
    for (i = 0; i < ROWS; i++)
    {
        for (j = 0; j < COLS; j++)
        {
            tileCount++;

            // not opened
            if (gridDisplay[i][j] == 0)
            {
                cout << "[";
                int tileCopy = tileCount;
                int digitSpaces = 0;
                while (tileCopy > 0)
                {
                    digitSpaces++;
                    tileCopy = int(tileCopy / 10);
                }
                for (int x = 0; x < spaceCount - digitSpaces; x++)
                {
                    cout << " ";
                }
                cout << tileCount << "]";
            }

            // opened and is empty
            else if (gridDisplay[i][j] == -2)
            {
                for (int x = 0; x < spaceCount + 2; x++)
                {
                    cout << "_";
                }
            }

            // mine. GAME OVER
            else if (gridDisplay[i][j] == -1)
            {
                for (int x = 0; x < spaceCount + 1; x++)
                {
                    cout << " ";
                }
                cout << "X";
            }

            // value
            else
            {
                for (int x = 0; x < spaceCount + 1; x++)
                {
                    cout << " ";
                }
                cout << gridDisplay[i][j];
            }

            cout << " ";
        }
        cout << endl;
    }
}

// print gridValues as it is
void printValues()
{
    cout
        << "\nValues grid\n";
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            cout << " " << gridValues[i][j];
        }
        cout << "\n";
    }
    cout << endl;
}

// expand open spaces which are connected to each other
void openSpace(int x, int y)
{
    if (x < 0 || x >= ROWS || y < 0 || y >= COLS || gridValues[x][y] != 0 || gridDisplay[x][y] == -2)
    {
        return;
    }
    gridDisplay[x][y] = -2;
    spotsLeft--;
    for (int i = 0; i < 8; i++)
    {
        openSpace(x + directions[i][0], y + directions[i][1]);
    }
}

// play the game
void playGame()
{
    int tileEntered = -1, totalTiles = ROWS * COLS;
    int row, col;
    bool entryValid = false;

    // tileEntered > totalTiles || tileEntered < 1 || gridDisplay[row][col] != 0
    while (!entryValid)
    {
        cout << "Please enter a valid tile number: ";
        cin >> tileEntered;
        // obtain row and column position in grid
        col = ((tileEntered - 1) % COLS);
        row = int(tileEntered / COLS);

        if (tileEntered % COLS == 0)
        {
            row--;
        }

        if (!cin.fail() && tileEntered <= totalTiles && tileEntered >= 1 && gridDisplay[row][col] == 0)
        {
            entryValid = true;
        }
        else
        {
            cout << "\nInvalid input!\n";
        }
    }

    // mine
    if (gridValues[row][col] == -1)
    {
        gameLost = true;
        gridDisplay[row][col] = -1;
    }

    // open space
    else if (gridValues[row][col] == 0)
    {
        openSpace(row, col);
    }

    else
    {
        gridDisplay[row][col] = gridValues[row][col];
        spotsLeft--;
    }
}

// print gridDisplay as it is
void printGridDisplay()
{
    cout << "\nDisplay grid:\n";
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            cout << " " << gridDisplay[i][j];
        }
        cout << "\n";
    }
}

int main()
{

    cout << "\nWelcome to Minesweeper!!\n\n";
    cout << "INSTRUCTIONS\n\n"
         << "- Pick a tile to flip based on the number of the tile. Make sure to choose valid tiles (those which are unflipped)\n";
    cout << "- Landing on a mine will end the game\n"
         << "- When all tiles except the mines are flipped, you will win the game\n\n";
    cout << "Press any key to start" << endl;

    getch();

    // spacing of characters in the grid
    int totalTiles = ROWS * COLS;
    while (totalTiles > 0)
    {
        spaceCount++;
        totalTiles = int(totalTiles / 10);
    }
    // spaceCount += 2; // adding 2 more for [ and ]

    initializeMines();
    initializeGridDisplay();
    initializeGridValues();
    printGrid();

    // printGridDisplay();
    // printValues();

    while (!gameLost && spotsLeft > MINE_COUNT)
    {
        playGame();
        printGrid();

        // printGridDisplay();
        // printValues();
    }
    if (gameLost)
    {
        cout << "\nYou landed on a mine!! GAME OVER!!\n"
             << endl;
    }
    else
    {
        cout << "\nCONGRATULATIONS!! YOU WON!!\n"
             << endl;
    }

    getch();
    return 0;
}