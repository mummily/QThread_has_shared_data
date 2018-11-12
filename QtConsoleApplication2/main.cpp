#include <QtCore/QCoreApplication>
#include <QVector>
#include <QReadWriteLock>
#include "QThread"
#include <iostream>
#include <memory>
using namespace std;

// QThread π≤œÌœÓ≤‚ ‘

template<class T>
class ThreadSafeVector : public QVector<T>
{
public:
    inline void push_back(const T &t)
    {
        QWriteLocker locker(&lock);
        QVector<T>::push_back(t);
    }

    const T &at(int i) const
    {
        QReadLocker locker(&lock);
        return QVector<T>::at(i);
    }
private:
    mutable QReadWriteLock lock;
};

class MyThread : public QThread
{
public:
    MyThread(ThreadSafeVector<int>* vec, int nFlag)
        : m_vec(vec), m_nFlag(nFlag)
    {}
    virtual ~MyThread() {};
protected:
    virtual void run();
private:
    ThreadSafeVector<int>* m_vec;
    int m_nFlag;
};

void MyThread::run()
{
    m_vec->push_back(m_nFlag * 10 + 1);
    m_vec->push_back(m_nFlag * 10 + 2);
    m_vec->push_back(m_nFlag * 10 + 3);
    m_vec->push_back(m_nFlag * 10 + 4);
    m_vec->push_back(m_nFlag * 10 + 5);
    m_vec->push_back(m_nFlag * 10 + 6);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto v = make_shared<ThreadSafeVector<int>>();
    QList<shared_ptr<MyThread>> lstThread;
    for (int nIndex = 0; nIndex < 4; ++nIndex)
    {
        auto t = make_shared<MyThread>(v.get(), nIndex);
        t->start();
        lstThread << t;
    }

    auto b = true;
    do 
    {
        b = std::all_of(lstThread.begin(), lstThread.end(), [&](shared_ptr<MyThread> t)->bool
        {
            return t->isFinished();
        });
    } while (!b);

    for (int nIndex = 0; nIndex < v->size(); ++nIndex)
    {
        cout << v->at(nIndex) << endl;
    }

    return a.exec();
}
