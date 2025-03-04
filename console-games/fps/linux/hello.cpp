#include <locale.h>
#include <ncurses.h>

int main()
{
	setlocale(LC_ALL, "");

	initscr();
	wchar_t * msg = L"こんにちは, World! ☺";
	mvaddwstr(0, 0, msg);
	refresh();
	getch();
	endwin();

	return 0;
}
