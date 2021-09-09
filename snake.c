////////////////////////////////////////////////////////////////////////////////
///                                  snake.c                                	///
////////////////////////////////////////////////////////////////////////////////
//
// In this version, the game operates as follows:
// 	
// 	1. The snake is initially five blocks long, and is positioned somewhere
// 	   in the center of the game box.
// 	2. The snake does not move unless a directional key is pressed by the
// 	   player (big difference from the original snake game).
// 	3. The snake may not move over its own body. So, at any time, the snake
// 	   should only be able to move in at most three directions (less if near
// 	   a border).
// 	4. It doesn't matter what character you use to represent the snake's
// 	   body.
// 	3. The snake must stay within the boxed window. The player does not
// 	   loose the game by hitting a border of that box though. There is no
// 	   way for the player to loose the game in this version of the game in
// 	   fact.
// 	4. Exactly one cherry is present inside the box at all times, at a 
// 	   random location. The snake can pick it up and grow by one block
// 	   as a result. A new cherry should then by spawned at another location
// 	   (that does not overlap with the snake's body).
// 	5. The player can press 'q' at any time to exit out of the game.
//
//
// Some of the curses functions you may find useful to understand the code : getch(), addch(), move(),
// addstr(), mvaddch(), mvaddstr().
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

/*
 *The game window's dimensions.
 */
#define WIN_H 24
#define WIN_W 80
typedef struct node
{
	int row;
	int column;
	struct node * next;
}

Node;

typedef struct point
{
	int x;
	int y;
}

Point;

Node* insert_node_at_head(Node *head, Node *node);
int get_list_length(Node *head);
void free_list(Node *head);
void free_node(Node *node);
Node* delete_node(Node *head, Node *node);

Node* initialize_game(WINDOW *window);
WINDOW* initialize_window(void);
void cleanup(Node *head);

void randCherry(int *x, int *y);
Node* NodeMove(Node *ptr, int key, Node *head);
bool checkFlag(Node *conditioncheck, Node *head, int countLength, int key, int limit);

void free_node(Node *node)
{
	free(node);

}

Node* insert_node_at_head(Node *head, Node *node)
{
	if (head == NULL)
	{
		return node;
	}
	else
	{
		node->next = head;
	}

	return node;
}

int get_list_length(Node *head)
{
	int count = 0;
	Node *countLength = head;
	while (countLength != NULL)
	{
		countLength = countLength->next;
		count += 1;
	}

	return count;
}

void free_list(Node *head)
{
	if (head == NULL)
	{
		return;
	}

	Node *temp = head;
	Node * store;
	while (temp->next != NULL)
	{
		store = temp->next;
		free_node(temp);
		temp = store;
	}

	free_node(temp);
}

Node* delete_node(Node *head, Node *node)
{
	Node *ptr = head;
	Node * temp;

	if (head == node)
	{
		temp = node->next;
		node->next = NULL;
		free_node(node);
		return temp;
	}
	else
	{
		while (ptr != NULL)
		{
			if (ptr->next == node)
			{
				ptr->next = node->next;
				break;
			}

			ptr = ptr->next;
		}
	}

	node->next = NULL;
	free_node(node);
	return head;
}

/*
 *Almost all functions from the curses library return either ERR on error, or
 *OK on success. This enables us to create the following short routine for 
 *general error checking. Simply call this right after any such function call.
 */
void check(int status, char *function, int line)
{
	if (status == ERR)
	{
		fprintf(stderr, "[ERROR] on line %d: %s() failed.\n", line, function);
		exit(EXIT_FAILURE);
	}
}

/*
 *Initialization and paramterization of the game window is done in this
 *function. initscr() must be called first. Use the manual pages to find out
 *what the other functions do. This function calls refresh().
 */
WINDOW* initialize_window(void)
{
	int status;

	WINDOW *window = initscr();
	if (window == NULL)
	{
		fprintf(stderr, "[ERROR] on line %d: %s() failed.\n", 28, "initscr");
		exit(EXIT_FAILURE);
	}

	/*
	 *The window is parametrized below, but feel free to modify and/or
	 *expand on this.
	 */

	status = resizeterm(WIN_H, WIN_W);
	check(status, "resizeterm", __LINE__);

	status = cbreak();
	check(status, "cbreak", __LINE__);

	status = noecho();
	check(status, "noecho", __LINE__);

	status = keypad(window, TRUE);
	check(status, "keypad", __LINE__);

	status = refresh();
	check(status, "refresh", __LINE__);

	status = curs_set(0);
	check(status, "refresh", __LINE__);

	return window;
}

/*
 *Use this function to clean up any messes before exiting the program, lest
 *the terminal becomes unusable. If you are allocating memory on the heap,
 *you may want to use this function to free it.
 */
void cleanup(Node *head)
{
	free_list(head);
	endwin();
}

// Generates the location of cherry in the box
void randCherry(int *x, int *y)
{
	*y = (rand() % (WIN_H - 2)) + 1;
	*x = (rand() % (WIN_W - 2)) + 1;
}

/*The following function supports the movement of the node 
 	it ensures the movement of the node, when the cherry is eaten 
 it makes a new node and assigns it as head
*/
Node* NodeMove(Node *ptr, int key, Node *head)
{
	Node *node = malloc(sizeof(Node));

	if (key == KEY_LEFT)
	{
		node->row = ptr->row;
		node->column = (ptr->column - 1);
	}
	else if (key == KEY_RIGHT)
	{
		node->row = ptr->row;
		node->column = (ptr->column + 1);
	}
	else if (key == KEY_DOWN)
	{
		node->row = (ptr->row + 1);
		node->column = ptr->column;
	}
	else if (key == KEY_UP)
	{
		node->row = (ptr->row - 1);
		node->column = ptr->column;
	}
	else
	{
		return NULL;
	}

	return node;
}

Node* locomotion(Node *ptr, Node *head, int cherry, int key)
{
	int status;
	while(ptr != NULL)
	{
		if(ptr  == head)
		{
			switch(key)
			{
				case KEY_LEFT :
					if(ptr->column > 1)
					{
						Node *node = NodeMove(ptr,key,head);
						head = insert_node_at_head(head,node);
						mvprintw(node->row,node->column,"o");
						status = refresh();
						check(status, "refresh", __LINE__);	
					}
                    break;

				case KEY_RIGHT :
					if(ptr->column <= WIN_W-2)
					{
						Node *node = NodeMove(ptr,key,head);
						head = insert_node_at_head(head,node);
						mvprintw(node->row,node->column,"o");
						status = refresh();
						check(status, "refresh", __LINE__);
					}
                    break;

				case KEY_UP :
					if(ptr->row > 1)
					{
						Node *node = NodeMove(ptr,key,head);
						head = insert_node_at_head(head,node);
						mvprintw(node->row,node->column,"o");
						status = refresh();
						check(status, "refresh", __LINE__);
					}
                    break;

				case KEY_DOWN :
					if(ptr->row <= WIN_H-2)
					{
						Node *node = NodeMove(ptr,key,head);
						head = insert_node_at_head(head,node);
						mvprintw(node->row,node->column,"o");
						status = refresh();
						check(status, "refresh", __LINE__);
					}
                    break;

			}

		}

		else if(ptr -> next == NULL && cherry != 1)
		{
			mvaddch(ptr->row,ptr->column,' ');
			status = refresh();
			check(status, "refresh", __LINE__);
			head = delete_node(head,ptr);
			break;
		}

		ptr = ptr->next;
	}

	return head;
}

/*
 *Use this function to initialize the game elements (e.g. draw a box around
 *the window, initialize the snake's position, etc.).
 */
Node* initialize_game(WINDOW *window)
{
	int status;

	status = box(window, 0, 0);
	check(status, "box", __LINE__);

	/*Your code here. */
	Node *head = NULL;
	int flag = 0;
	int i;
	for (i = 0; i < 5; i++)
	{
		Node *node = malloc(sizeof(Node));
		node->row = (WIN_H / 2);
		node->column = ((WIN_W / 2) - i);
		node->next = NULL;

		head = insert_node_at_head(head, node);

	}

	status = refresh();
	check(status, "refresh", __LINE__);

	Node *ptr = head;
	while (ptr != NULL)
	{
		mvprintw(ptr->row, ptr->column, "o");
		status = refresh();
		check(status, "refresh", __LINE__);
		ptr = ptr->next;
	}

	move(head->row, head->column + 1);
	status = refresh();
	check(status, "refresh", __LINE__);

	return head;
}

/*The following checks the if the snake is crossing the border 
	on pressing of the UP, DOWN, LEFT, RIGHT key and it even checks
	if the snake is eating itself or not
*/
bool checkFlag(Node *conditioncheck, Node *head, int countLength, int key, int limit)
{
	int i;
	int row;
	int col;
	for (i = 0; i < countLength; i++)
	{
		if (key == KEY_LEFT || key == KEY_RIGHT)
		{
			if (conditioncheck == head)
			{
				if (conditioncheck->next->column == (conditioncheck->column - limit))
				{
					return false;
				}
				else if ((conditioncheck->column < 2 && key == KEY_LEFT) || (conditioncheck->column >= 78 && key == KEY_RIGHT))
				{
					return false;
				}

				row = conditioncheck->row;
				col = conditioncheck->column;

			}

			if ((col - limit) == conditioncheck->column && row == conditioncheck->row)
			{
				return false;
			}

			conditioncheck = conditioncheck->next;

		}
		else if (key == KEY_DOWN || KEY_UP)
		{
			if (conditioncheck == head)
			{
				if (conditioncheck->next->row == (conditioncheck->row + limit))
				{
					return false;
				}
				else if ((conditioncheck->row >= 22 && key == KEY_DOWN) || (conditioncheck->row <= 1 && key == KEY_UP))
				{
					return false;
				}

				row = conditioncheck->row;
				col = conditioncheck->column;

			}

			if (col == conditioncheck->column && (row + limit) == conditioncheck->row)
			{
				return false;
			}

			conditioncheck = conditioncheck->next;

		}
	}

	return true;

}

int main()
{
	/*
	 *The game loop. The player can press 'q' to exit out of it and end
	 *the game. To keep things simple, you need only worry about 
	 *implementing the snake's movement using the arrow keys. Of course,
	 *if you wish, feel free to add some functionality (e.g. a key to reset
	 *the game, a key to pause the game, etc.).
	 */

	int status;

	WINDOW *window = initialize_window();
	Node *head = initialize_game(window);

	Point *cherryMake = malloc(sizeof(Point));
	cherryMake->x = 0;
	cherryMake->y=0;

	randCherry(&cherryMake->x, &cherryMake->y);
	mvprintw(cherryMake->y, cherryMake->x, "o");

	while (1)
	{
		int cherry = 0;
		if (head->row == cherryMake->y && head->column == cherryMake->x)
		{
			cherry = 1;
			Node *ptr = head;

			randCherry(&cherryMake->x, &cherryMake->y);

			int endflag = 0;

			while (endflag != 1 && ptr != NULL)
			{
				ptr = head;
				while (ptr != NULL)
				{
					if (cherryMake->y == ptr->row && cherryMake->x == ptr->column)
					{
						randCherry(&cherryMake->x, &cherryMake->y);
						break;
					}

					ptr = ptr->next;
					if (ptr == NULL)
					{
						endflag = 1;
					}
				}
			}

			mvprintw(cherryMake->y, cherryMake->x, "o");
			status = refresh();
			check(status, "refresh", __LINE__);
		}

		int key = getch();

		if (key == KEY_LEFT)
		{
			Node *ptr = head;
			Node *conditioncheck = head;
			bool flag = checkFlag(conditioncheck, head, get_list_length(head), key, 1);
			if (flag == true)
			{
				head = locomotion(ptr,head,cherry,key);
				status = move(head->row, head->column - 1);
				check(status, "move", __LINE__);
			}
		}

		if (key == KEY_RIGHT)
		{
			Node *ptr = head;
			Node *conditioncheck = head;
			bool flag = checkFlag(conditioncheck, head, get_list_length(head), key, -1);

			if (flag == true)
			{
		
                head = locomotion(ptr,head,cherry,key);
				status = move(head->row, head->column + 1);
				check(status, "move", __LINE__);
			}
		}
		else if (key == KEY_UP)
		{
			Node *ptr = head;
			Node *conditioncheck = head;
			bool flag = checkFlag(conditioncheck, head, get_list_length(head), key, -1);

			if (flag == true)
			{
                head = locomotion(ptr,head,cherry,key);
				status = move(head->row - 1, head->column);
				check(status, "move", __LINE__);
			}
		}
		else if (key == KEY_DOWN)
		{
			Node *ptr = head;
			Node *conditioncheck = head;
			bool flag = checkFlag(conditioncheck, head, get_list_length(head), key, 1);
			if (flag == true)
			{
		
                head = locomotion(ptr,head,cherry,key);
				status = move(head->row + 1, head->column);
				check(status, "move", __LINE__);
			}
		}
		else if (key == 'q')
		{
			break;
		}
		else if (key == 'r')
		{
			free_list(head);
			erase();
			refresh();
			WINDOW *window = initialize_window();
			Node *head = initialize_game(window);
			randCherry(&cherryMake->x, &cherryMake->y);
			mvprintw(cherryMake->y, cherryMake->x, "o");
		}
	}

	cleanup(head);
	return EXIT_SUCCESS;
    
}