#ifndef CPU_MEM_HPP
#define CPU_MEM_HPP
#include<QtGui>
class cpuuser{
private:
    long user,nice,sys,idle,io,irq,sirq;
    long init(const QString src,int n){
        QString tmp;
        int m=0;
        for(int i=0;i<src.size();++i){
            if('0'<=src[i]&&src[i]<='9')
                tmp.append(src[i]);
            else if(' '==src[i]&&!tmp.isEmpty())
            {
                if(m==n)
                    return tmp.toLong();
                ++m;
                tmp.clear();
            }
        }
        return 0;
    }

public:
    cpuuser(QTextStream &src){
        src.seek(0);
        QString tmp=src.readLine();
        user=init(tmp,0);
        nice=init(tmp,1);
        sys=init(tmp,2);
        idle=init(tmp,3);
        io=init(tmp,4);
        irq=init(tmp,5);
        sirq=init(tmp,6);
    }
    long diff_total(cpuuser &start){
        return (user+nice+sys+idle+io+irq+sirq)-(start.user+start.nice+start.sys+start.idle+start.io+start.irq+start.sirq);
    }
    long diff_idle(cpuuser &start){
        return (idle-start.idle);
    }
};
class cpu_mem:public QObject{
private:
    QSystemTrayIcon *trayicon;
    QWidget *widget;
    QHBoxLayout *layout;
    QLabel *cpu,*memory;
    QProgressBar *averagecpu,*memory1;
    int loop;
    int readmem(QTextStream &src,int n){
        int offset=0,lenght=0,ret;
        src.seek(0);
        while(n--)
            src.readLine();
        while(1){
            char c;
                src>>c;
                if('0'<=c&&c<='9')
                {
                    if(offset==0){
                        offset=src.pos();
                        --offset;
                    }
                    ++lenght;
                }
                if(c==' '&&offset!=0){
                    src.seek(offset);
                    QString tmp=src.read(lenght);
                    ret=tmp.toInt();
                    return ret;
                }
            }
    }
    void setvalue(bool i,int n){
        if(i)
            averagecpu->setValue(n);
        else
            memory1->setValue(n);
    }
public:
    cpu_mem():loop(1){
        //widget=new QWidget(0,Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
        widget=new QWidget(0,Qt::X11BypassWindowManagerHint);
        averagecpu=new QProgressBar(widget);memory1=new QProgressBar(widget);
        cpu=new QLabel(widget);memory=new QLabel(widget);
        cpu->setText("CPU");
        memory->setText("MEM");
        QFont font("Droid Sans",6);
        cpu->setFont(font);
        memory->setFont(font);
        widget->setGeometry(1100,10,100,15);
        cpu->setGeometry(0,0,15,15);
        memory->setGeometry(47,0,19,15);
        averagecpu->setGeometry(15,0,32,15);
        memory1->setGeometry(66,0,34,15);
        widget->setFixedSize(100,15);
        trayicon=new QSystemTrayIcon(widget);
        QIcon image("./cpu_mem.png");
        trayicon->setIcon(image);
        trayicon->show();
        cpu->installEventFilter(this);
        memory->installEventFilter(this);
        averagecpu->installEventFilter(this);
        memory1->installEventFilter(this);
    }
    ~cpu_mem(){
        delete widget;
    }
    void show(){
        widget->show();
    }
    void run(QTextStream &cpustream,QTextStream &memstream){
        int fmem,amem,showmem;
        int showcpu;
        amem=readmem(memstream,0);
        while(loop){
            fmem=readmem(memstream,1);
            fmem+=readmem(memstream,2);
            fmem+=readmem(memstream,3);
            showmem=(amem-fmem)*100/amem;
            setvalue(0,showmem);
            cpuuser first(cpustream);
            QEventLoop eventloop0;
            QTimer::singleShot(2000, &eventloop0, SLOT(quit())); //wait 2s
            eventloop0.exec();
            cpuuser second(cpustream);
            long dt=second.diff_total(first);
            long di=second.diff_idle(first);
            showcpu=(100*(dt-di))/dt;
            setvalue(1,showcpu);
        }
    }
    bool eventFilter(QObject *obj,QEvent *event){
        if(event->type()==QEvent::MouseButtonPress){
            QMouseEvent *meven=dynamic_cast<QMouseEvent *>(event);
            if(meven->button()==Qt::RightButton)
                loop=0;
        }
        return QObject::eventFilter(obj,event);
    }
};

#endif // CPU_MEM_HPP
