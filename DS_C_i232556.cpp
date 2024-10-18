#include <iostream>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>

using namespace std;

class node
{
public:
    int data;
    node *next;

    node(int v)
    {
        data = v;
        next = nullptr;
    }
};
class stack
{
private:
    node *top;

public:
    stack()
    {
        top = nullptr;
    }
    void push(int value) // first in last out ...insert at tail
    {
        node *temp = new node(value); // temp->data = value;
        temp->next = nullptr;
        if (top == nullptr)
            top = temp;
        else
        {
            temp->next = top;
            top = temp;
        }
        temp = nullptr;
        delete temp;
    }
    void pop(int value) // delete at tail
    {
        node *temp = new node(value);
        if (top == nullptr)
        {
            top = temp;
            return;
        }
        node *ptr = top;
        while (ptr->next != nullptr)
            ptr = ptr->next;
        ptr->next = temp;
        temp = nullptr;
        delete temp;
    }
    void display()
    {
        node *temp = top;
        while (temp != nullptr)
        {
            cout << temp->data << " , ";
            temp = temp->next;
        }
        cout << endl;
    }
};

class Node
{
public:
    bool isplayer, iskey, isdoor, iscoin, isbomb;
    Node *up;
    Node *down;
    Node *right;
    Node *left;

    Node()
    {
        isplayer = iskey = isdoor = iscoin = isbomb = false;
        up = down = right = left = nullptr;
    }
};

template <typename T>
class STACK
{
private:
    struct stacknode
    {
        T data;
        stacknode *next;
        stacknode(T v, stacknode *str = nullptr)
        {
            data = v;
            next = str;
        }
    };
    stacknode *top;

public:
    STACK() { top = nullptr; }
    void push(T value)
    {
        top = new stacknode(value, top);
    }
    void pop()
    {
        if (!isempty())
        {
            stacknode *temp = top;
            top = top->next;
            delete temp;
        }
    }
    T peek()
    {
        if (!isempty())
            return top->data;
        return nullptr;
    }
    bool isempty()
    {
        return top == nullptr;
    }
};

class Player
{
public:
    Node *player_node;
    int player_score;
    int player_moves;
    int undo;
    STACK<Node *> undo_feature; // a stack will be made for undos

    Player(Node *first_node)
    {
        player_node = first_node;
        player_moves = 0;
        player_score = 0;
        undo = 0;
        player_node->isplayer = true;
    }

    void move(Node *nextnode)
    {
        undo_feature.push(player_node); // store the present position of player
        player_node->isplayer = false;
        player_node = nextnode;
        player_node->isplayer = true;
        player_moves--; // shifted to given node and move --
    }
    void undo_z()
    {
        if (!undo_feature.isempty() && undo > 0)
        {
            player_node->isplayer = 0;
            player_node = undo_feature.peek();
            undo_feature.pop();
            player_node->isplayer = true;
            undo--;
        }
    }
};

class Maze
{
public:
    Node *start;
    Node *key;
    Node *door;
    Node *coins;
    Node *bombs;
    Player *player; // player object is created here
    int row_key, col_key, row_exit, col_exit;
    int rows, cols;
    int index_bombs[5][2];
    int index_coins[5][2];

    Maze(int r_, int c_)
    {
        rows = r_;
        cols = c_;
        start = new Node[rows * cols];
        setting_links();
        placeing_keynexit();
        player = new Player(start);
    }

    int absolutevalue(int v)
    {
        if (v < 0)
            return -v; // if - val ,return -v -- = +
        else
            return v;
    }

    void setting_links()
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                Node *newnode = access(i, j);
                if (i > 0)
                    newnode->up = access(i - 1, j);
                if (j > 0)
                    newnode->left = access(i, j - 1);
                if (i < rows - 1)
                    newnode->down = access(i + 1, j);
                if (j < cols - 1)
                    newnode->right = access(i, j + 1);
            }
        }
    }

    void placeing_keynexit()
    {
        srand(time(0));
        int row;
        int col;
        row = rand() % rows;
        col = rand() % cols;
        key = access(row, col);
        key->iskey = 1;

        row_exit = rand() % rows;
        col_exit = rand() % cols;
        while (row_exit == row && col_exit == col)
        {
            row_exit = rand() % rows;
            col_exit = rand() % cols;
        }
        door = access(row, col);
        door->isdoor = 1;
        row_key = row;
        col_key = col;
    }

    int cityblockdistance(Node *n1, Node *n2)
    {
        // converts 1d array to 2d array
        int rn1 = (n1 - start) / cols;
        int cn1 = (n1 - start) % cols;
        int rn2 = (n2 - start) / cols;
        int cn2 = (n2 - start) % cols;
        int d = absolutevalue(rn1 - rn2) + absolutevalue(cn1 - cn2);
        return d;
        // This line calculates the Manhattan Distance between two points (rn1, cn1) and (rn2, cn2) in a 2D grid. The Manhattan Distance is the sum of the absolute differences between the row indices and the column indices of two points. It tells you how far one point is from another if you can only move horizontally or vertically (not diagonally).
    }

    void senser()
    {
        int d = cityblockdistance(player->player_node, key);
        if (!player->undo_feature.isempty() && d < cityblockdistance(player->undo_feature.peek(), key))
        {
            // cout << "Getting closer ! to key";
            mvprintw(rows + 1, 0, "Getting closer ! to key");
        }
        else
        {
            mvprintw(rows + 1, 0, "Getting away from key");
            // cout << "away";
        }
        // refresh();
    }
    Node *access(int r, int c)
    {
        return &start[r * cols + c];
    }

    void print()
    {
        clear();
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                Node *nn = access(i, j);
                move(i, j);
                if (nn->isplayer)
                    printw("P");
                else if (nn->iskey)
                    printw("K");
                else if (nn->isdoor)
                    printw("D");
                else if (nn->isbomb)
                    printw("B");
                else if (nn->iscoin)
                    printw("C");
                else
                    printw(".");
            }
        }
        refresh();
    }
};

class GAME
{
public:
    Maze *maze;
    GAME(int l)
    {
        int grid, moves, undo;
        if (l == 1)
        {
            grid = 10;
            undo = 6;
            moves = 6;
        }
        else if (l == 2)
        {
            grid = 15;
            undo = 4;
            moves = 2;
        }
        else
        {
            grid = 20;
            undo = 1;
            moves = 0;
        }
        maze = new Maze(grid, grid);
        int a = maze->cityblockdistance(maze->player->player_node, maze->key);
        int b = maze->cityblockdistance(maze->key, maze->door);
        maze->player->player_moves = a + b + moves;
        maze->player->player_moves = undo;
    }

    void start()
    {
        int a;
        while (maze->player->player_moves > 0)
        {
            maze->print();
            mvprintw(maze->rows + 2, 0, "MOVES LEFT : %d  UNDO MOVES: %d", maze->player->player_moves, maze->player->undo);
            refresh();

            a = getch();
            if (a == 'u')
            {
                maze->player->undo_z();
            }
            else
            {
                game_move(a);
                maze->senser();
            }
            if (maze->player->player_node == maze->key)
            {
                mvprintw(maze->rows + 3, 0, "KEY = SUCCESS");
                refresh();
            }
            if (maze->player->player_node == maze->door && maze->key->iskey)
            {
                mvprintw(maze->rows + 4, 0, "ESCAPE = SUCCESS");
                refresh();
                break;
            }
        }
        mvprintw(maze->rows + 5, 0, "GAME OVER");
        refresh();
    }

    void game_move(int a)
    {
        Node *nn = nullptr;
        if (a == KEY_UP && maze->player->player_node->up)
            nn = maze->player->player_node->up;
        else if (a == KEY_DOWN && maze->player->player_node->down)
            nn = maze->player->player_node->down;
        else if (a == KEY_LEFT && maze->player->player_node->left)
            nn = maze->player->player_node->left;
        else if (a == KEY_RIGHT && maze->player->player_node->right)
            nn = maze->player->player_node->right;
        if (nn != nullptr)
        {
            maze->player->move(nn);
        }
    }
};

int main()
{
    int i;
    cout << "Select level!!";
    cin >> i;
    initscr();            // Start ncurses mode
    keypad(stdscr, TRUE); // Enable keypad for arrow keys
    noecho();             // Don't show typed characters
    cbreak();             // Disable line buffering

    GAME g(i);
    g.start();

    endwin(); // ending ncurse mode
    return 0;
}