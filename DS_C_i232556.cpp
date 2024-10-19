// g++ -o a pd.cpp -lncurses && ./a
// git add .
// git commit -m "msg"
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <ncurses.h>

using namespace std;

class node
{
public:
    int data1;
    int data2;
    node *next;

    node(int v, int u)
    {
        data1 = v;
        data2 = u;
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
    void push(int value, int v) // first in last out ...insert at tail
    {
        node *temp = new node(value, v); // temp->data = value;
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
    void pop() // delete at tail
    {
        if (top == nullptr)
            return;
        if (top->next == nullptr)
        {
            delete top;
            top = nullptr;
            return;
        }
        node *ptr = top;
        while (ptr->next->next != nullptr)
            ptr = ptr->next;
        delete ptr->next;
        ptr->next = nullptr;
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
            player_node->isplayer = false;
            player_node = undo_feature.peek(); // Retrieve the last position
            undo_feature.pop();                // Remove it from the stack
            player_node->isplayer = true;
            undo--; // Decrement undo moves
        }
    }
};

class coin
{
public:
    int r, c;
    bool iscoin;
    Coin()
    {
        iscoin = false;
        r = c = -1;
    }
    void activate(int r, int c)
    {
        this->r = r;
        this->c = c;
        iscoin = true;
    }
    void deactivate()
    {
        iscoin = false;
    }
};
class Bomb
{
public:
    int r, c;
    bool isbomb;
    Coin()
    {
        isbomb = false;
        r = c = -1;
    }
    Coin(int r, int c)
    {
        this->r = r;
        this->c = c;
        isbomb = true;
    }
    void deactivate()
    {
        isbomb = false;
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
    // int index_bombs[5][2];

    coin coins[5];
    coin c1, c2, c3, c4, c5;
    stack coin_s;
    int total_coins;
    int collected_coins;

    Maze(int r_, int c_)
    {
        rows = r_;
        cols = c_;
        start = new Node[rows * cols];
        setting_links();
        placeing_keynexit();
        player = new Player(start);

        collected_coins = 0;
        total_coins = 0;
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
        door = access(row_exit, col_exit);
        door->isdoor = 1;
        row_key = row;
        col_key = col;
    }

    void place_coins(int n)
    {
        total_coins = n;

        int row = rand() % rows;
        int col = rand() % cols;
        while (access(row, col)->isdoor == 1 || access(row, col)->iskey == 1 || access(row1, col1)->iscoin == 1)
        {
            row = rand() % rows;
            col = rand() % cols;
        }
        c1.activate(row, col);
        access(row, col)->iscoin = true;

        int row1 = rand() % rows;
        int col1 = rand() % cols;
        while (access(row1, col1)->isdoor == 1 || access(row1, col1)->iskey == 1 || access(row1, col1)->iscoin == 1)
        {
            row1 = rand() % rows;
            col1 = rand() % cols;
        }
        c2.activate(row1, col1);
        access(row1, col1)->iscoin = true;

        int row2 = rand() % rows;
        int col2 = rand() % cols;
        while (access(row2, col2)->isdoor == 1 || access(row2, col2)->iskey == 1 || access(row2, col2)->iscoin == 1)
        {
            row2 = rand() % rows;
            col2 = rand() % cols;
        }
        c2.activate(row2, col2);
        access(row2, col2)->iscoin = true;

        int row3 = rand() % rows;
        int col3 = rand() % cols;
        while (access(row3, col3)->isdoor == 1 || access(row3, col3)->iskey == 1 || access(row3, col3)->iscoin == 1)
        {
            row3 = rand() % rows;
            col3 = rand() % cols;
        }
        c2.activate(row3, col3);
        access(row3, col3)->iscoin = true;

        int row4 = rand() % rows;
        int col4 = rand() % cols;
        while (access(row4, col4)->isdoor == 1 || access(row4, col4)->iskey == 1 || access(row4, col4)->iscoin == 1)
        {
            row4 = rand() % rows;
            col4 = rand() % cols;
        }
        c2.activate(row4, col4);
        access(row4, col4)->iscoin = true;
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
            mvprintw(rows + 4, 0, "Getting closer to key");
        else
            mvprintw(rows + 4, 0, "Getting away from key");
        // cout << "away";
        refresh();
    }
    void senser_door()
    {
        int d = cityblockdistance(player->player_node, door);
        if (!player->undo_feature.isempty() && d < cityblockdistance(player->undo_feature.peek(), key))
            mvprintw(rows + 4, 0, "Getting closer to exit door");
        else
            mvprintw(rows + 4, 0, "Getting away from exit door");
        refresh();
    }

    Node *access(int r, int c)
    {
        return &start[r * cols + c];
    }

    void print()
    {
        clear();
        move(0, 0);

        for (int i = 0; i < cols + 2; i++)
            printw("# ");
        for (int i = 0; i < rows; i++)
        {
            move(i + 1, 0); // Move to the start of the row, adding 1 to leave space for the top boundary
            printw("# ");
            for (int j = 0; j < cols; j++)
            {
                Node *nn = access(i, j);
                move(i + 1, (j + 1) * 2);
                if (nn->isplayer)
                    printw("P ");
                else if (nn->iskey)
                    printw("K ");
                else if (nn->isdoor)
                    printw("D ");
                else if (nn->isbomb)
                    printw("B ");
                else if (nn->iscoin)
                    printw("C ");
                else
                    printw(". ");
            }
            printw("# ");
        }
        move(rows + 1, 0);
        for (int i = 0; i < cols + 2; i++)
            printw("# ");
        if (key->iskey)
            senser();
        else
            senser_door();
        refresh();
    }
};

class GAME
{
public:
    Maze *maze;
    bool iskeygot = false;
    int level;
    GAME(int l)
    {
        int grid, moves, undo;
        if (l == 1)
        {
            grid = 10;
            undo = 6;
            moves = 50;
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
        level = l;
        maze = new Maze(grid, grid);
        int a = maze->cityblockdistance(maze->player->player_node, maze->key);
        int b = maze->cityblockdistance(maze->key, maze->door);
        maze->player->player_moves = a + b + moves;
        maze->player->undo = undo;
    }

    void start()
    {
        int a;
        while (maze->player->player_moves > 0)
        {
            bool check = true;
            maze->print();
            mvprintw(maze->rows + 3, 0, "MOVES LEFT : %d  UNDO MOVES: %d", maze->player->player_moves, maze->player->undo);
            if (level == 1)
                mvprintw(maze->rows + 5, 0, "GAME_MODE : EASY           KEY STATUS : %d", iskeygot);
            else if (level == 2)
                mvprintw(maze->rows + 5, 0, "GAME_MODE : MEDIUM         KEY STATUS : %d", iskeygot);
            else
                mvprintw(maze->rows + 5, 0, "GAME_MODE : HARD           KEY STATUS : %d", iskeygot);
            if (iskeygot && check)
                mvprintw(maze->rows + 6, 0, "KEY got! now find door");

            if (maze->player->player_node == maze->door && iskeygot)
                mvprintw(maze->rows + 7, 0, "YOU ESCAPEd SUCCESS");
            refresh();

            a = getch();
            if (a == 'u')
            {
                maze->player->undo_z();
            }
            else
            {
                game_move(a);
            }
            if (maze->player->player_node == maze->key && !iskeygot)
            {
                refresh();
                iskeygot = true;
                maze->key->iskey = false;
            }
            if (maze->player->player_node == maze->door && iskeygot)
            {
                maze->door->isdoor = false;
                check = false;
                refresh();
                break;
            }
        }
        game_over();
    }

    void game_over()
    {
        clear();
        while (1)
        {
            if (maze->door->isdoor == false)
                mvprintw(0, 0, "YOU WON");
            else
                mvprintw(0, 0, "GAME OVER");
            mvprintw(2, 0, "Press q to exit");
            int a = getch();
            if (a == 'q')
                exit(1);
            refresh();
        }
    }

    void game_move(int a)
    {
        Node *curr = maze->player->player_node;
        // int d1 = maze->cityblockdistance(curr,maze->key);
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
            if (!(nn == maze->player->undo_feature.peek()))
                maze->player->move(nn);
        }
    }
};

int main()
{
    int i;
    cout << "Select level ";
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