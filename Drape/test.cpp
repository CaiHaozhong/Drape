#include <string>
#include <QString>

void func(const char* c)
{

}
int main(int argc, char** argv)
{
	QString s = QString("123");
	std::string stdS = s.toStdString();
	func(s.toStdString().c_str());
	printf("%s\n",stdS.c_str());
	getchar();
}
