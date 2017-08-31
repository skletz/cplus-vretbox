#include "Terminal.hpp"

void cplusutil::Terminal::showProgress(std::string _label, int _step, int _total)
{
	printf("\r");
	int tmptotal = _total;
	if (tmptotal == 0)
	{
		tmptotal = 1;
	}

	//progress width
	const int pwidth = 72;

	//minus label len
	int width = pwidth - int(_label.length());
	int pos = (_step * width) / tmptotal;


	int percent = (_step * 100) / tmptotal;


	printf("%s[", _label.c_str());

	//fill progress bar with =
	for (int i = 0; i < pos; i++)  printf("%c", '=');

	//fill progress bar with spaces
	printf("%*c", width - pos + 1, ']');
	printf(" %3d%%\r", percent);
}