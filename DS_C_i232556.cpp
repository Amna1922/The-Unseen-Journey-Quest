// g++ -o a pd.cpp -lncurses && ./a
// git add .
// git commit -m "msg"

#include <iostream>
#include <cstdlib>
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
        node *temp = top;
        top = top->next;
        delete temp;
    }
    void display()
    {
        // node *temp = top;
        move(9, 0);
        reversee(top);
        printw("\n");
        // while (temp != nullptr)
        // {
        //     printw(" [%d,%d] ", temp->data1, temp->data2);
        //     temp = temp->next;
        // }
        refresh();
    }
    void reversee(node *temp)
    {
        if (temp == nullptr)
            return;
        reversee(temp->next);
        printw(" [%d,%d] ", temp->data1, temp->data2);
        refresh();
    }
};

class Node
{
public:
    bool isplayer, iskey, isdoor, iscoin, isbomb;
    int r, c;
    Node *up;
    Node *down;
    Node *right;
    Node *left;

    Node()
    {
        isplayer = iskey = isdoor = iscoin = isbomb = false;
        up = down = right = left = nullptr;
        r = c = -1;
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

class Maze
{
public:
    Node *start;
    Node *key;
    Node *door;
    Node *bombs;
    Player *player; // player object is created here
    int rows, cols;
    Node *c1, *c2, *c3, *c4, *c5;
    Node *b1, *b2, *b3, *b4, *b5;
    stack coin_s;
    int collected_coins;
    int row_exit, col_exit, row_key, col_key;
    time_t coin_timer;

    Maze(int r_, int c_)
    {
        rows = r_;
        cols = c_;
        start = new Node[rows * cols];
        setting_links();
        placeing_keynexit();
        place_coins();
        place_bombs();
        player = new Player(start);
        collected_coins = 0;
        coin_timer = time(0);
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

    void place_coins()
    {
        srand(time(0));
        c1 = place_coin();
        c2 = place_coin();
        c3 = place_coin();
        c4 = place_coin();
        c5 = place_coin();
    }
    void place_bombs()
    {
        srand(time(0));
        b1 = place_bomb();
        b2 = place_bomb();
        b3 = place_bomb();
        b4 = place_bomb();
        b5 = place_bomb();
    }
    void remove_coins_bombs()
    {
        c1->iscoin = 0;
        c2->iscoin = 0;
        c3->iscoin = 0;
        c4->iscoin = 0;
        c5->iscoin = 0;

        b1->isbomb = 0;
        b2->isbomb = 0;
        b3->isbomb = 0;
        b4->isbomb = 0;
        b5->isbomb = 0;
    }

    Node *place_coin()
    {
        int row, col;
        do
        {
            row = rand() % rows;
            col = rand() % cols;
        } while (access(row, col)->isplayer == 1 || access(row, col)->isdoor == 1 || access(row, col)->iskey == 1 || access(row, col)->iscoin == 1 || access(row, col)->isbomb == 1);
        Node *c = access(row, col);
        c->iscoin = true;
        c->r = row;
        c->c = col;
        return c;
    }
    Node *place_bomb()
    {
        int row, col;
        do
        {
            row = rand() % rows;
            col = rand() % cols;
        } while (access(row, col)->isplayer == 1 || access(row, col)->isdoor == 1 || access(row, col)->iskey == 1 || access(row, col)->iscoin == 1 || access(row, col)->isbomb == 1);
        Node *c = access(row, col);
        c->isbomb = true;
        c->r = row;
        c->c = col;
        return c;
    }

    void update_coins_n_bombs()
    {
        if (difftime(time(0), coin_timer) >= 10)
        {
            remove_coins_bombs();
            place_coins();
            place_bombs();
            coin_timer = time(0);
        }
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
        attron(COLOR_PAIR(5));
        if (!player->undo_feature.isempty() && d < cityblockdistance(player->undo_feature.peek(), key))
            mvprintw(6, 0, "        Getting CLOSER to key");
        else
            mvprintw(6, 0, "        Getting AWAY from key");
        attroff(COLOR_PAIR(5));
        // cout << "away";
        refresh();
    }
    void senser_door()
    {
        int d = cityblockdistance(player->player_node, door);
        attron(COLOR_PAIR(5));
        if (!player->undo_feature.isempty() && d < cityblockdistance(player->undo_feature.peek(), key))
            mvprintw(6, 0, "        Getting CLOSER to exit door");
        else
            mvprintw(6, 0, "        Getting AWAY from exit door");
        attroff(COLOR_PAIR(5));
        refresh();
    }

    Node *access(int r, int c)
    {
        return &start[r * cols + c];
    }

    void collect_coins(Node *c)
    {
        if (c->iscoin == 1)
        {
            c->iscoin = 0;
            player->undo++;
            player->player_score += 2;
            collected_coins++;
            coin_s.push(c->r, c->c);
            // coin_s.display();
        }
    }

    void print()
    {
        clear();
        move(11, 11);

        for (int i = 0; i < cols + 2; i++)
            printw("# ");
        printw("\n");

        for (int i = 0; i < rows; i++)
        {
            printw("           # ");
            for (int j = 0; j < cols; j++)
            {
                Node *nn = access(i, j);
                if (nn->isplayer)
                {
                    attron(COLOR_PAIR(3));
                    printw("P "); // red
                    attroff(COLOR_PAIR(3));
                }
                // 2 . green _ 4.red
                else if (nn->iskey)
                    printw("K ");
                else if (nn->isdoor)
                    printw("D ");
                else if (nn->isbomb)
                {
                    attron(COLOR_PAIR(1));
                    printw("B ");
                    attroff(COLOR_PAIR(1));
                }
                else if (nn->iscoin)
                {
                    attron(COLOR_PAIR(4));
                    printw("C "); // yellow
                    attroff(COLOR_PAIR(4));
                }
                else
                    printw(". ");
            }
            printw("# \n"); // green
        }
        printw("           ");
        for (int i = 0; i < cols + 2; i++)
            printw("# ");
        printw("\n");
        if (key->iskey)
            senser();
        else
            senser_door();
        // refresh();
    }

    void print_gameover()
    {
        // clear();
        move(11, 11);

        for (int i = 0; i < cols + 2; i++)
            printw("# ");
        printw("\n");

        for (int i = 0; i < rows; i++)
        {
            printw("           # ");
            for (int j = 0; j < cols; j++)
            {
                Node *nn = access(i, j);
                if (nn->isplayer)
                {
                    attron(COLOR_PAIR(3));
                    printw("P "); // red
                    attroff(COLOR_PAIR(3));
                }
                // 2 . green _ 4.red
                else if (nn->iscoin)
                    printw("C ");
                else if (nn->isdoor)
                {
                    attron(COLOR_PAIR(4));
                    printw("D "); // yellow
                    attroff(COLOR_PAIR(4));
                }
                else if (nn->isbomb)
                    printw("B ");
                else if (nn->iskey)
                {
                    attron(COLOR_PAIR(4));
                    printw("K "); // yellow
                    attroff(COLOR_PAIR(4));
                }
                else
                    printw(". ");
            }
            printw("# \n"); // green
        }
        printw("           ");
        for (int i = 0; i < cols + 2; i++)
            printw("# ");
        printw("\n");
        // refresh();
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
            maze->update_coins_n_bombs();
            maze->print();

            mvprintw(0, 0, "__________________________________________________");
            attron(COLOR_PAIR(5));
            if (level == 1)
                mvprintw(2, 0, "               GAME_MODE : EASY           ");
            else if (level == 2)
                mvprintw(2, 0, "                GAME_MODE : MEDIUM         ");
            else
                mvprintw(2, 0, "               GAME_MODE : HARD           ");

            mvprintw(3, 0, "        MOVES LEFT : %d   UNDO MOVES: %d", maze->player->player_moves, maze->player->undo);
            mvprintw(4, 0, "        COINS : %d         SCORE: %d", maze->collected_coins, maze->player->player_score);
            mvprintw(5, 0, "                   KEY STATUS : %d", iskeygot);

            if (iskeygot && check)
                mvprintw(7, 0, "        KEY got! now find door");

            if (maze->player->player_node == maze->door && iskeygot)
                mvprintw(7, 0, "YOU ESCAPEd SUCCESS");
            attroff(COLOR_PAIR(5));
            mvprintw(8, 0, "__________________________________________________");

            refresh();

            a = getch();
            if (a == 'u')
            {
                maze->player->undo_z();
            }
            else if (a == 'q')
            {
                game_over();
                break;
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
            if (maze->player->player_node->isbomb == 1)
            {
                maze->player->player_node->isbomb = 0;
                game_over();
                refresh();
                break;
            }
            maze->collect_coins(maze->player->player_node);
        }
        game_over();
    }

    // void collect_bombs(Node *c)
    // {
    //     if (c->isbomb == 1 && maze->player->player_node == c)
    //     {
    //     }
    // }

    void start_display_gameover()
    {
        bool c = 1;
        while (c)
        {
            maze->print_gameover();
            bool check = true;
            mvprintw(0, 0, "__________________________________________________");
            attron(COLOR_PAIR(5));
            if (level == 1)
                mvprintw(2, 0, "               GAME_MODE : EASY           ");
            else if (level == 2)
                mvprintw(2, 0, "                GAME_MODE : MEDIUM         ");
            else
                mvprintw(2, 0, "               GAME_MODE : HARD           ");

            mvprintw(3, 0, "        MOVES LEFT : %d   UNDO MOVES: %d", maze->player->player_moves, maze->player->undo);
            mvprintw(4, 0, "        COINS : %d         SCORE: %d", maze->collected_coins, maze->player->player_score + maze->player->player_moves);
            mvprintw(5, 0, "                   KEY STATUS : %d", iskeygot);

            if (iskeygot && check)
                mvprintw(7, 0, "        KEY got! now find door");

            if (maze->player->player_node == maze->door && iskeygot)
                mvprintw(8, 0, "        YOU ESCAPED SUCCESSFULLY");
            attroff(COLOR_PAIR(5));
            maze->coin_s.display();
            mvprintw(10, 0, "__________________________________________________");
            c = 0;
        }
    }

    void game_over()
    {
        clear();
        bool n = true;

        start_display_gameover();
        nodelay(stdscr, TRUE);
        do
        {
            int x;
            if (level == 1)
                x = 26;
            else if (level == 2)
                x = 30;
            else
                x = 35;
            mvprintw(x, 0, "__________________________________________________");
            if (maze->door->isdoor == false)
            {
                attron(COLOR_PAIR(2));

                if (n)
                {
                    mvprintw(x + 1, 0, "     WINNER WINNER CHICKEN -DINNER ");
                    mvprintw(x + 2, 0, "                  O");
                    mvprintw(x + 3, 0, "                v-|-v");
                    mvprintw(x + 4, 0, "                 / \\");
                    n = false;
                }
                else
                {
                    mvprintw(x + 1, 0, "     WINNER WINNER CHICKEN -DINNER ");
                    mvprintw(x + 2, 0, "                  o");
                    mvprintw(x + 3, 0, "                ^~|~^");
                    mvprintw(x + 4, 0, "                  |");
                    n = true;
                }

                attroff(COLOR_PAIR(2)); // Turn off color

                mvprintw(x + 8, 0, "Press q to exit");
            }
            else
            {
                attron(COLOR_PAIR(1));
                if (n)
                {
                    mvprintw(x + 1, 0, "    MOYEE MOYEE");
                    mvprintw(x + 2, 0, "                  x");
                    mvprintw(x + 3, 0, "                --|--");
                    mvprintw(x + 4, 0, "                 / \\");
                    n = false;
                }
                else
                {
                    mvprintw(x + 1, 0, "    MOYEE MOYEE");
                    mvprintw(x + 2, 0, "                  X");
                    mvprintw(x + 3, 0, "                ~~|~~");
                    mvprintw(x + 4, 0, "                  |");
                    n = true;
                }
                attroff(COLOR_PAIR(1));
                mvprintw(x + 8, 0, "Press q to exit");
            }

            refresh();
            napms(150);

        } while (getch() != 'q');
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
    bool check = true;
    do
    {
        cout << "Select level (1 ,2 ,3) ";
        cin >> i;
        if (i == 1 || i == 2 || i == 3)
        {
            initscr(); // Start ncurses mode
            start_color();
            keypad(stdscr, TRUE); // Enable keypad for arrow keys
            noecho();             // Don't show typed characters
            cbreak();             // Disable line buffering

            init_pair(1, COLOR_RED, COLOR_BLACK);
            init_pair(2, COLOR_GREEN, COLOR_BLACK);
            init_pair(3, COLOR_BLUE, COLOR_BLACK);
            init_pair(4, COLOR_YELLOW, COLOR_BLACK);
            init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
            init_pair(6, COLOR_CYAN, COLOR_BLACK);

            GAME g(i);
            g.start();

            endwin(); // ending ncurse mode
            check = false;
        }
        else
        {
            cout << "Invalid Input!!\n";
            check = true;
        }
    } while (check);
    return 0;
}