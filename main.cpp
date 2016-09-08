#include"cpu_mem.hpp"
int main(int argc,char **argv)
{
    QApplication a(argc,argv);
    cpu_mem w;
    w.show();
    QFile cpufile("/proc/stat");
    QFile memfile("/proc/meminfo");
    cpufile.open(QIODevice::ReadOnly);
    QTextStream cpustream(&cpufile);
    memfile.open(QIODevice::ReadOnly);
    QTextStream memstream(&memfile);
    w.run(cpustream,memstream);
    cpufile.close();
    memfile.close();
    //a.exec();
    return 0;
}
